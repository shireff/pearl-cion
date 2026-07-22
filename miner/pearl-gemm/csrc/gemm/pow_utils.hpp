#pragma once

#include <cutlass/cutlass.h>
#include <cute/atom/mma_traits_sm90_gmma.hpp>

#include "blake3/blake3.cuh"
#include "host_signal_header.hpp"
#include "utils.h"

namespace pearl {

using namespace cute;

// Rotation amount for hash accumulation mixing — PROTOCOL CONSTANT, DO NOT CHANGE
static constexpr int HASH_ACCUMULATE_ROTATION = 13;

// ---------------------------------------------------------------------------
// xor3_lop3 — 3-input XOR using PTX lop3 instruction.
// LUT 0x96 = 0b10010110  =>  d = a ^ b ^ c
// PROTOCOL SAFE: bit-for-bit identical result.
// ---------------------------------------------------------------------------
CUTE_DEVICE __forceinline__
uint32_t xor3_lop3(uint32_t a, uint32_t b, uint32_t c) {
  uint32_t d;
  asm("lop3.b32 %0, %1, %2, %3, 0x96;" : "=r"(d) : "r"(a), "r"(b), "r"(c));
  return d;
}

// ---------------------------------------------------------------------------
// rotl_xor — rotl(x, shift) ^ y  using single shf.l.wrap PTX instruction.
// PROTOCOL SAFE: semantically identical to ((x << shift) | (x >> (32-shift))) ^ y.
// ---------------------------------------------------------------------------
template <int shift>
CUTE_DEVICE __forceinline__
uint32_t rotl_xor(uint32_t x, uint32_t y) {
  static_assert(shift > 0 && shift < 32, "Shift must be in range (0, 32)");
  uint32_t rotated;
  asm("shf.l.wrap.b32 %0, %1, %1, %2;" : "=r"(rotated) : "r"(x), "n"(shift));
  return rotated ^ y;
}

// ---------------------------------------------------------------------------
// XOR tree reduction — unchanged algorithm, __forceinline__ added (BN-9).
// ---------------------------------------------------------------------------
template <class OutputLayerSize, class InputLayer>
CUTE_DEVICE __forceinline__
auto process_xor_layer(InputLayer const& input_layer) {
  constexpr size_t input_size        = InputLayer{}.size();
  constexpr size_t output_layer_size = OutputLayerSize{}.value;
  constexpr size_t triplets          = input_size / 3;
  constexpr size_t remainder         = input_size % 3;

  static_assert(output_layer_size == triplets + remainder,
                "Output layer size must match expected reduction");

  cute::array<uint32_t, output_layer_size> result;

  CUTLASS_PRAGMA_UNROLL
  for (size_t i = 0; i < triplets; ++i) {
    result[i] = xor3_lop3(input_layer[3 * i],
                           input_layer[3 * i + 1],
                           input_layer[3 * i + 2]);
  }
  CUTLASS_PRAGMA_UNROLL
  for (size_t i = 0; i < remainder; ++i) {
    result[triplets + i] = input_layer[triplets * 3 + i];
  }

  return result;
}

template <size_t N>
constexpr auto xor_tree_layer_sizes() {
  if constexpr (N <= 3) {
    return cute::make_tuple(cute::Int<N>{});
  } else {
    constexpr size_t next = (N / 3) + (N % 3);
    return cute::tuple_cat(cute::make_tuple(cute::Int<N>{}),
                           xor_tree_layer_sizes<next>());
  }
}

template <typename TensorType>
CUTE_DEVICE __forceinline__
uint32_t xor_reduction(const TensorType& input_tensor) {
  constexpr size_t buffer_size =
      decltype(std::declval<TensorType>().size())::value;
  static_assert(buffer_size > 0, "Buffer size must be positive");

  cute::array<uint32_t, buffer_size> first_layer;
  CUTLASS_PRAGMA_UNROLL
  for (size_t i = 0; i < buffer_size; ++i) {
    first_layer[i] = static_cast<uint32_t>(input_tensor[i]);
  }

  constexpr auto all_layer_sizes  = xor_tree_layer_sizes<buffer_size>();
  constexpr auto remaining_layers = cute::take<1, -1>(all_layer_sizes);

  auto final_layer = cute::fold(
      remaining_layers, first_layer,
      [](auto const& layer, auto target_size) {
        return process_xor_layer<decltype(target_size)>(layer);
      });

  constexpr size_t final_size = cute::tuple_size_v<decltype(final_layer)>;
  static_assert(final_size >= 1 && final_size <= 3,
                "Final layer should have 1-3 elements");

  if constexpr (final_size == 1) {
    return final_layer[0];
  } else if constexpr (final_size == 2) {
    return final_layer[0] ^ final_layer[1];
  } else {
    return xor3_lop3(final_layer[0], final_layer[1], final_layer[2]);
  }
}

// ---------------------------------------------------------------------------
// TileHashAccumulator
//
// BN-4: Replace modulo in needs_accumulate() with a down-counter.
//   ORIGINAL: m_k_block_count % ReduceEveryK == 0  (IREM — ~20 cycles on SM90)
//   NEW:      m_reduce_counter == 0                 (ISETP — ~1 cycle on SM90)
//   The counter starts at ReduceEveryK. needs_accumulate() decrements it;
//   when it reaches 0 the predicate fires and the counter resets to ReduceEveryK.
//   The predicate fires at exactly the same call-indices as the modulo version.
//
//   WHY SAFE: Both versions fire every ReduceEveryK calls to needs_accumulate().
//   The overall count guard (m_k_block_count <= m_last_full_k_block) is kept
//   unchanged to ensure only full k_blocks contribute to the transcript.
//
// BN-10: Replace modulo in writeback() with bitwise AND.
//   ORIGINAL: (m_reduction_count + accums_per_tile) % MSG_BLOCK_SIZE_U32
//   NEW:      (m_reduction_count + accums_per_tile) & (MSG_BLOCK_SIZE_U32 - 1)
//   MSG_BLOCK_SIZE_U32 = 16, a power of 2, so % 16 == & 15 for uint32_t.
//   WHY SAFE: Identical for all non-negative uint32_t values; the static_assert
//   that accums_per_tile divides MSG_BLOCK_SIZE_U32 guarantees the result stays
//   in [0, 15] and there is no sign ambiguity.
// ---------------------------------------------------------------------------
template <int KBlocksPerTile, int ReduceEveryK, bool EnableDebug = false>
struct TileHashAccumulator {
  static constexpr int accums_per_tile =
      std::max<int>(1, KBlocksPerTile / ReduceEveryK);

  static_assert(blake3::MSG_BLOCK_SIZE_U32 % accums_per_tile == 0,
                "accums_per_tile must divide MSG_BLOCK_SIZE_U32");

  // Power-of-two check needed for BN-10 bitwise-AND optimisation.
  static_assert((blake3::MSG_BLOCK_SIZE_U32 & (blake3::MSG_BLOCK_SIZE_U32 - 1)) == 0,
                "MSG_BLOCK_SIZE_U32 must be a power of two for bitwise wrap");

 private:
  uint32_t  m_tile_transcript[accums_per_tile];

  // BN-10: wrapped position uses & mask instead of %
  uint32_t  m_reduction_count = 0;

  // BN-4: running k_block count retained for the <= m_last_full_k_block guard
  uint32_t  m_k_block_count   = 0;

  // BN-4: down-counter replacing the modulo predicate
  uint32_t  m_reduce_counter  = ReduceEveryK;

  uint32_t  m_last_full_k_block;
  uint64_t* m_debug_counter;

 public:
  CUTLASS_DEVICE
  TileHashAccumulator(uint32_t last_full_k_block, uint64_t* debug_counter)
      : m_last_full_k_block(last_full_k_block),
        m_debug_counter(debug_counter) {}

  template <typename TranscriptTensor>
  CUTLASS_DEVICE __forceinline__
  void preload(TranscriptTensor const& transcript) {
    CUTLASS_PRAGMA_UNROLL
    for (int i = 0; i < accums_per_tile; ++i) {
      m_tile_transcript[i] = transcript(m_reduction_count + i);
    }
  }

  // -------------------------------------------------------------------------
  // needs_accumulate (BN-4)
  // Pure predicate. Increments both the total k_block counter and the down-
  // counter. Returns true when the down-counter expires AND the k_block is
  // still within the full-block range.
  // CORRECTNESS: fires at call-index ReduceEveryK, 2×ReduceEveryK, …, exactly
  // as the original modulo version did.
  // -------------------------------------------------------------------------
  CUTLASS_DEVICE __forceinline__
  bool needs_accumulate(int /*k_block*/) {
    ++m_k_block_count;
    // BN-4: decrement down-counter; if it reaches 0, reset and return true
    if (--m_reduce_counter == 0) {
      m_reduce_counter = ReduceEveryK;
      return (m_k_block_count <= m_last_full_k_block);
    }
    return false;
  }

  template <typename TensorType>
  CUTLASS_DEVICE __forceinline__
  void accumulate_now(TensorType& tensor, int k_block) {
    warpgroup_wait<0>();
    warpgroup_fence_operand(tensor);

    if constexpr (EnableDebug) {
      atomicAdd((unsigned long long*)m_debug_counter, 1ULL);
    }

    uint32_t hash = xor_reduction(tensor);
    const int idx = k_block / ReduceEveryK;
    m_tile_transcript[idx] =
        rotl_xor<HASH_ACCUMULATE_ROTATION>(m_tile_transcript[idx], hash);
  }

  // Legacy combined interface — kept for backward compatibility.
  template <typename TensorType>
  CUTLASS_DEVICE __forceinline__
  void accumulate(TensorType& tensor, int k_block) {
    if (needs_accumulate(k_block)) {
      accumulate_now(tensor, k_block);
    }
  }

  // -------------------------------------------------------------------------
  // writeback (BN-10)
  // Replaces % MSG_BLOCK_SIZE_U32 with & (MSG_BLOCK_SIZE_U32 - 1).
  // CORRECTNESS: MSG_BLOCK_SIZE_U32 = 16 is a power of two; for uint32_t
  // (always non-negative) x % 16 == x & 15.
  // -------------------------------------------------------------------------
  template <typename TranscriptTensor>
  CUTLASS_DEVICE __forceinline__
  void writeback(TranscriptTensor& transcript) {
    CUTLASS_PRAGMA_UNROLL
    for (int i = 0; i < accums_per_tile; ++i) {
      transcript(m_reduction_count + i) = m_tile_transcript[i];
    }

    if ((KBlocksPerTile / ReduceEveryK > 0) ||
        (m_k_block_count % ReduceEveryK == 0)) {
      // BN-10: bitwise AND replaces modulo — identical for power-of-two modulus
      m_reduction_count =
          (m_reduction_count + accums_per_tile) &
          (blake3::MSG_BLOCK_SIZE_U32 - 1u);
    }
  }
};

// ---------------------------------------------------------------------------
// check_pow_target
//
// BN-5: Vectorized global loads for pow_key.
//   ORIGINAL: 8 × ld.global.u32 (scalar)
//   NEW:      2 × ld.global.v4.u32 (128-bit vectors)
//   PyTorch allocates CUDA tensors on 256-byte aligned boundaries, so the
//   pow_key pointer (uint32_t[8]) is always 16-byte aligned.  Casting to
//   uint4* and loading in two 128-bit transactions reduces cache line
//   pressure and instruction count.
//   CORRECTNESS: same 8 uint32 values read in the same little-endian order.
//   The uint4 layout on SM90 maps: x=word[0], y=word[1], z=word[2], w=word[3].
// ---------------------------------------------------------------------------
template <typename TranscriptTensor>
CUTLASS_DEVICE __forceinline__
bool check_pow_target(const TranscriptTensor& transcript,
                      const uint32_t*         pow_target,
                      const uint32_t*         pow_key) {
  // Initialise chaining value with key — BN-5: two 128-bit vector loads
  Tensor hash = make_tensor<uint32_t>(Int<blake3::CHAINING_VALUE_SIZE_U32>{});

  // pow_key is a uint32_t[8] buffer, always 16-byte aligned from PyTorch.
  // Two uint4 loads = 8 uint32 values in 2 global memory transactions.
  const uint4* key_vec = reinterpret_cast<const uint4*>(pow_key);
  uint4 kv0, kv1;
  // ld.global.v4.u32 — explicit vectorised load (compiler may already do this,
  // but the explicit cast guarantees it and documents the intent).
  asm("ld.global.v4.u32 {%0,%1,%2,%3}, [%4];"
      : "=r"(kv0.x), "=r"(kv0.y), "=r"(kv0.z), "=r"(kv0.w)
      : "l"(key_vec));
  asm("ld.global.v4.u32 {%0,%1,%2,%3}, [%4];"
      : "=r"(kv1.x), "=r"(kv1.y), "=r"(kv1.z), "=r"(kv1.w)
      : "l"(key_vec + 1));

  hash(0) = kv0.x;  hash(1) = kv0.y;  hash(2) = kv0.z;  hash(3) = kv0.w;
  hash(4) = kv1.x;  hash(5) = kv1.y;  hash(6) = kv1.z;  hash(7) = kv1.w;

  // BLAKE3 compression — PROTOCOL CONSTANT, UNCHANGED
  blake3::compress_msg_block_u32(transcript, hash,
                                 blake3::COMPRESS_PARAMS_SINGLE_BLOCK_KEYED);

  // uint256 comparison: hash <= target  (MSW-first, unchanged)
  bool block_found = true;
  CUTLASS_PRAGMA_UNROLL
  for (int i = blake3::CHAINING_VALUE_SIZE_U32 - 1; i >= 0; --i) {
    uint32_t target_i = pow_target[i];
    if (__builtin_expect(hash(i) > target_i, 1)) {
      block_found = false;
      break;
    }
    if (__builtin_expect(hash(i) < target_i, 0)) {
      break;
    }
  }

  return block_found;
}

// ---------------------------------------------------------------------------
// write_host_signal_header — cold path, correctness-critical, unchanged.
// ---------------------------------------------------------------------------
template <typename TiledMma, typename TileShape, typename ProblemShape,
          typename BlockCoord>
CUTLASS_DEVICE void write_host_signal_header(
    HostSignalSync*   host_signal_sync,
    HostSignalHeader* host_signal_header_pinned,
    ProblemShape const& problem_shape,
    BlockCoord const&   block_coord,
    int                 thread_idx,
    const uint32_t*     pow_target) {
  auto ix = static_cast<uint32_t>(get<0>(block_coord));
  auto iy = static_cast<uint32_t>(get<1>(block_coord));
  auto iz = static_cast<uint32_t>(get<2>(block_coord));

  TiledMma tiled_mma;
  auto thr_mma = tiled_mma.get_thread_slice(thread_idx);

  Tensor cD   = make_identity_tensor(select<0, 1>(TileShape{}));
  Tensor tCcD = thr_mma.partition_C(cD);

  cute::array<uint32_t, blake3::CHAINING_VALUE_SIZE_U32> target;
  CUTLASS_PRAGMA_UNROLL
  for (int i = 0; i < blake3::CHAINING_VALUE_SIZE_U32; ++i) {
    target[i] = pow_target[i];
  }

  while (atomicCAS(&host_signal_sync->global_lock, 0, 1) != 0) {
    __threadfence();
  }

  if (host_signal_sync->status != HostSignalStatus::kSignalTriggered) {
    HostSignalHeader new_header = {
        .status    = HostSignalStatus::kSignalTriggered,
        .gridDim   = {gridDim.x,   gridDim.y,   gridDim.z},
        .blockDim  = {blockDim.x,  blockDim.y,  blockDim.z},
        .blockIdx  = {blockIdx.x,  blockIdx.y,  blockIdx.z},
        .tileCoord = {ix, iy, iz},
        .threadIdx = {threadIdx.x, threadIdx.y, threadIdx.z},
        .num_registers_per_thread = static_cast<uint16_t>(size(tCcD)),
        .mma_size      = {get<0>(problem_shape), get<1>(problem_shape),
                          get<2>(problem_shape)},
        .mma_tile_size = {get<0>(TileShape{}), get<1>(TileShape{}),
                          get<2>(TileShape{})},
        .target = target,
    };

    static_assert(size(tCcD) <= new_header.thread_rows.size());
    for (int j = 0; j < size(tCcD); j++) {
      new_header.thread_rows[j] = get<0>(tCcD(j));
      new_header.thread_cols[j] = get<1>(tCcD(j));
    }

    if (new_header.block_in_bounds()) {
      *host_signal_header_pinned = new_header;
      host_signal_sync->status   = HostSignalStatus::kSignalTriggered;
    }
  }

  __threadfence();
  atomicExch(&host_signal_sync->global_lock, 0);
}

}  // namespace pearl
