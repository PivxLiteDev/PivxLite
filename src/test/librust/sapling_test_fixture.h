// Copyright (c) 2020 The PIVX developers
// Copyright (c) 2019-2021 The PIVXL developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php.

#ifndef PIVXL_SAPLING_TEST_FIXTURE_H
#define PIVXL_SAPLING_TEST_FIXTURE_H

#include "test/test_pivx.h"

/**
 * Testing setup that configures a complete environment for Sapling testing.
 */
struct SaplingTestingSetup : public TestingSetup
{
    SaplingTestingSetup(const std::string& chainName = CBaseChainParams::MAIN);
    ~SaplingTestingSetup();
};

/**
 * Regtest setup with sapling always active
 */
struct SaplingRegTestingSetup : public SaplingTestingSetup
{
    SaplingRegTestingSetup();
};


#endif //PIVXL_SAPLING_TEST_FIXTURE_H
