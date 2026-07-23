#!/usr/bin/env python3
"""Build pearl_gemm_cuda in-place using subprocess."""

from __future__ import annotations

import os
import sys
import subprocess
import tempfile
import shutil

ROOT_DIR = os.path.abspath(os.path.dirname(__file__))

os.chdir(ROOT_DIR)

_cuda_home = os.environ.get("CUDA_HOME", "/usr/local/cuda")
if os.path.isdir(_cuda_home) and _cuda_home not in os.environ.get("PATH", ""):
    os.environ["PATH"] = _cuda_home + os.pathsep + os.environ.get("PATH", "")
os.environ.setdefault("CUDA_HOME", _cuda_home)

tmpdir = tempfile.mkdtemp(prefix="pearl_build_")
log_path = os.path.join(tmpdir, "build.log")

env = os.environ.copy()
env["TORCH_EXTENSION_LOG_LEVEL"] = "DEBUG"

proc = subprocess.run(
    [sys.executable, "setup.py", "build_ext", "--inplace"],
    env=env,
    capture_output=True,
    text=True,
    cwd=ROOT_DIR,
)

combined_output = proc.stdout + "\n" + proc.stderr
with open(log_path, "w", encoding="utf-8") as f:
    f.write(combined_output)

for line in combined_output.splitlines():
    if ".cu:" in line or ".cuh:" in line or ".hpp:" in line:
        print(f"ERROR_FILE_LINE: {line.strip()}")

if proc.returncode != 0:
    print("BUILD FAILED")
    shutil.rmtree(tmpdir, ignore_errors=True)
    sys.exit(1)

print("BUILD COMPLETE")
shutil.rmtree(tmpdir, ignore_errors=True)
