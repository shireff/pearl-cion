# Developer Resources

* [Code Contribution Guidelines](https://github.com/pearl-research-labs/pearl/node/tree/master/docs/code_contribution_guidelines.md)

* [JSON-RPC Reference](https://github.com/pearl-research-labs/pearl/node/tree/master/docs/json_rpc_api.md)
  * [RPC Examples](https://github.com/pearl-research-labs/pearl/node/tree/master/docs/json_rpc_api.md#ExampleCode)

* The Pearl Research Labs Pearl-related Go Packages:
  * [rpcclient](https://github.com/pearl-research-labs/pearl/node/tree/master/rpcclient) - Implements a
    robust and easy to use Websocket-enabled Pearl JSON-RPC client
  * [btcjson](https://github.com/pearl-research-labs/pearl/node/tree/master/btcjson) - Provides an extensive API
    for the underlying JSON-RPC command and return values
  * [wire](https://github.com/pearl-research-labs/pearl/node/tree/master/wire) - Implements the
    Pearl wire protocol
  * [peer](https://github.com/pearl-research-labs/pearl/node/tree/master/peer) -
    Provides a common base for creating and managing Pearl network peers.
  * [blockchain](https://github.com/pearl-research-labs/pearl/node/tree/master/blockchain) -
    Implements Pearl block handling and chain selection rules
  * [blockchain/fullblocktests](https://github.com/pearl-research-labs/pearl/node/tree/master/blockchain/fullblocktests) -
    Provides a set of block tests for testing the consensus validation rules
  * [txscript](https://github.com/pearl-research-labs/pearl/node/tree/master/txscript) -
    Implements the Pearl transaction scripting language
  * [btcec](https://github.com/pearl-research-labs/pearl/node/tree/master/btcec) - Implements
    support for the elliptic curve cryptographic functions needed for the
    Pearl scripts
  * [database](https://github.com/pearl-research-labs/pearl/node/tree/master/database) -
    Provides a database interface for the Pearl block chain
  * [mempool](https://github.com/pearl-research-labs/pearl/node/tree/master/mempool) -
    Package mempool provides a policy-enforced pool of unmined Pearl
    transactions.
  * [btcutil](https://github.com/pearl-research-labs/pearl/node/tree/master/btcutil) - Provides Pearl-specific
    convenience functions and types
  * [chainhash](https://github.com/pearl-research-labs/pearl/node/tree/master/chaincfg/chainhash) -
    Provides a generic hash type and associated functions that allows the
    specific hash algorithm to be abstracted.
  * [connmgr](https://github.com/pearl-research-labs/pearl/node/tree/master/connmgr) -
    Package connmgr implements a generic Pearl network connection manager.
