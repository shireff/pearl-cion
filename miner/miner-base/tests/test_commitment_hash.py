import secrets

import pytest
import torch
from miner_base.commitment_hash import CommitmentHasher
from pearl_gateway.comm.dataclasses import CommitmentHash
from pearl_gateway.comm.mining_configuration import MiningConfiguration
from pearl_mining import IncompleteBlockHeader


@pytest.fixture
def incomplete_header_bytes() -> bytes:
    """Generate a random header bytes."""
    return secrets.token_bytes(IncompleteBlockHeader.SERIALIZED_SIZE)


@pytest.fixture
def mining_config(default_matmul_config) -> MiningConfiguration:
    """Generate a test mining configuration."""
    return default_matmul_config.mining_config


class TestCommitmentHasher:
    """Test suite for CommitmentHasher class."""

    def test_commitment_hash_basic(self, incomplete_header_bytes, mining_config):
        """Test basic commitment hash functionality."""
        A = torch.randint(-128, 127, (8, 8), dtype=torch.int8)
        B = torch.randint(-128, 127, (8, 8), dtype=torch.int8)

        result = CommitmentHasher.commitment_hash(A, B, incomplete_header_bytes, mining_config)

        assert isinstance(result, CommitmentHash)
        assert len(result.noise_seed_A) == 32
        assert len(result.noise_seed_B) == 32

    def test_commitment_hash_deterministic(self, incomplete_header_bytes, mining_config):
        """Test that commitment hash is deterministic."""
        A = torch.randint(-128, 127, (8, 8), dtype=torch.int8)
        B = torch.randint(-128, 127, (8, 8), dtype=torch.int8)

        result1 = CommitmentHasher.commitment_hash(A, B, incomplete_header_bytes, mining_config)
        result2 = CommitmentHasher.commitment_hash(A, B, incomplete_header_bytes, mining_config)

        assert result1 == result2

    def test_commitment_hash_different_order(self, incomplete_header_bytes, mining_config):
        """Test that different order produce different hashes."""

        A = torch.zeros((8, 8), dtype=torch.int8)
        B = torch.ones((8, 8), dtype=torch.int8)

        result1 = CommitmentHasher.commitment_hash(A, B, incomplete_header_bytes, mining_config)
        result2 = CommitmentHasher.commitment_hash(B, A, incomplete_header_bytes, mining_config)

        assert result1 != result2

    def test_commitment_hash_invalid_dimensions(self, incomplete_header_bytes, mining_config):
        """Test error handling for non-2D tensors."""

        # 1D tensor
        tensor_1d = torch.randint(-128, 127, (8,), dtype=torch.int8)
        with pytest.raises(ValueError, match="Expected 2D tensor"):
            CommitmentHasher.commitment_hash(
                tensor_1d, tensor_1d, incomplete_header_bytes, mining_config
            )

        # 3D tensor
        tensor_3d = torch.randint(-128, 127, (8, 8, 3), dtype=torch.int8)
        with pytest.raises(ValueError, match="Expected 2D tensor"):
            CommitmentHasher.commitment_hash(
                tensor_3d, tensor_3d, incomplete_header_bytes, mining_config
            )
