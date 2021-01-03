(note: this is a temporary file, to be added-to by anybody, and moved to release-notes at release time)

PIVX Core version *version* is now available from:  <https://github.com/pivx-project/pivx/releases>

This is a new major version release, including various bug fixes and performance improvements, as well as updated translations.

Please report bugs using the issue tracker at github: <https://github.com/pivx-project/pivx/issues>


Mandatory Update
==============


How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely shut down (which might take a few minutes for older versions), then run the installer (on Windows) or just copy over /Applications/PIVX-Qt (on Mac) or pivxd/pivx-qt (on Linux).

#### Sapling Parameters
In order to run, PIVX Core now requires two files, `sapling-output.params` and `sapling-spend.params` (with total size ~50 MB), to be saved in a specific location.

For the following packages, no action is required by the user:
- macOS release `dmg` binaries will use the params that are bundled into the .app bundle.
- Windows installer `.exe` will automatically copy the files in the proper location.
- Linux `PPA/Snap/Copr` installs will automatically copy the files in the proper location.

For the other packages, the user must save the param files in the proper location, before being able to run PIVX v5.0.0:
- macOS/Linux `tar.gz` tarballs include a bash script (`install-params.sh`) to copy the parameters in the appropriate location.
- Windows `.zip` users need to manually copy the files from the `share/pivx` folder to the `%APPDATA%\PIVXParams` directory.
- self compilers can run the script from the repository sources (`params/install-params.sh`), or copy the files directly from the `params` subdirectory.



Compatibility
==============

PIVX Core is extensively tested on multiple operating systems using the Linux kernel, macOS 10.10+, and Windows 7 and later.

Microsoft ended support for Windows XP on [April 8th, 2014](https://www.microsoft.com/en-us/WindowsForBusiness/end-of-xp-support), No attempt is made to prevent installing or running the software on Windows XP, you can still do so at your own risk but be aware that there are known instabilities and issues. Please do not report issues about Windows XP to the issue tracker.

Apple released it's last Mountain Lion update August 13, 2015, and officially ended support on [December 14, 2015](http://news.fnal.gov/2015/10/mac-os-x-mountain-lion-10-8-end-of-life-december-14/). PIVX Core software starting with v3.2.0 will no longer run on MacOS versions prior to Yosemite (10.10). Please do not report issues about MacOS versions prior to Yosemite to the issue tracker.

PIVX Core should also work on most other Unix-like systems but is not frequently tested on them.


Notable Changes
==============

(Developers: add your notes here as part of your pull requests whenever possible)

New SHIELD Protocol Implemented!
--------------------------------

#### Overview
Users will be able to protect their financial information by sending and receiving PIVs privately, as well as sending and receiving encrypted messages attached to the shield transactions.
More visual information about the protocol can be found at https://pivx.org .

#### GUI features
New set of functionalities for the interaction with the SHIELD protocol:

* Receive screen modified to show, generate and set label of wallet's shield addresses.
* Dashboard transactions list including shield transactions.
* Top bar showing shield PIV balances.
* Send screen modified, implementing shield transaction crafting and broadcast.
* New encrypted memo functionality.
* Transaction detail dialog presenting shield transaction information.
* Spending process moved to a background thread, loading screen connected while the operation is being performed.
* Contacts screen modified to store external shield addresses.

#### Wallet
A brand new manager encapsulating all Sapling related capabilities inside the wallet has been implemented:

* New address type: shield addresses (using bech32 format).
* New derivation path for Sapling keys: Shield addresses are derived from the same wallet master seed used for the deterministic derivation of all other keys. The same seed, therefore, can be used to restore both transparent and Shield addresses, recovering both transparent and Shield PIV balance.
* Support for Sapling extended full viewing keys, incoming viewing keys, outgoing viewing keys and spending keys.
* Sapling notes management:
  - Notes decryption.
  - Blockchain scanning protocol: handling when and how to store a Sapling output and its related keys. As well as mark used notes.
  - Notes metadata cache: retrieving information without requiring to decrypt notes on-demand.
  - Able to filter available notes for spending.
* Sapling witnesses and nullifiers tracking.
* New transaction building process, crafting and signing shield transactions.

#### Block Primitive:
* Block version bumped to 8.
* Starting from block v8, the block header includes an additional `uint256` field, `hashFinalSaplingRoot` (which adds 32 bytes to the end of the serialized block header) storing the root of the Sapling note commitment tree, corresponding to the final Sapling treestate of this block.
* Each block index now is tracking the network total value entering and exiting the shield pool.

#### Transaction Primitive:
* The "transaction type" concept was introduced, dividing the version field (4 bytes) in version (first 2 bytes) and type (second 2 bytes).
* For transactions with `nVersion >= 3`, a new optional field `sapData` has been included, comprising:
  - `valueBalance`: the net value of Sapling spend transfers minus output transfers
  - `vShieldedSpend`: a sequence of Sapling spend descriptions
  - `vShieldedOutput`: a sequence of Sapling output descriptions
  - `bindingSig`: the Sapling binding signature
* Transactions with `nVersion >= 3` and `nType != 0`(not enabled by consensus at the moment), are defined "special transactions" and must include an extra payload serialized according to the definition imposed by the relative type.
* A new signature hash for Sapling and Special transactions has been implemented (as defined in [ZIP-243](https://zips.z.cash/zip-0243)).

#### Build System
In order to support the protocol, the following dependencies are introduced in the build system:
- `Sodium` https://github.com/jedisct1/libsodium
- `Rust/Cargo`https://www.rust-lang.org/

Tier Two Network
----------------
A large number of performance and stability improvements over the complete tier two network and masternodes sources have been performed. Re-writing, encapsulating and decoupling workflows, cleaning up an extensive number of redundancies and misalignment.
Plus, tier two and masternodes capabilities have been introduced to regtest, enabling the local testing environment setup and the creation of a functional testing framework for the area.
This work is directly correlated with a substantial efficiency improvement for the entire software, not only for masternodes, and is one of the building blocks for the new tier two network and sync protocol that will be continued post-v5.

Instant Proposal Removal:
The threshold was increased to 30% of negative votes.
The proposal will never be removed before its first superblock.

Maximum Proposal Payment:
The limit was decreased to a maximum of 6 payments.

Concurrency
-----------
A complete overhaul of the thread synchronization area has been done, all of the workflows that were containing locks with cyclic dependencies that could cause a deadlock were reworked.
Solving every circular dependency, a good number of inconsistent states were fixed. The software now can be fully run with the `--enable-debug` configure time flag without crashing for a possible deadlock.

Account API Removed
-------------------

The 'account' API was deprecated in v4.2.0, and has been fully removed in v5.0.0.
The 'label' API was introduced in v4.2.0 as a replacement for accounts.

See the release notes from [v4.2.0](https://github.com/PIVX-Project/PIVX/blob/master/doc/release-notes/release-notes-4.2.0.md#label-and-account-apis-for-wallet) for a full description of the changes from the 'account' API to the 'label' API.


GUI Changes
-----------
#### Shield Protocol
Several views and widgets have been updated to support new features as described in the previous section.

#### RPC Console
- Added autocompletion for the `help` command
- Added warning/confirmation dialog before executing commands that expose private keys (`dumpwallet`, `dumpprivkey`, `exportsaplingkey`)


RPC Changes
------------------
The RPC server now returns a JSON-RPC exception when receiving a request during the warm-up phase. The returned exception includes the current init message ("Loading sporks", "Loading block index", "Verifying blocks", etc...)

#### New RPC Commands

Several new RPC commands have been introduced to support SHIELD address and transaction interactions, as well as expanded informational and layer 2 functionality. The below table lists each command and it's intended purpose.

| Command Name | Purpose | Requires Unlocked Wallet? |
| ------------ | ------- | ------------------------- |
| `getbestsaplinganchor` | Returns the most recent SaplingMerkleTree root | No |
| `exportsaplingkey` | Exports the private key of a SHIELD address | Yes |
| `exportsaplingviewingkey`| Exports the viewing key of a SHIELD address | Yes |
| `getnewshieldaddress` | Creates a new SHIELD address | Yes |
| `getsaplingnotescount` | Returns the number of sapling notes available in the wallet | No |
| `getshieldbalance` | Return information about the shield value of funds stored in the wallet | No |
| `importsaplingkey` | Imports the private key of a SHIELD address | Yes |
| `importsaplingviewingkey` | Imports the viewing key of a SHIELD address | Yes |
| `listreceivedbyshieldaddress` | Returns a list of amounts received by a SHIELD address in the wallet | No |
| `listshieldaddresses` | Returns the list of shield addresses belonging to the wallet | No |
| `listshieldunspent` | Returns array of unspent SHIELD notes in the wallet | No |
| `rawshieldsendmany` | Creates a transaction sending to many recipients (without committing it), and returns the hex string. | Yes |
| `shieldsendmany` | Send to many recipients (Either transparent or SHIELD) | Yes |
| `viewshieldtransaction` | Get detailed SHIELD information about an in-wallet transaction | Yes |
| `getsupplyinfo` | Returns detailed PIV supply information | No |
| `initmasternode` | Manually initialize the client as a masternode | No |
| `getcachedblockhashes` | Return the block hashes cached in the masternode manager | No |

Each new command is detailed below:

* `getbestsaplinganchor`
  ```
  Returns the most recent SaplingMerkleTree root.

  Result
  "hex"      (string) the sapling anchor hex encoded
  ```
* `exportsaplingkey`
  ```
  Reveals the key corresponding to the 'shield_addr'.
  Then the importsaplingkey can be used with this output

  Arguments:
  1. "addr"   (string, required) The shield addr for the private key

  Result:
  "key"       (string) The private key
  ```
* `exportsaplingviewingkey`
  ```
  Reveals the viewing key corresponding to 'shield_addr'.
  Then the importsaplingviewingkey can be used with this output

  Arguments:
  1. "shield_addr"   (string, required) The shield addr for the viewing key

  Result:
  "vkey"               (string) The viewing key
  ```
* `getnewshieldaddress`
  ```
  Returns a new shield address for receiving payments.

  Result:
  "address"    (string) The new shield address.
  ```
* `getsaplingnotescount`
  ```
  Returns the number of sapling notes available in the wallet.

  Arguments:
  1. minconf      (numeric, optional, default=1) Only include notes in transactions confirmed at least this many times.

  Result:
  num             (numeric) the number of sapling notes in the wallet
  ```
* `getshieldbalance`
  ```
  Return the total shield value of funds stored in the node's wallet or if an address was given,
  returns the balance of the shield addr belonging to the node's wallet.

  CAUTION: If the wallet contains any addresses for which it only has incoming viewing keys,
  the returned private balance may be larger than the actual balance, because spends cannot
  be detected with incoming viewing keys.

  Arguments:
  1. "address"        (string, optional) The selected address. If non empty nor "*", it must be a Sapling address
  2. minconf          (numeric, optional, default=1) Only include private and transparent transactions confirmed at least this many times.
  3. includeWatchonly (bool, optional, default=false) Also include balance in watchonly addresses (see 'importaddress' and 'importsaplingviewingkey')

  Result:
  amount              (numeric) the total balance of shield funds (in Sapling addresses)
  ```
* `importsaplingkey`
  ```
  Adds a key (as returned by exportsaplingkey) to your wallet.

  Arguments:
  1. "key"           (string, required) The zkey (see exportsaplingkey)
  2. rescan          (string, optional, default="whenkeyisnew") Rescan the wallet for transactions - can be "yes", "no" or "whenkeyisnew"
  3. startHeight     (numeric, optional, default=0) Block height to start rescan from

  Note: This call can take minutes to complete if rescan is true.

  Result:
  {
    "address" : "address|DefaultAddress",    (string) The address corresponding to the spending key (the default address).
  }
  ```
* `importsaplingviewingkey`
  ```
  Adds a viewing key (as returned by exportsaplingviewingkey) to your wallet.

  Arguments:
  1. "vkey"           (string, required) The viewing key (see exportsaplingviewingkey)
  2. rescan           (string, optional, default="whenkeyisnew") Rescan the wallet for transactions - can be "yes", "no" or "whenkeyisnew"
  3. startHeight      (numeric, optional, default=0) Block height to start rescan from

  Note: This call can take minutes to complete if rescan is true.

  Result:
  {
    "address" : "address|DefaultAddress",    (string) The address corresponding to the viewing key (for Sapling, this is the default address).
  }
  ```
* `listreceivedbyshieldaddress`
  ```
  Return a list of amounts received by a shield addr belonging to the node's wallet.

  Arguments:
  1. "address"      (string) The private address.
  2. minconf        (numeric, optional, default=1) Only include transactions confirmed at least this many times.

  Result:
  {
    "txid": "txid",            (string) the transaction id
    "amount": xxxxx,           (numeric) the amount of value in the note
    "memo": xxxxx,             (string) hexadecimal string representation of memo field
    "confirmations" : n,       (numeric) the number of confirmations
    "blockheight": n,          (numeric) The block height containing the transaction
    "blockindex": n,           (numeric) The block index containing the transaction.
    "blocktime": xxx,          (numeric) The transaction time in seconds since epoch (midnight Jan 1 1970 GMT).
    "outindex" (sapling) : n,  (numeric) the output index
    "change": true|false,      (boolean) true if the address that received the note is also one of the sending addresses
  }
  ```
* `listshieldaddresses`
  ```
  Returns the list of shield addresses belonging to the wallet.

  Arguments:
  1. includeWatchonly (bool, optional, default=false) Also include watchonly addresses (see 'importviewingkey')

  Result:
  [                 (json array of string)
    "addr",         (string) a shield address belonging to the wallet
    ...
  ]
  ```
* `listshieldunspent`
  ```
  Returns array of unspent shield notes with between minconf and maxconf (inclusive) confirmations.
  Optionally filter to only include notes sent to specified addresses.
  When minconf is 0, unspent notes with zero confirmations are returned, even though they are not immediately spendable.

  Arguments:
  1. minconf          (numeric, optional, default=1) The minimum confirmations to filter
  2. maxconf          (numeric, optional, default=9999999) The maximum confirmations to filter
  3. includeWatchonly (bool, optional, default=false) Also include watchonly addresses (see 'importsaplingviewingkey')
  4. "addresses"      (string) A json array of shield addrs to filter on.  Duplicate addresses not allowed.
     [
       "address",     (string) shield addr
       ...
     ]

  Result:
  [                             (array of json object)
    {
      "txid" : "txid",          (string) the transaction id
      "outindex" (sapling) : n,       (numeric) the output index
      "confirmations" : n,       (numeric) the number of confirmations
      "spendable" : true|false,  (boolean) true if note can be spent by wallet, false if address is watchonly
      "address" : "address",    (string) the shield address
      "amount": xxxxx,          (numeric) the amount of value in the note
      "memo": xxxxx,            (string) hexademical string representation of memo field
      "change": true|false,     (boolean) true if the address that received the note is also one of the sending addresses
      "nullifier": xxxxx,       (string) the note's nullifier, hex encoded  }
    },
    ...
  ]
  ```
* `rawshieldsendmany`
  ```
  Creates a transaction sending to many recipients (without committing it), and returns the hex string.
  Amounts are decimal numbers with at most 8 digits of precision.
  Change generated from a transparent addr flows to a new  transparent addr address, while change generated from a shield addr returns to itself.
  When sending coinbase UTXOs to a shield addr, change is not allowed. The entire value of the UTXO(s) must be consumed.
  Requires wallet passphrase to be set with walletpassphrase call.

  Arguments:
  1. "fromaddress"        (string, required) The transparent addr or shield addr to send the funds from.
     It can also be the string "from_transparent"|"from_shield" to send the funds
     from any transparent|shield address available.
     Additionally, it can be the string "from_trans_cold" to select transparent funds,
     possibly including delegated coins, if needed.
  2. "amounts"            (array, required) An array of json objects representing the amounts to send.
     [{
       "address":address  (string, required) The address is a transparent addr or shield addr
       "amount":amount    (numeric, required) The numeric amount in PIV is the value
       "memo":memo        (string, optional) If the address is a shield addr, message string of max 512 bytes
     }, ... ]
  3. minconf              (numeric, optional, default=1) Only use funds confirmed at least this many times.
  4. fee                  (numeric, optional), The fee amount to attach to this transaction.
     If not specified, the wallet will try to compute the minimum possible fee for a shield TX,
     based on the expected transaction size and the current value of -minRelayTxFee.

  Result:
  transaction                (string) hex string of the transaction
  ```
* `shieldsendmany`
  ```
  Send to many recipients. Amounts are decimal numbers with at most 8 digits of precision.
  Change generated from a transparent addr flows to a new  transparent addr address, while change generated from a shield addr returns to itself.
  When sending coinbase UTXOs to a shield addr, change is not allowed. The entire value of the UTXO(s) must be consumed.
  Requires wallet passphrase to be set with walletpassphrase call.

  Arguments:
  1. "fromaddress"         (string, required) The transparent addr or shield addr to send the funds from.
     It can also be the string "from_transparent"|"from_shield" to send the funds
     from any transparent|shield address available.
     Additionally, it can be the string "from_trans_cold" to select transparent funds,
     possibly including delegated coins, if needed.
  2. "amounts"             (array, required) An array of json objects representing the amounts to send.
     [{
       "address":address  (string, required) The address is a transparent addr or shield addr
       "amount":amount    (numeric, required) The numeric amount in PIV is the value
       "memo":memo        (string, optional) If the address is a shield addr, message string of max 512 bytes
     }, ... ]
  3. minconf               (numeric, optional, default=1) Only use funds confirmed at least this many times.
  4. fee                   (numeric, optional), The fee amount to attach to this transaction.
     If not specified, the wallet will try to compute the minimum possible fee for a shield TX,
     based on the expected transaction size and the current value of -minRelayTxFee.

  Result:
  "id"          (string) transaction hash in the network
  ```
* `viewshieldtransaction`
  ```
  Get detailed shield information about in-wallet transaction "txid"

  Requires wallet passphrase to be set with walletpassphrase call.

  Arguments:
  1. "txid"    (string, required) The transaction id

  Result:
  {
    "txid" : "transactionid",   (string) The transaction id
    "fee"  : x.xxx,             (numeric) The transaction fee in PIV
    "spends" : [
      {
        "spend" : n,                    (numeric, sapling) the index of the spend within vShieldedSpend
        "txidPrev" : "transactionid",   (string) The id for the transaction this note was created in
        "outputPrev" : n,               (numeric, sapling) the index of the output within the vShieldedOutput
        "address" : "pivxaddress",      (string) The PIVX address involved in the transaction
        "value" : x.xxx                 (numeric) The amount in PIV
        "valueSat" : xxxx               (numeric) The amount in satoshis
      }
      ,...
    ],
    "outputs" : [
      {
        "output" : n,                   (numeric, sapling) the index of the output within the vShieldedOutput
        "address" : "pivxaddress",      (string) The PIVX address involved in the transaction
        "outgoing" : true|false         (boolean, sapling) True if the output is not for an address in the wallet
        "value" : x.xxx                 (numeric) The amount in PIV
        "valueSat" : xxxx               (numeric) The amount in satoshis
        "memo" : "hexmemo",             (string) Hexademical string representation of the memo field
        "memoStr" : "memo",             (string) Only returned if memo contains valid UTF-8 text.
      }
      ,...
    ],
  }
  ```
* `getsupplyinfo`
  ```
  If forceupdate=false (default if no argument is given): return the last cached money supply
  (sum of spendable transaction outputs) and the height of the chain when it was last updated
  (it is updated periodically, whenever the chainstate is flushed).

  If forceupdate=true: Flush the chainstate to disk and return the money supply updated to
  the current chain height.

  Arguments:
  1. forceupdate       (boolean, optional, default=false) flush chainstate to disk and update cache

  Result:
  {
    "updateheight" : n,       (numeric) The chain height when the transparent supply was updated
    "transparentsupply" : n   (numeric) The sum of all spendable transaction outputs at height updateheight
    "shieldsupply": n         (numeric) Chain tip shield pool value
    "totalsupply": n          (numeric) The sum of transparentsupply and shieldsupply
  }
  ```
* `initmasternode`
  ```
  Initialize masternode on demand if it's not already initialized.

  Arguments:
  1. masternodePrivKey          (string, required) The masternode private key.
  2. masternodeAddr             (string, required) The IP:Port of this masternode.

  Result:
  success                      (string) if the masternode initialization succeeded.
  ```
* `getcachedblockhashes`
  ```
  Return the block hashes cached in the masternode manager

  Result:
  [
    ...
    "xxxx",   (string) hash at Index d (height modulo max cache size)
    ...
  ]
  ```

#### Changed RPC Commands

Several RPC commands have had changes to their input arguments or output fields since v4.3.0 to support new functionality. Below is a detailed list of changes to existing RPC commands:

* `dumpwallet`
  A new entry (`warning`) is added to the return object, explaining the risks connected with sharing the output of the command. Also the filename can no longer contain the strings `"bug"` and `"log"`.

* `delegatestake`/`rawdelegatestake`
  A new input parameter (`fUseShielded`) has been added, in order to delegate funds from Shield addresses.

* `getblock`
  A new JSON entry (`finalsaplingroot`) has been added to the return object. (string) The root of the Sapling commitment tree after applying this block.

* `getblockheader`
  A new JSON object (`shield_pool_value`) has been added to the return object with the following information:
  ```
  "shield_pool_value":   (object) Block shield pool value
  {
     "chainValue":        (numeric) Total value held by the Sapling circuit up to and including this block
     "valueDelta":        (numeric) Change in value held by the Sapling circuit over this block
  }
  ```

* `getblockchaininfo`
  A new JSON entry (`initial_block_downloading`) has been added to the return object. (boolean) whether the node is in initial block downloading state or not.
  A new JSON object (`shield_pool_value`) has been added to the return object with the following information:
  ```
  "shield_pool_value":   (object) Chain tip shield pool value
  {
     "chainValue":        (numeric) Total value held by the Sapling circuit up to and including this block
     "valueDelta":        (numeric) Change in value held by the Sapling circuit over this block
  }
  ```

* `getblockindexstats`
  The `fFeeOnly` input argument has been removed.
  The `spendcount` and `pubspendcount` return objects have been removed due to zerocoin deprecation.

* `preparebudget` and `submitbudget`
  A maximum of `6` payment cycles per proposal (`20` on testnet) is now imposed for newly created proposals. Amounts higher than this limit will now return an error message.

* `getbudgetprojection`
  A typo in the JSON return fields has been corrected; `Alloted` and `TotalBudgetAlloted` have been renamed to `Allotted` and `TotalBudgetAllotted` respectively.

* `mnfinalbudget show`
  The `strName` key of the JSON object, representing a finalized budget in the output list, has now the format `"main" (Hash)`. The object no longer has the `"Hash"` entry.

* `sendtoaddress`/`sendmany`
  Shield addresses can be used as recipients. In this case the request is forwarded to `shieldsendmany` with first parameter set to `from_transparent`.

* `startmasternode`
  A new optional input argument (`reload_conf`) has been added to reload the client's `masternode.conf` file in real-time. This optional argument is only available when running `startmasternode` with "alias" being specified as the first argument.

* `decodemasternodebroadcast`
  The `nlastdsq` JSON return entry has been removed. This was a remnant from pre-zerocoin "CoinJoin" mixing.

* `getinfo`
  The `zerocoinbalance` JSON return entry and the `zPIVsupply` return JSON object have been removed due to zerocoin deprecation.
  The `balance` JSON return entry is now all inclusive (the sum of all transparent and shield PIV)
  The `moneysupply` JSON return entry is the total PIV supply (the sum of all transparent and shield PIV)
  A new JSON return entry (`transparentsupply`) has been added to return the sum of the value of all unspent outputs when the chainstate was last flushed to disk.
  A new JSON return entry (`shieldsupply`) has been added to return the shield supply at the chain tip.
  Integrators should switch to the newly added `getsupplyinfo` RPC command for the purpose of obtaining supply information as it is more robust and does not rely on information caching.

* `validateaddress`
  The `account` JSON return entry has been removed due to the removal of the internal accounting system. The `label` entry was it's replacement.

* `getrawtransaction`
  A new JSON return entry (`type`) has been added which indicates via a numeric value the type of transaction (currently not fully implemented).
  A new JSON return array (`shield_addresses`) has been added which will contain a list of shield address strings involved in the transaction (if that information is available to the local client).
  Two new JSON return entries (`extraPayloadSize` and `extraPayload`) have been added pre-emptively to support future implementation of "special" transactions (currently NYI).

* `getrawtransaction` and `decoderawtransaction`
  New JSON output entries have been added to both these commands to detail sapling input/output information after the `vout` array in the following format:
  ```
  "valueBalance": n,          (numeric) The net value of spend transfers minus output transfers
  "valueBalanceSat": n,       (numeric) `valueBalance` in sats
  "vShieldSpend": [               (array of json objects)
     {
       "cv": "hex",         (string) A value commitment to the value of the input note
       "anchor": hex,         (string) A Merkle root of the Sapling note commitment tree at some block height in the past
       "nullifier": hex,       (string) The nullifier of the input note
       "rk": hex,              (string) The randomized public key for spendAuthSig
       "proof": hex,           (string) A zero-knowledge proof using the spend circuit
       "spendAuthSig": hex,    (string) A signature authorizing this spend
     }
     ,...
  ],
  "vShieldOutput": [             (array of json objects)
     {
       "cv": hex,                  (string) A value commitment to the value of the output note
       "cmu": hex,                 (string) The u-coordinate of the note commitment for the output note
       "ephemeralKey": hex,         (string) A Jubjub public key
       "encCiphertext": hex,       (string) A ciphertext component for the encrypted output note
       "outCiphertext": hex,       (string) A ciphertext component for the encrypted output note
       "proof": hex,               (string) A zero-knowledge proof using the output circuit
     }
     ,...
  ],
  "bindingSig": hex,       (string) Prove consistency of valueBalance with the value commitments in spend descriptions and output descriptions, and proves knowledge of the randomness used for the spend and output value commitments
  ```

* `getbalance`
  The `account` input argument has been removed due to the removal of the internal accounting system.
  A new optional input argument (`includeShield`) has been added to include shield balance. Default is `true`.

* `getcoldstakingbalance` and `getdelegatedbalance`
  The optional `account` input argument has been removed due to the removal of the internal accounting system.

* `listreceivedbyaddress`, `listreceivedbylabel`, `listsinceblock`, and `gettransaction`
  The JSON return entry `account` has been removed due to the removal of the internal accounting system.
  The JSON return entry `bcconfirmations` has been marked as deprecated and will be removed in the future.

* `listunspent`
  The JSON return entry `account` has been removed due to the removal of the internal accounting system.
  A new JSON entry (`generated`) has been added to indicate if the txout is a coinstake transaction.


#### Removed RPC Commands

The table below details RPC commands that have now been fully removed. These commands had previously been deprecated or otherwise marked for removal with the v5.0 release:

| Command Name | Previous Purpose | Reason for removal |
| ------------ | ---------------- | ------------------ |
| `findserial` | Searches the zerocoin database for a zerocoin spend transaction that contains the specified serial | Zerocoin deprecation |
| `getserials` | Look at the inputs of any tx in a range of blocks and returns the serial numbers for any coinspend | Zerocoin deprecation |
| `getaccountaddress` | Returned the current PIVX address associated with an internal account | Account system removal |
| `setaccount` | Associate a PIVX address with an internal account name | Account system removal |
| `getaccount` | Returned the internal account associated with a given PIVX address | Account system removal |
| `getaddressesbyaccount` | Returned a list of PIVX addresses associated with an internal account | Account system removal |
| `sendtoaddressix` | Send an amount to a given address using SwiftX | SwiftX disabled |
| `getreceivedbyaccount` | Returned the total amount received by addresses associated with an internal account | Account system removal |
| `move` | Move funds from one internal account to another internal account | Account system removal |
| `sendfrom` | Send an amount from an internal account to a PIVX address | Account system removal |
| `listreceivedbyaccount` | List received transactions by account | Account system removal |
| `listaccounts` | List internal account names and balances | Account system removal |
| `multisend` | Configure sending a portion of stake rewards to a PIVX address | Pending a full rewrite |
| `getzerocoinbalance` | Returned the wallet's total zPIV balance | Zerocoin deprecation |
| `listmintedzerocoins` | List all zPIV mints in the wallet | Zerocoin deprecation |
| `listzerocoinamounts` | List information about your zerocoin amounts | Zerocoin deprecation |
| `listspentzerocoins` | List all the spent zPIV mints in the wallet | Zerocoin deprecation |
| `mintzerocoin` | Mint the specified zPIV amount | Zerocoin deprecation |
| `spendzerocoin` | Spend zPIV to a PIV address | Zerocoin deprecation |
| `spendzerocoinmints` | Spend specific zPIV mints to a PIV address | Zerocoin deprecation |
| `resetmintzerocoin` | Scan the chain for zerocoin mints held in the wallet DB and refresh their status | Zerocoin deprecation |
| `resetspentzerocoin` | Scan the chain for all zerocoin spends held in the wallet DB and refresh their status | Zerocoin deprecation |
| `getarchivedzerocoin` | Display zerocoins that were archived because they were believed to be orphans | Zerocoin deprecation |
| `exportzerocoins` | Export zerocoin mints that are held in the wallet DB | Zerocoin deprecation |
| `importzerocoins` | Import zerocoin mints | Zerocoin deprecation |
| `reconsiderzerocoins` | Check archived zPIVs to see if any mints were added to the chain | Zerocoin deprecation |
| `setzpivseed` | Set the wallet's deterministic zPIV seed to a specific value | Zerocoin deprecation |
| `getzpivseed` | Return the current deterministic zPIV seed | Zerocoin deprecation |
| `generatemintlist` | Show mints that are derived from the deterministic zPIV seed | Zerocoin deprecation |
| `dzpivstate` | Show the current state of the mintpool of the wallet | Zerocoin deprecation |
| `searchdzpiv` | Do a search for deterministically generated zPIV that have not yet added to the wallet | Zerocoin deprecation |
| `spendrawzerocoin` | Create and broadcast a TX spending the provided zericoin | Zerocoin deprecation |


*version* Change log
==============

Detailed release notes follow. This overview includes changes that affect behavior, not code moves, refactors and string updates. For convenience in locating the code changes and accompanying discussion, both the pull request and git merge commit are mentioned.

### Core Features

### Build System

### GUI

### Wallet

### P2P Protocol and Network Code

### Tier Two Network

### RPC/REST

### Miscellaneous

## Credits

Thanks to everyone who directly contributed to this release:


As well as everyone that helped translating on [Transifex](https://www.transifex.com/projects/p/pivx-project-translations/), the QA team during Testing and the Node hosts supporting our Testnet.
