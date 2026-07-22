testhelper
==========

[![Build Status](https://github.com/pearl-research-labs/pearl/workflows/Build%20and%20Test/badge.svg)](https://github.com/pearl-research-labs/pearl/actions)
[![ISC License](http://img.shields.io/badge/license-ISC-blue.svg)](http://copyfree.org)
[![GoDoc](https://img.shields.io/badge/godoc-reference-blue.svg)](https://pkg.go.dev/github.com/pearl-research-labs/pearl/node/blockchain/internal/testhelper)

Package testhelper provides functions that are used internally in the
blockchain and blockchain/fullblocktests packages to test consensus
validation rules.  Mainly provided to avoid dependency cycles internally among
the different packages in pearld.

## License

Package testhelper is licensed under the [copyfree](http://copyfree.org) ISC
License.
