# Pearl Desktop Wallet

A modern, secure desktop wallet application for the Pearl blockchain network. Built with Electron, React, and TypeScript.

## Prerequisites

- **Go** >= 1.26.1
- **Rust** (stable toolchain) — required to build ZK proof libraries
- **Task** (`go-task`) — task runner for the backend build
- **Node.js** >= 22.0.0
- **pnpm** >= 8.15.6

## Setup

### 1. Build the oyster binary (backend wallet daemon)

The desktop wallet depends on the `oyster` binary from the root pearl repository. Build it from the repo root:

```bash
# From the pearl repo root
task build:blockchain
```

This compiles all blockchain binaries (including `oyster`) into `pearl/bin/`.

> **Note:** `task build:blockchain` requires Rust (for ZK/XMSS FFI libraries) and CGO. Make sure your toolchain is set up before running this.

### 2. Copy oyster to the wallet's bin directory

Copy the compiled binary to `apps/apps/pearl-desktop-wallet/bin/`, renaming it to match your OS and architecture:

| Platform          | Binary name              |
|-------------------|--------------------------|
| macOS (Apple Silicon) | `oyster-darwin-arm64`  |
| macOS (Intel)     | `oyster-darwin-x64`      |
| Linux (x64)       | `oyster-linux-x64`       |
| Windows (x64)     | `oyster-windows-x64.exe` |

**macOS (Apple Silicon):**
```bash
cp bin/oyster apps/apps/pearl-desktop-wallet/bin/oyster-darwin-arm64
```

**macOS (Intel):**
```bash
cp bin/oyster apps/apps/pearl-desktop-wallet/bin/oyster-darwin-x64
```

**Linux:**
```bash
cp bin/oyster apps/apps/pearl-desktop-wallet/bin/oyster-linux-x64
```

**Windows** (PowerShell):
```powershell
Copy-Item bin\oyster.exe apps\apps\pearl-desktop-wallet\bin\oyster-windows-x64.exe
```

### 3. Install frontend dependencies

```bash
# From pearl/apps
cd apps
pnpm install
```

## Development

Run the wallet in development mode (hot-reload):

```bash
# From pearl/apps
pnpm --filter @pearl/pearl-desktop-wallet dev

# Or from pearl/apps/apps/pearl-desktop-wallet
pnpm dev
```

## Building

Build the Electron app:

```bash
# From pearl/apps
pnpm --filter @pearl/pearl-desktop-wallet build

# Or from pearl/apps/apps/pearl-desktop-wallet
pnpm build
```

Build a distributable for your platform:

```bash
# macOS
pnpm build:mac

# Linux
pnpm build:linux

# Windows
pnpm build:win
```

Output is placed in `dist/`.

## Viewing Logs

```bash
# Tail live logs
pnpm logs

# Open logs directory in Finder (macOS)
pnpm logs:open
```

## Project Structure

```
pearl-desktop-wallet/
├── bin/                  # Platform-specific oyster binaries (not committed)
├── src/
│   ├── main/             # Electron main process
│   ├── preload/          # Preload scripts
│   ├── renderer/         # React frontend
│   ├── types/            # Shared TypeScript types
│   └── utils/            # Shared utilities
├── resources/            # Static assets bundled with the app
├── electron.vite.config.ts
├── electron-builder.json
└── package.json
```
