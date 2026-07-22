rpctest
=======

[![Build Status](https://github.com/pearl-research-labs/pearl/workflows/Build%20and%20Test/badge.svg)](https://github.com/pearl-research-labs/pearl/actions)
[![ISC License](http://img.shields.io/badge/license-ISC-blue.svg)](http://copyfree.org)
[![GoDoc](https://img.shields.io/badge/godoc-reference-blue.svg)](https://pkg.go.dev/github.com/pearl-research-labs/pearl/node/integration/rpctest)

Package rpctest provides a pearld-specific RPC testing harness crafting and
executing integration tests by driving a `pearld` instance via the `RPC`
interface. Each instance of an active harness comes equipped with a simple
in-memory HD wallet capable of properly syncing to the generated chain,
creating new addresses, and crafting fully signed transactions paying to an
arbitrary set of outputs.

This package was designed specifically to act as an RPC testing harness for
`pearld`. However, the constructs presented are general enough to be adapted to
any project wishing to programmatically drive a `pearld` instance of its
systems/integration tests.

## Installation

This package is part of the `github.com/pearl-research-labs/pearl` module. Use it as a dependency in your Go project:

```bash
go get github.com/pearl-research-labs/pearl
```

## License

Package rpctest is licensed under the [copyfree](http://copyfree.org) ISC
License.
