"""
pearl_gemm package

This package provides CUDA kernels for Pearl GEMM with noising/denoising and PoW extraction.

The compiled CUDA extension (pearl_gemm_cuda) is built separately via `build_ext`.
Importing this package without the compiled extension raises a clear error rather
than a confusing ModuleNotFoundError.
"""

import importlib as _importlib


def _require_cuda_extension():
    """Raise a helpful error if the compiled CUDA extension is missing."""
    try:
        return _importlib.import_module("pearl_gemm_cuda")
    except ModuleNotFoundError as exc:
        raise ModuleNotFoundError(
            "The compiled CUDA extension 'pearl_gemm_cuda' was not found.\n"
            "Build it with:\n"
            "  pip install -e . --no-build-isolation\n"
            "or explicitly:\n"
            "  python setup.py build_ext --inplace\n"
            "\nOriginal error: " + str(exc)
        ) from exc


_cuda_ext = _require_cuda_extension()

# Re-export pearl_gemm_cuda utilities for cleaner API
HostSignalStatus = _cuda_ext.HostSignalStatus
get_host_signal_header = _cuda_ext.get_host_signal_header
get_host_signal_header_size = _cuda_ext.get_host_signal_header_size
get_host_signal_sync_size = _cuda_ext.get_host_signal_sync_size
get_required_scratchpad_bytes = _cuda_ext.get_required_scratchpad_bytes
kEALScaleFactorDenoise = _cuda_ext.kEALScaleFactorDenoise
kEBRScaleFactorDenoise = _cuda_ext.kEBRScaleFactorDenoise

from . import pearl_gemm_interface
from .helpers import (
    HostSignalHeaderPinnedPool,
    ProofTileIndices,
    extract_indices,
    make_pow_target_tensor,
)
from .pearl_gemm_interface import (
    commitment_hash_from_merkle_roots,
    denoise_converter,
    gemm,
    noise_A,
    noise_B,
    noise_gen,
    noisy_gemm,
    tensor_hash,
)

__all__ = [
    "HostSignalHeaderPinnedPool",
    "HostSignalStatus",
    "ProofTileIndices",
    "commitment_hash_from_merkle_roots",
    "denoise_converter",
    "extract_indices",
    "gemm",
    "get_host_signal_header",
    "get_host_signal_header_size",
    "get_host_signal_sync_size",
    "get_required_scratchpad_bytes",
    "kEALScaleFactorDenoise",
    "kEBRScaleFactorDenoise",
    "make_pow_target_tensor",
    "noise_A",
    "noise_B",
    "noise_gen",
    "noisy_gemm",
    "pearl_gemm_interface",
    "tensor_hash",
]
