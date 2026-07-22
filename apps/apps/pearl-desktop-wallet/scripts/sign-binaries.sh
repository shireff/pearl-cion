#!/bin/bash

# Sign oyster macOS binaries with Developer ID Application certificate
# This should be run before building the Electron app
#
# Note: Only macOS binaries need Apple code signing for notarization
# Linux and Windows binaries are not signed with Apple certificates

set -e

CERT_NAME="Developer ID Application"
BIN_DIR="$(dirname "$0")/../bin"

echo "Signing macOS oyster binaries..."

# Sign x64 binary
if [ -f "$BIN_DIR/oyster-darwin-x64" ]; then
  echo "Signing oyster-darwin-x64..."
  codesign --force --sign "$CERT_NAME" \
    --options runtime \
    --timestamp \
    "$BIN_DIR/oyster-darwin-x64"
  echo "✓ Signed oyster-darwin-x64"
else
  echo "⚠ Warning: oyster-darwin-x64 not found"
fi

# Sign arm64 binary
if [ -f "$BIN_DIR/oyster-darwin-arm64" ]; then
  echo "Signing oyster-darwin-arm64..."
  codesign --force --sign "$CERT_NAME" \
    --options runtime \
    --timestamp \
    "$BIN_DIR/oyster-darwin-arm64"
  echo "✓ Signed oyster-darwin-arm64"
else
  echo "⚠ Warning: oyster-darwin-arm64 not found"
fi
