%%bash
set -e

echo "=== Step 1/8: Installing system dependencies ==="
apt-get update -qq && apt-get install -y -qq ninja-build curl

echo "=== Step 2/8: Checking GPU ==="
if ! command -v nvidia-smi &> /dev/null; then
    echo "ERROR: nvidia-smi not found. This runtime does not have a GPU."
    echo "Please enable GPU runtime: Runtime -> Change runtime type -> Hardware accelerator -> GPU"
    exit 1
fi

GPU_NAME=$(nvidia-smi --query-gpu=name --format=csv,noheader | head -n1 || echo "Unknown")
echo "Detected GPU: $GPU_NAME"

case "$GPU_NAME" in
    *H100*|*H200*) export PEARL_GEMM_ARCH="arch=compute_90a,code=sm_90a" ;;
    *A100*|*A10G*) export PEARL_GEMM_ARCH="arch=compute_80,code=sm_80" ;;
    *L40*|*L4*)    export PEARL_GEMM_ARCH="arch=compute_86,code=sm_86" ;;
    *T4*)          export PEARL_GEMM_ARCH="arch=compute_75,code=sm_75" ;;
    *V100*)        export PEARL_GEMM_ARCH="arch=compute_70,code=sm_70" ;;
    *)             echo "WARNING: Unknown GPU '$GPU_NAME', defaulting to sm_90a (may fail on older GPUs)"; export PEARL_GEMM_ARCH="arch=compute_90a,code=sm_90a" ;;
esac
echo "Using PEARL_GEMM_ARCH=$PEARL_GEMM_ARCH"

echo "=== Step 3/8: Checking CUDA toolkit (nvcc) ==="
if ! command -v nvcc &> /dev/null; then
    if [ -x "/usr/local/cuda/bin/nvcc" ]; then
        export PATH="/usr/local/cuda/bin:$PATH"
        echo "Found nvcc at /usr/local/cuda/bin/nvcc"
    else
        echo "ERROR: nvcc not found. Installing CUDA toolkit..."
        apt-get install -y -qq cuda-toolkit-13-0 || apt-get install -y -qq cuda-toolkit || {
            echo "ERROR: Failed to install CUDA toolkit."
            echo "Please install CUDA toolkit manually or use a runtime with nvcc available."
            exit 1
        }
        if [ -x "/usr/local/cuda/bin/nvcc" ]; then
            export PATH="/usr/local/cuda/bin:$PATH"
        fi
    fi
fi

if ! command -v nvcc &> /dev/null; then
    echo "ERROR: nvcc still not found after installation attempt."
    exit 1
fi

NVCC_VERSION=$(nvcc --version | grep 'release' | awk '{print $5}' || echo "unknown")
echo "nvcc version: $NVCC_VERSION"

echo "=== Step 4/8: Downloading Pearl repository ==="
cd /content
rm -rf pearl.zip pearl
curl -sL https://github.com/pearl-research-labs/pearl/archive/refs/heads/main.zip -o pearl.zip
unzip -q pearl.zip
mv pearl-main pearl
cd pearl/miner/pearl-gemm

echo "=== Step 5/8: Installing uv ==="
curl -LsSf https://astral.sh/uv/install.sh | sh
export PATH="/root/.cargo/bin:$PATH"

echo "=== Step 6/8: Syncing Python dependencies ==="
cd /content/pearl
uv sync --package pearl-gemm-build-utils --package pearl-gemm --package vllm-miner --no-editable --refresh

echo "=== Step 7/8: Building pearl-gemm CUDA extension ==="
cd /content/pearl/miner/pearl-gemm
export MAX_JOBS=2

echo "Checking torch CUDA version compatibility..."
TORCH_CUDA_VERSION=$(uv run python -c "import torch; print(torch.version.cuda)" 2>/dev/null || echo "unknown")
echo "torch CUDA version: $TORCH_CUDA_VERSION"
if [ "$TORCH_CUDA_VERSION" != "$NVCC_VERSION" ] && [ "$TORCH_CUDA_VERSION" != "unknown" ]; then
    echo "WARNING: nvcc version ($NVCC_VERSION) does not match torch CUDA version ($TORCH_CUDA_VERSION)."
    echo "Build may fail. Consider using a runtime with matching CUDA toolkit."
fi

python build_inplace.py

echo "=== Step 8/8: Running tests ==="
cd /content/pearl
uv run pytest miner/pearl-gemm/tests miner/vllm-miner/tests -v
