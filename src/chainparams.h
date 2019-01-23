// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CHAINPARAMS_H
#define BITCOIN_CHAINPARAMS_H

#include <chainparamsbase.h>
#include <consensus/params.h>
#include <primitives/block.h>
#include <chain.h>
#include <protocol.h>

#include <memory>
#include <vector>
#include "libzerocoin/Params.h"

class CBlockIndex;

static const uint32_t CHAIN_NO_GENESIS = 444444;
static const uint32_t CHAIN_NO_STEALTH_SPEND = 444445; // used hardened

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

typedef std::map<int, uint256> MapCheckpoints;

struct CCheckpointData {
    MapCheckpoints mapCheckpoints;
};

/**
 * Holds various statistics on transactions within a chain. Used to estimate
 * verification progress during chain sync.
 *
 * See also: CChainParams::TxData, GuessVerificationProgress.
 */
struct ChainTxData {
    int64_t nTime;    //!< UNIX timestamp of last known number of transactions
    int64_t nTxCount; //!< total number of transactions between genesis and that timestamp
    double dTxRate;   //!< estimated number of transactions per second after that timestamp
};

class CImportedCoinbaseTxn
{
public:
    CImportedCoinbaseTxn(uint32_t nHeightIn, uint256 hashIn) : nHeight(nHeightIn), hash(hashIn) {};
    uint32_t nHeight;
    uint256 hash; // hash of output data
};

class DevFundSettings
{
public:
    DevFundSettings(std::string sAddrTo, int nMinDevStakePercent_, int nDevOutputPeriod_)
        : sDevFundAddresses(sAddrTo), nMinDevStakePercent(nMinDevStakePercent_), nDevOutputPeriod(nDevOutputPeriod_) {};

    std::string sDevFundAddresses;
    int nMinDevStakePercent; // [0, 100]
    int nDevOutputPeriod; // dev fund output is created every n blocks
    //CAmount nMinDevOutputSize; // if nDevOutputGap is -1, create a devfund output when value is > nMinDevOutputSize
};

/**
 * CChainParams defines various tweakable parameters of a given instance of the
 * Bitcoin system. There are three: the main network on which people trade goods
 * and services, the public test network which gets reset from time to time and
 * a regression test mode which is intended for private networks only. It has
 * minimal difficulty to ensure that blocks can be found instantly.
 */
class CChainParams
{
public:
    enum Base58Type {
        PUBKEY_ADDRESS,
        SCRIPT_ADDRESS,
        SECRET_KEY,
        EXT_PUBLIC_KEY,
        EXT_SECRET_KEY,
        STEALTH_ADDRESS,
        EXT_KEY_HASH,
        EXT_ACC_HASH,
        EXT_PUBLIC_KEY_BTC,
        EXT_SECRET_KEY_BTC,
        PUBKEY_ADDRESS_256,
        SCRIPT_ADDRESS_256,
        STAKE_ONLY_PKADDR,
        EXT_COIN_TYPE,  // BIP44
        MAX_BASE58_TYPES
    };

    const Consensus::Params& GetConsensus() const { return consensus; }
    const CMessageHeader::MessageStartChars& MessageStart() const { return pchMessageStart; }
    int GetDefaultPort() const { return nDefaultPort; }

    int BIP44ID() const { return nBIP44ID; }

    uint32_t GetModifierInterval() const { return nModifierInterval; }
    uint32_t GetStakeMinConfirmations() const { return nStakeMinConfirmations; }
    uint32_t GetTargetSpacing() const { return nTargetSpacing; }
    uint32_t GetTargetTimespan() const { return nTargetTimespan; }

    uint32_t GetStakeTimestampMask(int nHeight) const { return nStakeTimestampMask; }
    int64_t GetCoinYearReward(int64_t nTime) const;

    const DevFundSettings *GetDevFundSettings(int64_t nTime) const;
    const std::vector<std::pair<int64_t, DevFundSettings> > &GetDevFundSettings() const {return vDevFundSettings;};

    int64_t GetProofOfStakeReward(const CBlockIndex *pindexPrev, int64_t nFees) const;

    bool CheckImportCoinbase(int nHeight, uint256 &hash) const;
    uint32_t GetLastImportHeight() const { return nLastImportHeight; }
    int COINBASE_MATURITY() const { return nMaturity; }
    CAmount MaxMoneyOut() const { return nMaxMoneyOut; }
    /** The masternode count that we will allow the see-saw reward payments to be off by */
    int MasternodeCountDrift() const { return nMasternodeCountDrift; }
    const CBlock& GenesisBlock() const { return genesis; }
    /** Default value for -checkmempool and -checkblockindex argument */
    bool DefaultConsistencyChecks() const { return fDefaultConsistencyChecks; }
    /** Policy: Filter transactions that do not match well-defined patterns */
    bool RequireStandard() const { return fRequireStandard; }
    uint64_t PruneAfterHeight() const { return nPruneAfterHeight; }
    /** Make miner stop after a block is found. In RPC, don't return until nGenProcLimit blocks are generated */
    bool MineBlocksOnDemand() const { return fMineBlocksOnDemand; }
    /** Return the BIP70 network string (main, test or regtest) */
    std::string NetworkIDString() const { return strNetworkID; }
    /** Return true if the fallback fee is by default enabled for this network */
    bool IsFallbackFeeEnabled() const { return m_fallback_fee_enabled; }
    /** Return the list of hostnames to look up for DNS seeds */
    const std::vector<std::string>& DNSSeeds() const { return vSeeds; }
    const std::vector<unsigned char>& Base58Prefix(Base58Type type) const { return base58Prefixes[type]; }
    const std::vector<unsigned char>& Bech32Prefix(Base58Type type) const { return bech32Prefixes[type]; }
    const std::string& Bech32HRP() const { return bech32_hrp; }
    const std::vector<SeedSpec6>& FixedSeeds() const { return vFixedSeeds; }
    const CCheckpointData& Checkpoints() const { return checkpointData; }
    const ChainTxData& TxData() const { return chainTxData; }

    bool IsBech32Prefix(const std::vector<unsigned char> &vchPrefixIn) const;
    bool IsBech32Prefix(const std::vector<unsigned char> &vchPrefixIn, CChainParams::Base58Type &rtype) const;
    bool IsBech32Prefix(const char *ps, size_t slen, CChainParams::Base58Type &rtype) const;

    std::string NetworkID() const { return strNetworkID; }

    void SetCoinYearReward(int64_t nCoinYearReward_)
    {
        assert(strNetworkID == "regtest");
        nCoinYearReward = nCoinYearReward_;
    }

    void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout);

    //DASH
    /** Allow nodes with the same address and multiple ports */
    bool AllowMultiplePorts() const { return fAllowMultiplePorts; }
    int PoolMaxTransactions() const { return nPoolMaxTransactions; }
    int FulfilledRequestExpireTime() const { return nFulfilledRequestExpireTime; }
    const std::string& SporkAddress() const { return strSporkAddress; }
    //WISPR
    int NEW_PROTOCOLS_STARTHEIGHT() const { return nNewProtocolStartHeightPiv; }
    int NEW_PROTOCOLS_STARTTIME() const { return nNewProtocolStartTime; }
    /** Zerocoin **/
    std::string Zerocoin_Modulus() const { return zerocoinModulus; }
    libzerocoin::ZerocoinParams* Zerocoin_Params(bool useModulusV1) const;
    int Zerocoin_MaxSpendsPerTransaction() const { return nMaxZerocoinSpendsPerTransaction; }
    CAmount Zerocoin_MintFee() const { return nMinZerocoinMintFee; }
    int Zerocoin_MintRequiredConfirmations() const { return nMintRequiredConfirmations; }
    int Zerocoin_RequiredAccumulation() const { return nRequiredAccumulation; }
    int Zerocoin_DefaultSpendSecurity() const { return nDefaultSecurityLevel; }
    int Zerocoin_HeaderVersion() const { return nZerocoinHeaderVersion; }
    int Zerocoin_RequiredStakeDepth() const { return nZerocoinRequiredStakeDepth; }
    int64_t TargetTimespanV1() const {
        return nTargetTimespanV1;
    }
    int64_t TargetSpacingV1() const {
        return nTargetSpacingV1;
    }
    int64_t IntervalV1() const {
        return nTargetTimespanV1 / nTargetSpacingV1;
    }
    int64_t TargetTimespanV2() const {
        return nTargetTimespanV2;
    }
    int64_t TargetSpacingV2() const {
        return nTargetSpacingV2;
    }
    int64_t IntervalV2() const {
        return nTargetTimespanV2 / nTargetSpacingV2;
    }
    const uint256& ProofOfWorkLimit() const { return bnProofOfWorkLimit; }
    const uint256& ProofOfStakeLimit() const { return bnProofOfStakeLimit; }
    int LAST_POW_BLOCK() const { return nLastPOWBlock; }

protected:
    CChainParams() {}

    void SetLastImportHeight()
    {
        nLastImportHeight = 0;
        for (auto cth : vImportedCoinbaseTxns)
            nLastImportHeight = std::max(nLastImportHeight, cth.nHeight);
    }

    Consensus::Params consensus;
    CMessageHeader::MessageStartChars pchMessageStart;
    int nDefaultPort;
    int nBIP44ID;

    uint32_t nModifierInterval;         // seconds to elapse before new modifier is computed
    uint32_t nStakeMinConfirmations;    // min depth in chain before staked output is spendable
    uint32_t nTargetSpacing;            // targeted number of seconds between blocks
    uint32_t nTargetTimespan;

    uint32_t nStakeTimestampMask = (1 << 4) -1; // 4 bits, every kernel stake hash will change every 16 seconds
    int64_t nCoinYearReward = 2 * CENT; // 2% per year

    std::vector<CImportedCoinbaseTxn> vImportedCoinbaseTxns;
    uint32_t nLastImportHeight;       // set from vImportedCoinbaseTxns

    std::vector<std::pair<int64_t, DevFundSettings> > vDevFundSettings;


    uint64_t nPruneAfterHeight;
    std::vector<std::string> vSeeds;
    std::vector<unsigned char> base58Prefixes[MAX_BASE58_TYPES];
    std::vector<unsigned char> bech32Prefixes[MAX_BASE58_TYPES];
    std::string bech32_hrp;
    std::string strNetworkID;
    CBlock genesis;
    std::vector<SeedSpec6> vFixedSeeds;
    bool fDefaultConsistencyChecks;
    bool fRequireStandard;
    bool fMineBlocksOnDemand;
    CCheckpointData checkpointData;
    ChainTxData chainTxData;
    bool m_fallback_fee_enabled;

    //DASH
    bool fAllowMultiplePorts;
    bool fAllowMultipleAddressesFromGroup;
    int nPoolMaxTransactions;
    std::string strSporkAddress;

    //WISPR
    int nMasternodeCountDrift;
    int nMaturity;
    CAmount nMaxMoneyOut;
    std::string strObfuscationPoolDummyAddress;
    int64_t nStartMasternodePayments;
    std::string zerocoinModulus;
    int nMaxZerocoinSpendsPerTransaction;
    CAmount nMinZerocoinMintFee;
    int nMintRequiredConfirmations;
    int nRequiredAccumulation;
    int nDefaultSecurityLevel;
    int nZerocoinHeaderVersion;
    int64_t nBudget_Fee_Confirmations;
    int nZerocoinStartHeight;
    int nNewProtocolStartHeightPiv;
    int nNewProtocolStartHeightPart;
    int nZerocoinStartTime;
    int nNewProtocolStartTime;
    int nZerocoinRequiredStakeDepth;
    uint256 bnProofOfStakeLimit;
    int nFulfilledRequestExpireTime;
    int nLastPOWBlock;
    uint256 bnProofOfWorkLimit;
    int64_t nTargetTimespanV1;
    int64_t nTargetTimespanV2;
    int64_t nTargetSpacingV1;
    int64_t nTargetSpacingV2;
};

/**
 * Creates and returns a std::unique_ptr<CChainParams> of the chosen chain.
 * @returns a CChainParams* of the chosen chain.
 * @throws a std::runtime_error if the chain is not supported.
 */
std::unique_ptr<CChainParams> CreateChainParams(const std::string& chain);

/**
 * Return the currently selected parameters. This won't change after app
 * startup, except for unit tests.
 */
const CChainParams &Params();
const CChainParams *pParams();

/**
 * Sets the params returned by Params() to those for the given BIP70 chain name.
 * @throws std::runtime_error when the chain is not supported.
 */
void SelectParams(const std::string& chain);

/**
 * Toggle old parameters for unit tests
 */
void SetOldParams(std::unique_ptr<CChainParams> &params);
void ResetParams(std::string sNetworkId, bool fWisprModeIn);

/**
 * mutable handle to regtest params
 */
CChainParams &RegtestParams();

#endif // BITCOIN_CHAINPARAMS_H
