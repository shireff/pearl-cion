btcutil
=======

[![Build Status](https://github.com/pearl-research-labs/pearl/workflows/Build%20and%20Test/badge.svg)](https://github.com/pearl-research-labs/pearl/actions)
[![ISC License](https://img.shields.io/badge/license-ISC-blue.svg)](http://copyfree.org)
[![GoDoc](https://img.shields.io/badge/godoc-reference-blue.svg)](https://godoc.org/github.com/pearl-research-labs/pearl/node/btcutil)

Package btcutil provides Pearl-specific convenience functions and types.
A comprehensive suite of tests is provided to ensure proper functionality.  See
`test_coverage.txt` for the gocov coverage report.  Alternatively, if you are
running a POSIX OS, you can run the `cov_report.sh` script for a real-time
report.

This package was developed for pearld, an alternative full-node implementation of
Pearl.  Although it was primarily written for pearld, this package has
intentionally been designed so it can be used as a standalone package for any
projects needing the functionality provided.

## Installation

This package is part of the `github.com/pearl-research-labs/pearl` module. Use it
by adding the module as a dependency in your project.

## License

Package btcutil is licensed under the [copyfree](http://copyfree.org) ISC
License.
