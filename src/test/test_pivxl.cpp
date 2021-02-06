// Copyright (c) 2011-2013 The Bitcoin Core developers
// Copyright (c) 2017-2020 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#define BOOST_TEST_MODULE Pivx Test Suite

#include "test/test_pivxl.h"

#include "guiinterface.h"
#include "net_processing.h"
#include "random.h"
#include "rpc/server.h"
#include "rpc/register.h"
#include "script/sigcache.h"
#include "txmempool.h"
#include "txdb.h"
#include "validation.h"

#include <boost/test/unit_test.hpp>

std::unique_ptr<CConnman> g_connman;

CClientUIInterface uiInterface;  // Declared but not defined in guiinterface.h

uint256 insecure_rand_seed = GetRandHash();
FastRandomContext insecure_rand_ctx(insecure_rand_seed);

extern bool fPrintToConsole;
extern void noui_connect();

BasicTestingSetup::BasicTestingSetup()
{
        RandomInit();
        ECC_Start();
        SetupEnvironment();
        InitSignatureCache();
        fCheckBlockIndex = true;
        SelectParams(CBaseChainParams::MAIN);
}
BasicTestingSetup::~BasicTestingSetup()
{
        ECC_Stop();
        g_connman.reset();
}

TestingSetup::TestingSetup()
{
        ClearDatadirCache();
        pathTemp = GetTempPath() / strprintf("test_pivxl_%lu_%i", (unsigned long)GetTime(), (int)(InsecureRandRange(100000)));
        fs::create_directories(pathTemp);
        gArgs.ForceSetArg("-datadir", pathTemp.string());
        // Ideally we'd move all the RPC tests to the functional testing framework
        // instead of unit tests, but for now we need these here.
        RegisterAllCoreRPCCommands(tableRPC);
        pblocktree = new CBlockTreeDB(1 << 20, true);
        pcoinsdbview = new CCoinsViewDB(1 << 23, true);
        pcoinsTip = new CCoinsViewCache(pcoinsdbview);
        InitBlockIndex();
        {
            CValidationState state;
            bool ok = ActivateBestChain(state);
            BOOST_CHECK(ok);
        }
        nScriptCheckThreads = 3;
        for (int i=0; i < nScriptCheckThreads-1; i++)
            threadGroup.create_thread(&ThreadScriptCheck);
        g_connman = std::unique_ptr<CConnman>(new CConnman(0x1337, 0x1337)); // Deterministic randomness for tests.
        connman = g_connman.get();
        RegisterNodeSignals(GetNodeSignals());
}

TestingSetup::~TestingSetup()
{
        UnregisterNodeSignals(GetNodeSignals());
        threadGroup.interrupt_all();
        threadGroup.join_all();
        UnloadBlockIndex();
        delete pcoinsTip;
        delete pcoinsdbview;
        delete pblocktree;
        fs::remove_all(pathTemp);
}

CTxMemPoolEntry TestMemPoolEntryHelper::FromTx(CMutableTransaction &tx, CTxMemPool *pool) {
    const CTransactionRef txn = MakeTransactionRef(tx); // todo: move to the caller side..
    bool hasNoDependencies = pool ? pool->HasNoInputsOf(tx) : hadNoDependencies;
    // Hack to assume either its completely dependent on other mempool txs or not at all
    CAmount inChainValue = hasNoDependencies ? txn->GetValueOut() : 0;

    return CTxMemPoolEntry(txn, nFee, nTime, dPriority, nHeight,
                           hasNoDependencies, inChainValue, spendsCoinbaseOrCoinstake, sigOpCount);
}

[[noreturn]] void Shutdown(void* parg)
{
    std::exit(0);
}

[[noreturn]] void StartShutdown()
{
    std::exit(0);
}

bool ShutdownRequested()
{
  return false;
}
