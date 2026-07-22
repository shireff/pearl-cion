rpcclient
=========

[![Build Status](https://github.com/pearl-research-labs/pearl/workflows/Build%20and%20Test/badge.svg)](https://github.com/pearl-research-labs/pearl/actions)
[![ISC License](http://img.shields.io/badge/license-ISC-blue.svg)](http://copyfree.org)
[![GoDoc](https://img.shields.io/badge/godoc-reference-blue.svg)](https://pkg.go.dev/github.com/pearl-research-labs/pearl/node/rpcclient)

rpcclient implements a Websocket-enabled Pearl JSON-RPC client package written
in [Go](http://golang.org/).  It provides a robust and easy to use client for
interfacing with a Pearl RPC server that uses a pearld-compatible
Pearl JSON-RPC API.

## Status

This package is currently under active development.  It is already stable and
the infrastructure is complete.  However, there are still several RPCs left to
implement and the API is not stable yet.

## Documentation

* [API Reference](https://pkg.go.dev/github.com/pearl-research-labs/pearl/node/rpcclient)
* [pearld Websockets Example](https://github.com/pearl-research-labs/pearl/tree/master/node/rpcclient/examples/btcdwebsockets)
  Connects to a pearld RPC server using TLS-secured websockets, registers for
  block connected and block disconnected notifications, and gets the current
  block count
* [Oyster Websockets Example](https://github.com/pearl-research-labs/pearl/tree/master/node/rpcclient/examples/btcwalletwebsockets)
  Connects to an Oyster RPC server using TLS-secured websockets, registers for
  notifications about changes to account balances, and gets a list of unspent
  transaction outputs (utxos) the wallet can sign
* [HTTP POST Example](https://github.com/pearl-research-labs/pearl/tree/master/node/rpcclient/examples/bitcoincorehttp)
  Connects to an RPC server using HTTP POST mode with TLS disabled
  and gets the current block count

## Major Features

* Supports Websockets (pearld/Oyster) and HTTP POST mode (compatible fork)
* Provides callback and registration functions for pearld/Oyster notifications
* Supports pearld extensions
* Translates to and from higher-level and easier to use Go types
* Offers a synchronous (blocking) and asynchronous API
* When running in Websockets mode (the default):
  * Automatic reconnect handling (can be disabled)
  * Outstanding commands are automatically reissued
  * Registered notifications are automatically reregistered
  * Back-off support on reconnect attempts

## Installation

This package is part of the `github.com/pearl-research-labs/pearl` module. Use it
by adding the module as a dependency in your project.

## License

Package rpcclient is licensed under the [copyfree](http://copyfree.org) ISC
License.
