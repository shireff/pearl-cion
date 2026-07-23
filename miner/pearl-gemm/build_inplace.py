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

import setup as _setup

_setup._apply_ninja_patch()
_setup._init_submodules()

extensions = _setup._build_ext_modules()
if not extensions:
    print("No extensions to build.")
    sys.exit(0)

from torch.utils.cpp_extension import BuildExtension

cmd = BuildExtension()
cmd.extensions = extensions
cmd.inplace = 1
cmd.ensure_finalized()
cmd.run()

print("BUILD COMPLETE")
for ext in extensions:
    print(f"Extension: {ext.name}")
