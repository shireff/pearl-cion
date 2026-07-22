bloom
=====

[![ISC License](http://img.shields.io/badge/license-ISC-blue.svg)](http://copyfree.org)
[![GoDoc](http://img.shields.io/badge/godoc-reference-blue.svg)](http://godoc.org/github.com/pearl-research-labs/pearl/node/btcutil/bloom)

Package bloom provides an API for dealing with Pearl-specific bloom filters.

A comprehensive suite of tests is provided to ensure proper functionality.  See
`test_coverage.txt` for the gocov coverage report.  Alternatively, if you are
running a POSIX OS, you can run the `cov_report.sh` script for a real-time
report.

## Installation

This package is part of the `github.com/pearl-research-labs/pearl` module. Use it as a dependency in your Go project:

```bash
go get github.com/pearl-research-labs/pearl
```

## Examples

* [NewFilter Example](http://godoc.org/github.com/pearl-research-labs/pearl/node/btcutil/bloom#example-NewFilter)  
  Demonstrates how to create a new bloom filter, add a transaction hash to it,
  and check if the filter matches the transaction.

## License

Package bloom is licensed under the [copyfree](http://copyfree.org) ISC
License.
