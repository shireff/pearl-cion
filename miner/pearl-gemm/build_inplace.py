#!/usr/bin/env python3
"""Build pearl_gemm_cuda in-place."""

from __future__ import annotations

import os
import sys

ROOT_DIR = os.path.abspath(os.path.dirname(__file__))
SRC_DIR = os.path.join(ROOT_DIR, "src")

if SRC_DIR not in sys.path:
    sys.path.insert(0, SRC_DIR)

os.chdir(ROOT_DIR)

_cuda_home = os.environ.get("CUDA_HOME", "/usr/local/cuda")
if os.path.isdir(_cuda_home) and _cuda_home not in os.environ.get("PATH", ""):
    os.environ["PATH"] = _cuda_home + os.pathsep + os.environ.get("PATH", "")
os.environ.setdefault("CUDA_HOME", _cuda_home)

import setup as _setup

_setup._apply_ninja_patch()
_setup._init_submodules()

extensions = _setup._build_ext_modules()
if not extensions:
    print("No extensions to build.")
    sys.exit(0)

from setuptools import setup

setup(
    name="pearl_gemm_build_inplace",
    ext_modules=extensions,
    cmdclass={"build_ext": _setup.PearlBuildExtension},
    script_args=["build_ext", "--inplace"],
)

print("BUILD COMPLETE")
for ext in extensions:
    print(f"Extension: {ext.name}")
