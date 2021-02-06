// Copyright (c) 2018-2020 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "zpivchain.h"

#include "guiinterface.h"
#include "invalid.h"
#include "txdb.h"
#include "wallet/wallet.h"
#include "zpiv/zpivmodule.h"

// 6 comes from OPCODE (1) + vch.size() (1) + BIGNUM size (4)
#define SCRIPT_OFFSET 6
// For Script size (BIGNUM/Uint256 size)
#define BIGNUM_SIZE   4

bool BlockToMintValueVector(const CBlock& block, const libzerocoin::CoinDenomination denom, std::vector<CBigNum>& vValues)
{
    for (const auto& txIn : block.vtx) {
        const CTransaction& tx = *txIn;
        if(!tx.HasZerocoinMintOutputs())
            continue;

        for (const CTxOut& txOut : tx.vout) {
            if(!txOut.IsZerocoinMint())
                continue;

            CValidationState state;
            libzerocoin::PublicCoin coin(Params().GetConsensus().Zerocoin_Params(false));
            if(!TxOutToPublicCoin(txOut, coin, state))
                return false;

            if (coin.getDenomination() != denom)
                continue;

            vValues.push_back(coin.getValue());
        }
    }

    return true;
}

bool BlockToPubcoinList(const CBlock& block, std::list<libzerocoin::PublicCoin>& listPubcoins, bool fFilterInvalid)
{
    for (const auto& txIn : block.vtx) {
        const CTransaction& tx = *txIn;
        if(!tx.HasZerocoinMintOutputs())
            continue;

        // Filter out mints that have used invalid outpoints
        if (fFilterInvalid) {
            bool fValid = true;
            for (const CTxIn& in : tx.vin) {
                if (!ValidOutPoint(in.prevout, INT_MAX)) {
                    fValid = false;
                    break;
                }
            }
            if (!fValid)
                continue;
        }

        uint256 txHash = tx.GetHash();
        for (unsigned int i = 0; i < tx.vout.size(); i++) {
            //Filter out mints that use invalid outpoints - edge case: invalid spend with minted change
            if (fFilterInvalid && !ValidOutPoint(COutPoint(txHash, i), INT_MAX))
                break;

            const CTxOut txOut = tx.vout[i];
            if(!txOut.IsZerocoinMint())
                continue;

            CValidationState state;
            libzerocoin::PublicCoin pubCoin(Params().GetConsensus().Zerocoin_Params(false));
            if(!TxOutToPublicCoin(txOut, pubCoin, state))
                return false;

            listPubcoins.emplace_back(pubCoin);
        }
    }

    return true;
}

//return a list of zerocoin mints contained in a specific block
bool BlockToZerocoinMintList(const CBlock& block, std::list<CZerocoinMint>& vMints, bool fFilterInvalid)
{
    for (const auto& txIn : block.vtx) {
        const CTransaction& tx = *txIn;
        if(!tx.HasZerocoinMintOutputs())
            continue;

        // Filter out mints that have used invalid outpoints
        if (fFilterInvalid) {
            bool fValid = true;
            for (const CTxIn& in : tx.vin) {
                if (!ValidOutPoint(in.prevout, INT_MAX)) {
                    fValid = false;
                    break;
                }
            }
            if (!fValid)
                continue;
        }

        uint256 txHash = tx.GetHash();
        for (unsigned int i = 0; i < tx.vout.size(); i++) {
            //Filter out mints that use invalid outpoints - edge case: invalid spend with minted change
            if (fFilterInvalid && !ValidOutPoint(COutPoint(txHash, i), INT_MAX))
                break;

            const CTxOut txOut = tx.vout[i];
            if(!txOut.IsZerocoinMint())
                continue;

            CValidationState state;
            libzerocoin::PublicCoin pubCoin(Params().GetConsensus().Zerocoin_Params(false));
            if(!TxOutToPublicCoin(txOut, pubCoin, state))
                return false;

            //version should not actually matter here since it is just a reference to the pubcoin, not to the privcoin
            uint8_t version = 1;
            CZerocoinMint mint = CZerocoinMint(pubCoin.getDenomination(), pubCoin.getValue(), 0, 0, false, version, nullptr);
            mint.SetTxHash(tx.GetHash());
            vMints.push_back(mint);
        }
    }

    return true;
}

void FindMints(std::vector<CMintMeta> vMintsToFind, std::vector<CMintMeta>& vMintsToUpdate, std::vector<CMintMeta>& vMissingMints)
{
    // see which mints are in our public zerocoin database. The mint should be here if it exists, unless
    // something went wrong
    for (CMintMeta meta : vMintsToFind) {
        uint256 txHash;
        if (!zerocoinDB->ReadCoinMint(meta.hashPubcoin, txHash)) {
            vMissingMints.push_back(meta);
            continue;
        }

        // make sure the txhash and block height meta data are correct for this mint
        CTransaction tx;
        uint256 hashBlock;
        if (!GetTransaction(txHash, tx, hashBlock, true)) {
            LogPrintf("%s : cannot find tx %s\n", __func__, txHash.GetHex());
            vMissingMints.push_back(meta);
            continue;
        }

        if (!mapBlockIndex.count(hashBlock)) {
            LogPrintf("%s : cannot find block %s\n", __func__, hashBlock.GetHex());
            vMissingMints.push_back(meta);
            continue;
        }

        //see if this mint is spent
        uint256 hashTxSpend;
        bool fSpent = zerocoinDB->ReadCoinSpend(meta.hashSerial, hashTxSpend);

        //if marked as spent, check that it actually made it into the chain
        CTransaction txSpend;
        uint256 hashBlockSpend;
        if (fSpent && !GetTransaction(hashTxSpend, txSpend, hashBlockSpend, true)) {
            LogPrintf("%s : cannot find spend tx %s\n", __func__, hashTxSpend.GetHex());
            meta.isUsed = false;
            vMintsToUpdate.push_back(meta);
            continue;
        }

        //The mint has been incorrectly labelled as spent in zerocoinDB and needs to be undone
        int nHeightTx = 0;
        uint256 hashSerial = meta.hashSerial;
        uint256 txidSpend;
        if (fSpent && !IsSerialInBlockchain(hashSerial, nHeightTx, txidSpend)) {
            LogPrintf("%s : cannot find block %s. Erasing coinspend from zerocoinDB.\n", __func__, hashBlockSpend.GetHex());
            meta.isUsed = false;
            vMintsToUpdate.push_back(meta);
            continue;
        }

        // is the denomination correct?
        for (auto& out : tx.vout) {
            if (!out.IsZerocoinMint())
                continue;
            libzerocoin::PublicCoin pubcoin(Params().GetConsensus().Zerocoin_Params(meta.nVersion < libzerocoin::PrivateCoin::PUBKEY_VERSION));
            CValidationState state;
            TxOutToPublicCoin(out, pubcoin, state);
            if (GetPubCoinHash(pubcoin.getValue()) == meta.hashPubcoin && pubcoin.getDenomination() != meta.denom) {
                LogPrintf("%s: found mismatched denom pubcoinhash = %s\n", __func__, meta.hashPubcoin.GetHex());
                meta.denom = pubcoin.getDenomination();
                vMintsToUpdate.emplace_back(meta);
            }
        }

        // if meta data is correct, then no need to update
        if (meta.txid == txHash && meta.nHeight == mapBlockIndex[hashBlock]->nHeight && meta.isUsed == fSpent)
            continue;

        //mark this mint for update
        meta.txid = txHash;
        meta.nHeight = mapBlockIndex[hashBlock]->nHeight;
        meta.isUsed = fSpent;
        LogPrintf("%s: found updates for pubcoinhash = %s\n", __func__, meta.hashPubcoin.GetHex());

        vMintsToUpdate.push_back(meta);
    }
}

bool GetZerocoinMint(const CBigNum& bnPubcoin, uint256& txHash)
{
    txHash = UINT256_ZERO;
    return zerocoinDB->ReadCoinMint(bnPubcoin, txHash);
}

bool IsPubcoinInBlockchain(const uint256& hashPubcoin, uint256& txid)
{
    txid.SetNull();
    return zerocoinDB->ReadCoinMint(hashPubcoin, txid);
}

bool IsSerialInBlockchain(const CBigNum& bnSerial, int& nHeightTx)
{
    uint256 txHash;
    // if not in zerocoinDB then its not in the blockchain
    if (!zerocoinDB->ReadCoinSpend(bnSerial, txHash))
        return false;

    return IsTransactionInChain(txHash, nHeightTx);
}

bool IsSerialInBlockchain(const uint256& hashSerial, int& nHeightTx, uint256& txidSpend)
{
    CTransaction tx;
    return IsSerialInBlockchain(hashSerial, nHeightTx, txidSpend, tx);
}

bool IsSerialInBlockchain(const uint256& hashSerial, int& nHeightTx, uint256& txidSpend, CTransaction& tx)
{
    txidSpend.SetNull();
    // if not in zerocoinDB then its not in the blockchain
    if (!zerocoinDB->ReadCoinSpend(hashSerial, txidSpend))
        return false;

    return IsTransactionInChain(txidSpend, nHeightTx, tx);
}

bool RemoveSerialFromDB(const CBigNum& bnSerial)
{
    return zerocoinDB->EraseCoinSpend(bnSerial);
}

libzerocoin::CoinSpend TxInToZerocoinSpend(const CTxIn& txin)
{
    CDataStream serializedCoinSpend = ZPIVModule::ScriptSigToSerializedSpend(txin.scriptSig);
    return libzerocoin::CoinSpend(serializedCoinSpend);
}

bool TxOutToPublicCoin(const CTxOut& txout, libzerocoin::PublicCoin& pubCoin, CValidationState& state)
{
    CBigNum publicZerocoin;
    std::vector<unsigned char> vchZeroMint;
    vchZeroMint.insert(vchZeroMint.end(), txout.scriptPubKey.begin() + SCRIPT_OFFSET,
                       txout.scriptPubKey.begin() + txout.scriptPubKey.size());
    publicZerocoin.setvch(vchZeroMint);

    libzerocoin::CoinDenomination denomination = libzerocoin::AmountToZerocoinDenomination(txout.nValue);
    LogPrint(BCLog::LEGACYZC, "%s : denomination %d for pubcoin %s\n", __func__, denomination, publicZerocoin.GetHex());
    if (denomination == libzerocoin::ZQ_ERROR)
        return state.DoS(100, error("TxOutToPublicCoin : txout.nValue is not correct"));

    libzerocoin::PublicCoin checkPubCoin(Params().GetConsensus().Zerocoin_Params(false), publicZerocoin, denomination);
    pubCoin = checkPubCoin;

    return true;
}

//return a list of zerocoin spends contained in a specific block, list may have many denominations
std::list<libzerocoin::CoinDenomination> ZerocoinSpendListFromBlock(const CBlock& block, bool fFilterInvalid)
{
    std::list<libzerocoin::CoinDenomination> vSpends;
    for (const auto& txIn : block.vtx) {
        const CTransaction& tx = *txIn;
        if (!tx.HasZerocoinSpendInputs())
            continue;

        for (const CTxIn& txin : tx.vin) {
            bool isPublicSpend = txin.IsZerocoinPublicSpend();
            if (!txin.IsZerocoinSpend() && !isPublicSpend)
                continue;

            if (fFilterInvalid && !isPublicSpend) {
                libzerocoin::CoinSpend spend = TxInToZerocoinSpend(txin);
                if (invalid_out::ContainsSerial(spend.getCoinSerialNumber()))
                    continue;
            }

            libzerocoin::CoinDenomination c = libzerocoin::IntToZerocoinDenomination(txin.nSequence);
            vSpends.push_back(c);
        }
    }
    return vSpends;
}

