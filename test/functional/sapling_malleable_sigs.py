#!/usr/bin/env python3
# Copyright (c) 2018 The Zcash developers
# Copyright (c) 2020 The PIVX developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://www.opensource.org/licenses/mit-license.php .

from test_framework.test_framework import PivxTestFramework

from test_framework.messages import (
    CTransaction,
)
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    bytes_to_hex_str,
    hex_str_to_bytes,
)

from decimal import Decimal
from io import BytesIO

class MalleableSigsTest(PivxTestFramework):

    def set_test_params(self):
        self.num_nodes = 1
        saplingUpgrade = ['-nuparams=v5_shield:201']
        self.extra_args = [saplingUpgrade]

    def run_test(self):
        node = self.nodes[0]
        node.generate(2)
        assert_equal(node.getblockcount(), 202)

        t_balance = node.getbalance()
        z_balance = node.getshieldedbalance()

        z_addr = node.getnewshieldedaddress()
        shield_to = [{"address": z_addr, "amount": Decimal('10')}]

        # Create rawtx shielding 10 PIV
        self.log.info("Shielding 10 PIV...")
        rawtx = node.rawshieldedsendmany("from_transparent", shield_to)["hex"]
        self.log.info("Raw tx created")

        # Creating malleated tx
        self.log.info("Removing sapling data...")
        new_tx = CTransaction()
        new_tx.deserialize(BytesIO(hex_str_to_bytes(rawtx)))
        new_tx.sapData = b""
        new_rawtx = bytes_to_hex_str(new_tx.serialize())
        self.log.info("Sending malleated tx...")
        txid = node.sendrawtransaction(new_rawtx, True)
        assert(txid is not None)
        self.log.info("Tx accepted in mempool")

        # No notes minted
        assert_equal(node.getshieldedbalance(), z_balance)
        # But 10 PIV have been effectively burned
        fee = - Decimal(node.gettransaction(txid)['fee'])
        assert_greater_than(fee, Decimal('10'))
        assert_equal(node.getbalance(), t_balance - fee)

        hashes = node.generate(1)
        assert_equal(len(hashes), 1)
        chain_tx = node.gettransaction(txid)
        assert_equal(chain_tx['confirmations'], 1)
        self.log.info("Malleated Tx accepted on chain")





if __name__ == '__main__':
    MalleableSigsTest().main()
