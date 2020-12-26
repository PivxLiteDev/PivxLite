(note: this is a temporary file, to be added-to by anybody, and moved to release-notes at release time)

PIVX Core version *version* is now available from:  <https://github.com/pivx-project/pivx/releases>

This is a new major version release, including various bug fixes and performance improvements, as well as updated translations.

Please report bugs using the issue tracker at github: <https://github.com/pivx-project/pivx/issues>


Mandatory Update
==============


How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely shut down (which might take a few minutes for older versions), then run the installer (on Windows) or just copy over /Applications/PIVX-Qt (on Mac) or pivxd/pivx-qt (on Linux).


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
New set of functionalities for usage of the SHIELD protocol:

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
* New derivation path for Sapling keys: Shield addresses are derived from the unique wallet's master seed (same as regular & cold staking addresses. The wallet's seed is able to restore PIVs and Shield PIVs equally).
* Support for Sapling extended full viewing keys, incoming viewing keys, outgoing viewing keys and spending keys.
* Sapling notes management: 
  - Notes decryption.
  - Blockchain scanning protocol: handling when and how to store a Sapling output and its related keys. As well as mark used notes.
  - Notes metadata cache: retrieving information without requiring to decrypt notes on-demand.
  - Able to filter available notes for spending.
* Sapling witnesses and nullifiers tracking.
* New transaction building process, crafting and signing shield transactions.

#### Block Primitive:
* Block version bumped to 8, the header is storing 32 bytes more for the Sapling incremental merkle tree root.
* Each block index now is tracking the network total value entering and exiting the shield pool.

#### Transaction Primitive:
* Version bumped to 2.
* The "transaction type" concept was introduced, dividing the version field (4 bytes) in version (first 2 bytes) and type (second 2 bytes).
* Sapling data has been included: `valueBalance`, `vShieldedSpend`, `ShieldedSpend` and `bindingSig`.
* Special transactions extra payload introduction (not enabled by consensus).
* A new signature hash for Sapling and Special transactions has been implemented.

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