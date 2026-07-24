%%bash
set -e

apt-get update -qq && apt-get install -y -qq ninja-build curl

cd /content
rm -rf pearl.zip pearl
curl -sL https://github.com/shireff/pearl-cion/archive/refs/heads/main.zip -o pearl.zip
unzip -q pearl.zip
mv pearl-cion-main pearl
cd pearl/miner/pearl-gemm

curl -LsSf https://astral.sh/uv/install.sh | sh
export PATH="/root/.cargo/bin:$PATH"

python3 -c "
import pathlib, sys

# Patch build_routing_data.cu for CUDA 12.8 CUB API compatibility
p = pathlib.Path('csrc/moe/build_routing_data.cu')
text = p.read_text()
old = '''cub::DeviceTransform::Transform(
      slot_indices, routing_data, numel,
      [top_k_value] __device__(int32_t slot) { return slot / top_k_value; },
      stream);'''
new = '''cub::DeviceFor::Bulk(
      numel,
      [slot_indices, routing_data, top_k_value] __device__(int32_t idx) {
        routing_data[idx] = slot_indices[idx] / top_k_value;
      },
      stream);'''
if old in text:
    text = text.replace(old, new)
    p.write_text(text)
    print('Patched build_routing_data.cu')
else:
    print('Skipped build_routing_data.cu (already patched)')

# Patch __init__.py to register pearl_gemm_cuda in sys.modules
p = pathlib.Path('src/pearl_gemm/__init__.py')
text = p.read_text()
old = '_cuda_ext = _load_cuda_extension()'
new = '''_cuda_ext = _load_cuda_extension()
_sys.modules.setdefault('pearl_gemm_cuda', _cuda_ext)'''
if old in text and '_sys.modules.setdefault' not in text:
    text = text.replace(old, new)
    p.write_text(text)
    print('Patched __init__.py')
else:
    print('Skipped __init__.py (already patched)')

# Do NOT patch pearl_gemm_interface.py, moe.py, helpers.py
# They already have correct relative imports on main branch
print('Skipped Python import patches (already correct on main)')
"

python build_inplace.py

uv sync --package vllm-miner

cd /content/pearl
uv run pytest miner/pearl-gemm/tests miner/vllm-miner/tests -v
