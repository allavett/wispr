// Copyright (c) 2017-2018 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef WISPR_STAKEINPUT_H
#define WISPR_STAKEINPUT_H

#include <streams.h>
#include <vector>
#include <primitives/zerocoin.h>
#include "libzerocoin/CoinSpend.h"


class CKeyStore;
class CHDWallet;
class CWalletTx;

class CStakeInput
{
protected:
    CBlockIndex* pindexFrom;

public:
    virtual ~CStakeInput() = default;;
    virtual CBlockIndex* GetIndexFrom() = 0;
    virtual bool CreateTxIn(CHDWallet* pwallet, CTxIn& txIn, uint256 hashTxOut = 0) = 0;
    virtual bool GetTxFrom(CTransaction& tx) = 0;
    virtual CAmount GetValue() = 0;
    virtual bool CreateTxOuts(CHDWallet* pwallet, std::vector<CTxOut>& vout, CAmount nTotal) = 0;
    virtual bool GetModifier(uint64_t& nStakeModifier) = 0;
    virtual bool IsZWSP() = 0;
    virtual CDataStream GetUniqueness() = 0;
};


// zWSPStake can take two forms
// 1) the stake candidate, which is a zcmint that is attempted to be staked
// 2) a staked zwsp, which is a zcspend that has successfully staked
class CZWspStake : public CStakeInput
{
private:
    uint32_t nChecksum;
    bool fMint;
    libzerocoin::CoinDenomination denom;
    uint256 hashSerial;

public:
    explicit CZWspStake(libzerocoin::CoinDenomination denom, const uint256& hashSerial)
    {
        this->denom = denom;
        this->hashSerial = hashSerial;
        this->pindexFrom = nullptr;
        fMint = true;
    }

    explicit CZWspStake(const libzerocoin::CoinSpend& spend);

    CBlockIndex* GetIndexFrom() override;
    bool GetTxFrom(CTransaction& tx) override;
    CAmount GetValue() override;
    bool GetModifier(uint64_t& nStakeModifier) override;
    CDataStream GetUniqueness() override;
    bool CreateTxIn(CHDWallet* pwallet, CTxIn& txIn, uint256 hashTxOut = 0) override;
    bool CreateTxOuts(CHDWallet* pwallet, std::vector<CTxOut>& vout, CAmount nTotal) override;
    bool MarkSpent(CHDWallet* pwallet, const uint256& txid);
    bool IsZWSP() override { return true; }
    int GetChecksumHeightFromMint();
    int GetChecksumHeightFromSpend();
    uint32_t GetChecksum();
};

class CWspStake : public CStakeInput
{
private:
    CTransaction txFrom;
    unsigned int nPosition;
public:
    CWspStake()
    {
        this->pindexFrom = nullptr;
    }

    bool SetInput(CTransaction txPrev, unsigned int n);

    CBlockIndex* GetIndexFrom() override;
    bool GetTxFrom(CTransaction& tx) override;
    CAmount GetValue() override;
    bool GetModifier(uint64_t& nStakeModifier) override;
    CDataStream GetUniqueness() override;
    bool CreateTxIn(CHDWallet* pwallet, CTxIn& txIn, uint256 hashTxOut = 0) override;
    bool CreateTxOuts(CHDWallet* pwallet, std::vector<CTxOut>& vout, CAmount nTotal) override;
    bool IsZWSP() override { return false; }
};


#endif //WISPR_STAKEINPUT_H
