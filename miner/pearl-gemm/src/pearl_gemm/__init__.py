"""
pearl_gemm package

Provides CUDA kernels for Pearl GEMM with noising/denoising and PoW extraction.

The compiled CUDA extension (pearl_gemm_cuda) is built during installation.
If you see ModuleNotFoundError for 'pearl_gemm_cuda', the extension was not compiled.
Run:  pip install -e /path/to/pearl-gemm --no-build-isolation
"""

from __future__ import annotations

import importlib as _importlib
import sys as _sys

_MISSING_EXT_MSG = (
    "The compiled CUDA extension 'pearl_gemm_cuda' was not found.\n"
    "\n"
    "The .so file must be compiled before importing this package. Fix with:\n"
    "\n"
    "  # Option 1 — reinstall (recommended)\n"
    "  pip install -e /path/to/pearl-gemm --no-build-isolation\n"
    "\n"
    "  # Option 2 — build in-place explicitly\n"
    "  cd /path/to/pearl-gemm && python setup.py build_ext --inplace\n"
    "\n"
    "Original error: {original}"
)


def _load_cuda_extension():
    """Import pearl_gemm_cuda and raise a clear error if missing."""
    try:
        return _importlib.import_module("pearl_gemm_cuda")
    except ModuleNotFoundError as exc:
        raise ModuleNotFoundError(
            _MISSING_EXT_MSG.format(original=exc)
        ) from exc


# Fail fast and clearly if the .so is missing — before any sub-module import
# attempts the same and produces a cryptic traceback.
_cuda_ext = _load_cuda_extension()

# ---------------------------------------------------------------------------
# Re-export pearl_gemm_cuda symbols for the public API
# ---------------------------------------------------------------------------

HostSignalStatus = _cuda_ext.HostSignalStatus
get_host_signal_header = _cuda_ext.get_host_signal_header
get_host_signal_header_size = _cuda_ext.get_host_signal_header_size
get_host_signal_sync_size = _cuda_ext.get_host_signal_sync_size
get_required_scratchpad_bytes = _cuda_ext.get_required_scratchpad_bytes
kEALScaleFactorDenoise = _cuda_ext.kEALScaleFactorDenoise
kEBRScaleFactorDenoise = _cuda_ext.kEBRScaleFactorDenoise

# ---------------------------------------------------------------------------
# Sub-module imports — safe now that pearl_gemm_cuda is confirmed present
# ---------------------------------------------------------------------------

from . import pearl_gemm_interface  # noqa: E402
from .helpers import (  # noqa: E402
    HostSignalHeaderPinnedPool,
    ProofTileIndices,
    extract_indices,
    make_pow_target_tensor,
)
from .pearl_gemm_interface import (  # noqa: E402
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
