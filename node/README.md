# pearld

[![Build Status](https://github.com/pearl-research-labs/pearl/workflows/Build%20and%20Test/badge.svg)](https://github.com/pearl-research-labs/pearl/actions)
[![ISC License](https://img.shields.io/badge/license-ISC-blue.svg)](http://copyfree.org)
[![GoDoc](https://img.shields.io/badge/godoc-reference-blue.svg)](https://pkg.go.dev/github.com/pearl-research-labs/pearl/node)

pearld is the reference implementation of the Pearl Protocol. It is a full node
that downloads, validates, and serves the Pearl blockchain. pearld includes
zero-knowledge proof-of-work verification and XMSS post-quantum signature
support.

pearld properly relays newly mined blocks, maintains a transaction pool, and
relays individual transactions that have not yet made it into a block. It
ensures all transactions admitted to the pool follow the consensus rules and
also includes stricter checks which filter transactions based on miner
requirements ("standard" transactions).

pearld does *not* include wallet functionality. That is provided by the
[Oyster wallet](https://github.com/pearl-research-labs/pearl/tree/master/wallet).

## Requirements

- [Go](https://golang.org) 1.26 or newer
- [Rust](https://rustup.rs) toolchain (for ZK verification library)
- C compiler (for XMSS library)
- [Task](https://taskfile.dev) runner

## Building

From the repository root:

```bash
task build:pearld
```

Or to build all binaries (pearld, prlctl, oyster):

```bash
task build:blockchain
```

Binaries are placed in `bin/`.

## Getting Started

pearld has several configuration options available to tweak how it runs, but all
of the basic operations work with zero configuration.

```bash
./bin/pearld
```

See [sample-pearld.conf](sample-pearld.conf) for the full list of options.

## Documentation

Documentation is located in the [docs](docs/) folder.

## Issue Tracker

The [integrated GitHub issue tracker](https://github.com/pearl-research-labs/pearl/issues)
is used for this project.

## License

pearld is licensed under the [copyfree](http://copyfree.org) ISC License.
