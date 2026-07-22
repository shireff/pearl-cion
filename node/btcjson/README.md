btcjson
=======

[![Build Status](https://github.com/pearl-research-labs/pearl/workflows/Build%20and%20Test/badge.svg)](https://github.com/pearl-research-labs/pearl/actions)
[![ISC License](http://img.shields.io/badge/license-ISC-blue.svg)](http://copyfree.org)
[![GoDoc](https://img.shields.io/badge/godoc-reference-blue.svg)](https://pkg.go.dev/github.com/pearl-research-labs/pearl/node/btcjson)

Package btcjson implements concrete types for marshalling to and from the
Pearl JSON-RPC API.  A comprehensive suite of tests is provided to ensure
proper functionality.

Although this package was primarily written for Pearl, it has
intentionally been designed so it can be used as a standalone package for any
projects needing to marshal to and from Pearl JSON-RPC requests and responses.

Note that although it's possible to use this package directly to implement an
RPC client, it is not recommended since it is only intended as an infrastructure
package.  Instead, RPC clients should use the
[rpcclient](https://github.com/pearl-research-labs/pearl/tree/master/node/rpcclient) package which provides
a full blown RPC client with many features such as automatic connection
management, websocket support, automatic notification re-registration on
reconnect, and conversion from the raw underlying RPC types (strings, floats,
ints, etc) to higher-level types with many nice and useful properties.

## Installation

This package is part of the `github.com/pearl-research-labs/pearl` module. Use it
by adding the module as a dependency in your project.

## Examples

* [Marshal Command](https://pkg.go.dev/github.com/pearl-research-labs/pearl/node/btcjson#example-MarshalCmd)  
  Demonstrates how to create and marshal a command into a JSON-RPC request.

* [Unmarshal Command](https://pkg.go.dev/github.com/pearl-research-labs/pearl/node/btcjson#example-UnmarshalCmd)  
  Demonstrates how to unmarshal a JSON-RPC request and then unmarshal the
  concrete request into a concrete command.

* [Marshal Response](https://pkg.go.dev/github.com/pearl-research-labs/pearl/node/btcjson#example-MarshalResponse)  
  Demonstrates how to marshal a JSON-RPC response.

* [Unmarshal Response](https://pkg.go.dev/github.com/pearl-research-labs/pearl/node/btcjson#example-package--UnmarshalResponse)  
  Demonstrates how to unmarshal a JSON-RPC response and then unmarshal the
  result field in the response to a concrete type.

## License

Package btcjson is licensed under the [copyfree](http://copyfree.org) ISC
License.
