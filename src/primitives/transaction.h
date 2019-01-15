// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PRIMITIVES_TRANSACTION_H
#define BITCOIN_PRIMITIVES_TRANSACTION_H

#include <stdint.h>
#include <amount.h>
#include <script/script.h>
#include <serialize.h>
#include <uint256.h>
#include <pubkey.h>
#include <consensus/consensus.h>

#include <secp256k1_rangeproof.h>
#include <utility>

static const int SERIALIZE_TRANSACTION_NO_WITNESS = 0x04;

static const uint8_t WISPR_BLOCK_VERSION = 0x10;
static const uint8_t WISPR_TXN_VERSION = 0x03;
static const uint8_t MAX_WISPR_TXN_VERSION = 0x03;
static const uint8_t BTC_TXN_VERSION = 0x02;


enum OutputTypes
{
    OUTPUT_NULL             = 0, // marker for CCoinsView (0.14)
    OUTPUT_STANDARD         = 1,
    OUTPUT_CT               = 2,
    OUTPUT_RINGCT           = 3,
    OUTPUT_DATA             = 4,
};

enum TransactionTypes
{
    TXN_STANDARD            = 1,
    TXN_COINBASE            = 1,
    TXN_COINSTAKE           = 1,
//    TXN_STANDARD            = 0,
//    TXN_COINBASE            = 1,
//    TXN_COINSTAKE           = 2,
};

enum DataOutputTypes
{
    DO_NULL                 = 0, // reserved
    DO_NARR_PLAIN           = 1,
    DO_NARR_CRYPT           = 2,
    DO_STEALTH              = 3,
    DO_STEALTH_PREFIX       = 4,
    DO_VOTE                 = 5,
    DO_FEE                  = 6,
    DO_DEV_FUND_CFWD        = 7,
    DO_FUND_MSG             = 8,
};

inline bool IsWisprTxVersion(int nVersion)
{
    return (nVersion & 0x0) >= WISPR_TXN_VERSION;
//    return (nVersion & 0xFF) >= WISPR_TXN_VERSION;
}

/** An outpoint - a combination of a transaction hash and an index n into its vout */
class COutPoint
{
public:
    uint256 hash;
    uint32_t n;

    static const uint32_t ANON_MARKER = 0xffffffa0;

    COutPoint(): n((uint32_t) -1) { }
    COutPoint(uint256 hashIn, uint32_t nIn): hash(std::move(hashIn)), n(nIn) { }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
//        READWRITE(*this);
        READWRITE(hash);
        READWRITE(n);
    }

    void SetNull() { hash.SetNull(); n = (uint32_t) -1; }
    bool IsNull() const { return (hash.IsNull() && n == (uint32_t) -1); }

    friend bool operator<(const COutPoint& a, const COutPoint& b)
    {
        int cmp = a.hash.Compare(b.hash);
        return cmp < 0 || (cmp == 0 && a.n < b.n);
    }

    friend bool operator==(const COutPoint& a, const COutPoint& b)
    {
        return (a.hash == b.hash && a.n == b.n);
    }

    friend bool operator!=(const COutPoint& a, const COutPoint& b)
    {
        return !(a == b);
    }

    bool IsAnonInput() const
    {
        return n == ANON_MARKER;
    };

    std::string ToString() const;
    std::string ToStringShort() const;
};

/** An input of a transaction.  It contains the location of the previous
 * transaction's output that it claims and a signature that matches the
 * output's public key.
 */
class CTxIn
{
public:
    COutPoint prevout;
    CScript scriptSig;
    uint32_t nSequence;
    CScript prevPubKey;
    CScriptWitness scriptData; //!< Non prunable
    CScriptWitness scriptWitness; //!< Only serialized through CTransaction

    /* Setting nSequence to this value for every input in a transaction
     * disables nLockTime. */
    static const uint32_t SEQUENCE_FINAL = std::numeric_limits<unsigned int>::max();
//    static const uint32_t SEQUENCE_FINAL = 0xffffffff;

    /* Below flags apply in the context of BIP 68*/
    /* If this flag set, CTxIn::nSequence is NOT interpreted as a
     * relative lock-time. */
    static const uint32_t SEQUENCE_LOCKTIME_DISABLE_FLAG = (1U << 31);

    /* If CTxIn::nSequence encodes a relative lock-time and this flag
     * is set, the relative lock-time has units of 512 seconds,
     * otherwise it specifies blocks with a granularity of 1. */
    static const uint32_t SEQUENCE_LOCKTIME_TYPE_FLAG = (1 << 22);

    /* If CTxIn::nSequence encodes a relative lock-time, this mask is
     * applied to extract that lock-time from the sequence field. */
    static const uint32_t SEQUENCE_LOCKTIME_MASK = 0x0000ffff;

    /* In order to use the same number of bits to encode roughly the
     * same wall-clock duration, and because blocks are naturally
     * limited to occur every 600s on average, the minimum granularity
     * for time-based relative lock-time is fixed at 512 seconds.
     * Converting from CTxIn::nSequence to seconds is performed by
     * multiplying by 512 = 2^9, or equivalently shifting up by
     * 9 bits. */
    static const int SEQUENCE_LOCKTIME_GRANULARITY = 9;

    CTxIn()
    {
        nSequence = std::numeric_limits<unsigned int>::max();
    }

    explicit CTxIn(COutPoint prevoutIn, CScript scriptSigIn=CScript(), uint32_t nSequenceIn=std::numeric_limits<unsigned int>::max());
    CTxIn(uint256 hashPrevTx, uint32_t nOut, CScript scriptSigIn=CScript(), uint32_t nSequenceIn=std::numeric_limits<unsigned int>::max());

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(prevout);
        READWRITE(scriptSig);
        READWRITE(nSequence);

        if (IsAnonInput())
        {
            READWRITE(scriptData.stack);
        };
    }

    friend bool operator==(const CTxIn& a, const CTxIn& b)
    {
        return (a.prevout   == b.prevout &&
                a.scriptSig == b.scriptSig &&
                a.nSequence == b.nSequence);
    }

    friend bool operator!=(const CTxIn& a, const CTxIn& b)
    {
        return !(a == b);
    }

    bool IsAnonInput() const
    {
        return prevout.IsAnonInput();
    };

    bool SetAnonInfo(uint32_t nInputs, uint32_t nRingSize)
    {
        memcpy(prevout.hash.begin(), &nInputs, 4);
        memcpy(prevout.hash.begin()+4, &nRingSize, 4);
        return true;
    };

    bool GetAnonInfo(uint32_t &nInputs, uint32_t &nRingSize) const
    {
        memcpy(&nInputs, prevout.hash.begin(), 4);
        memcpy(&nRingSize, prevout.hash.begin()+4, 4);
        return true;
    };

    std::string ToString() const;
    uint256 GetHash() const;

};

class CTxOutStandard;
class CTxOutCT;
class CTxOutRingCT;
class CTxOutData;

class CTxOutBase
{
public:
    CTxOutBase(uint8_t v) : nVersion(v) {};
    virtual ~CTxOutBase() = default;;
    uint8_t nVersion;

    template<typename Stream>
    void Serialize(Stream &s) const
    {
        switch (nVersion)
        {
            case OUTPUT_STANDARD:
                s << *((CTxOutStandard*) this);
                break;
            case OUTPUT_CT:
                s << *((CTxOutCT*) this);
                break;
            case OUTPUT_RINGCT:
                s << *((CTxOutRingCT*) this);
                break;
            case OUTPUT_DATA:
                s << *((CTxOutData*) this);
                break;
            default:
                assert(false);
        };
    };

    template<typename Stream>
    void Unserialize(Stream &s)
    {
        switch (nVersion)
        {
            case OUTPUT_STANDARD:
                s >> *((CTxOutStandard*) this);
                break;
            case OUTPUT_CT:
                s >> *((CTxOutCT*) this);
                break;
            case OUTPUT_RINGCT:
                s >> *((CTxOutRingCT*) this);
                break;
            case OUTPUT_DATA:
                s >> *((CTxOutData*) this);
                break;
            default:
                assert(false);
        };
    };

    uint8_t GetType() const
    {
        return nVersion;
    };

    bool IsType(uint8_t nType) const
    {
        return nVersion == nType;
    };

    bool IsStandardOutput() const
    {
        return nVersion == OUTPUT_STANDARD;
    };

    const CTxOutStandard *GetStandardOutput() const
    {
        assert(nVersion == OUTPUT_STANDARD);
        return (CTxOutStandard*)this;
    };

    virtual bool IsEmpty() const { return false;}

    void SetValue(CAmount value);

    virtual CAmount GetValue() const;

    virtual bool PutValue(std::vector<uint8_t> &vchAmount) const { return false; };

    virtual bool GetScriptPubKey(CScript &scriptPubKey_) const { return false; };
    virtual const CScript *GetPScriptPubKey() const { return nullptr; };

    virtual secp256k1_pedersen_commitment *GetPCommitment() { return nullptr; };
    virtual std::vector<uint8_t> *GetPRangeproof() { return nullptr; };

    virtual bool GetCTFee(CAmount &nFee) const { return false; };
    virtual bool SetCTFee(CAmount &nFee) { return false; };
    virtual bool GetDevFundCfwd(CAmount &nCfwd) const { return false; };

    std::string ToString() const;
};

#define OUTPUT_PTR std::shared_ptr
using CTxOutBaseRef = std::shared_ptr<CTxOutBase>;
#define MAKE_OUTPUT std::make_shared

class CTxOutStandard : public CTxOutBase
{
public:
    CTxOutStandard() : CTxOutBase(OUTPUT_STANDARD) {};
    CTxOutStandard(const CAmount& nValueIn, CScript scriptPubKeyIn);

    CAmount nValue;
    CScript scriptPubKey;

    template<typename Stream>
    void Serialize(Stream &s) const
    {
        s << nValue;
        s << *(CScriptBase*)(&scriptPubKey);
    };

    template<typename Stream>
    void Unserialize(Stream &s)
    {
        s >> nValue;
        s >> *(CScriptBase*)(&scriptPubKey);
    };


    bool IsEmpty() const override
    {
        return (nValue == 0 && scriptPubKey.empty());
    }

    bool PutValue(std::vector<uint8_t> &vchAmount) const override
    {
        vchAmount.resize(8);
        memcpy(&vchAmount[0], &nValue, 8);
        return true;
    };

    CAmount GetValue() const override
    {
        return nValue;
    };

    bool GetScriptPubKey(CScript &scriptPubKey_) const override
    {
        scriptPubKey_ = scriptPubKey;
        return true;
    };

    virtual const CScript *GetPScriptPubKey() const override
    {
        return &scriptPubKey;
    };
};

class CTxOutCT : public CTxOutBase
{
public:
    CTxOutCT() : CTxOutBase(OUTPUT_CT) {};
    secp256k1_pedersen_commitment commitment;
    std::vector<uint8_t> vData; // first 33 bytes is always ephemeral pubkey, can contain token for stealth prefix matching
    CScript scriptPubKey;
    std::vector<uint8_t> vRangeproof;

    template<typename Stream>
    void Serialize(Stream &s) const
    {
        const bool fAllowWitness = !(s.GetVersion() & SERIALIZE_TRANSACTION_NO_WITNESS);
        s.write((char*)&commitment.data[0], 33);
        s << vData;
        s << *(CScriptBase*)(&scriptPubKey);

        if (fAllowWitness)
        {
            s << vRangeproof;
        } else
        {
            WriteCompactSize(s, 0);
        };
    };

    template<typename Stream>
    void Unserialize(Stream &s)
    {
        s.read((char*)&commitment.data[0], 33);
        s >> vData;
        s >> *(CScriptBase*)(&scriptPubKey);

        s >> vRangeproof;
    };

    bool PutValue(std::vector<uint8_t> &vchAmount) const override
    {
        vchAmount.resize(33);
        memcpy(&vchAmount[0], commitment.data, 33);
        return true;
    };

    bool GetScriptPubKey(CScript &scriptPubKey_) const override
    {
        scriptPubKey_ = scriptPubKey;
        return true;
    };

    virtual const CScript *GetPScriptPubKey() const override
    {
        return &scriptPubKey;
    };

    secp256k1_pedersen_commitment *GetPCommitment() override
    {
        return &commitment;
    };

    std::vector<uint8_t> *GetPRangeproof() override
    {
        return &vRangeproof;
    };
};

class CTxOutRingCT : public CTxOutBase
{
public:
    CTxOutRingCT() : CTxOutBase(OUTPUT_RINGCT) {};
    CCmpPubKey pk;
    std::vector<uint8_t> vData; // first 33 bytes is always ephemeral pubkey, can contain token for stealth prefix matching
    secp256k1_pedersen_commitment commitment;
    std::vector<uint8_t> vRangeproof;

    template<typename Stream>
    void Serialize(Stream &s) const
    {
        const bool fAllowWitness = !(s.GetVersion() & SERIALIZE_TRANSACTION_NO_WITNESS);
        s.write((char*)pk.begin(), 33);
        s.write((char*)&commitment.data[0], 33);
        s << vData;

        if (fAllowWitness)
        {
            s << vRangeproof;
        } else
        {
            WriteCompactSize(s, 0);
        };
    };

    template<typename Stream>
    void Unserialize(Stream &s)
    {
        s.read((char*)pk.ncbegin(), 33);
        s.read((char*)&commitment.data[0], 33);
        s >> vData;
        s >> vRangeproof;
    };

    bool PutValue(std::vector<uint8_t> &vchAmount) const override
    {
        vchAmount.resize(33);
        memcpy(&vchAmount[0], commitment.data, 33);
        return true;
    };

    secp256k1_pedersen_commitment *GetPCommitment() override
    {
        return &commitment;
    };

    std::vector<uint8_t> *GetPRangeproof() override
    {
        return &vRangeproof;
    };
};

class CTxOutData : public CTxOutBase
{
public:
    CTxOutData() : CTxOutBase(OUTPUT_DATA) {};
    CTxOutData(std::vector<uint8_t> vData_) : CTxOutBase(OUTPUT_DATA), vData(std::move(vData_)) {};

    std::vector<uint8_t> vData;

    template<typename Stream>
    void Serialize(Stream &s) const
    {
        s << vData;
    };

    template<typename Stream>
    void Unserialize(Stream &s)
    {
        s >> vData;
    };

    bool GetCTFee(CAmount &nFee) const override
    {
        if (vData.size() < 2 || vData[0] != DO_FEE)
            return false;

        size_t nb;
        return (0 == GetVarInt(vData, 1, (uint64_t&)nFee, nb));
    };

    bool SetCTFee(CAmount &nFee) override
    {
        vData.clear();
        vData.push_back(DO_FEE);
        return (0 == PutVarInt(vData, nFee));
    }

    bool GetDevFundCfwd(CAmount &nCfwd) const override
    {
        if (vData.size() < 5)
            return false;

        size_t ofs = 4; // first 4 bytes will be height
        while (ofs < vData.size())
        {
            if (vData[ofs] == DO_VOTE)
            {
                ofs += 5;
                continue;
            };
            if (vData[ofs] == DO_DEV_FUND_CFWD)
            {
                ofs++;
                size_t nb;
                return (0 == GetVarInt(vData, ofs, (uint64_t&)nCfwd, nb));
            };
            break;
        };

        return false;
    };
};


/** An output of a transaction.  It contains the public key that the next input
 * must be able to sign with to claim it.
 */
class CTxOut
{
public:
    CAmount nValue;
    CScript scriptPubKey;
    int nRounds;

    CTxOut()
    {
        SetNull();
    }

    CTxOut(const CAmount& nValueIn, CScript scriptPubKeyIn);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(nValue);
        READWRITE(scriptPubKey);
    }

    void SetNull()
    {
        nValue = -1;
        scriptPubKey.clear();
        nRounds = -10; // an initial value, should be no way to get this by calculations

    }

    void SetEmpty()
    {
        nValue = 0;
        scriptPubKey.clear();
    }

    bool IsNull() const
    {
        return (nValue == -1);
    }

    bool IsEmpty() const
    {
        return (nValue == 0 && scriptPubKey.empty());
    }

    friend bool operator==(const CTxOut& a, const CTxOut& b)
    {
        return (a.nValue       == b.nValue &&
                a.scriptPubKey == b.scriptPubKey);
    }

    friend bool operator!=(const CTxOut& a, const CTxOut& b)
    {
        return !(a == b);
    }
    bool IsZerocoinMint() const
    {
        return !scriptPubKey.empty() && scriptPubKey.IsZerocoinMint();
    }
    std::string ToString() const;
    uint256 GetHash() const;

};

struct CMutableTransaction;

/**
 * Basic transaction serialization format:
 * - int32_t nVersion
 * - std::vector<CTxIn> vin
 * - std::vector<CTxOut> vout
 * - uint32_t nLockTime
 *
 * Extended transaction serialization format:
 * - int32_t nVersion
 * - unsigned char dummy = 0x00
 * - unsigned char flags (!= 0)
 * - std::vector<CTxIn> vin
 * - std::vector<CTxOut> vout
 * - if (flags & 1):
 *   - CTxWitness wit;
 * - uint32_t nLockTime
 */
template<typename Stream, typename TxType>
inline void UnserializeTransaction(TxType& tx, Stream& s) {
    const bool fAllowWitness = !(s.GetVersion() & SERIALIZE_TRANSACTION_NO_WITNESS);

//    printf("Unserialize: transaction\n");
//    int32_t nVersion = tx.nVersion;

//    tx.nVersion = 0;
    s >> tx.nVersion;
    uint8_t bv = tx.nVersion & 0x0;

    if (IsWisprTxVersion(tx.nVersion))
    {
        tx.nVersion = 0;
//        printf("Unserialize: WISPR_TXN_VERSION\n");
        tx.nVersion = bv;

        s >> bv;
        tx.nVersion |= bv<<8; // TransactionTypes

        s >> tx.nLockTime;

        tx.vin.clear();
        s >> tx.vin;

        size_t nOutputs = ReadCompactSize(s);
        tx.vpout.resize(nOutputs);
        for (size_t k = 0; k < tx.vpout.size(); ++k)
        {
            s >> bv;

            switch (bv)
            {
                case OUTPUT_STANDARD:
                    tx.vpout[k] = MAKE_OUTPUT<CTxOutStandard>();
                    break;
                case OUTPUT_CT:
                    tx.vpout[k] = MAKE_OUTPUT<CTxOutCT>();
                    break;
                case OUTPUT_RINGCT:
                    tx.vpout[k] = MAKE_OUTPUT<CTxOutRingCT>();
                    break;
                case OUTPUT_DATA:
                    tx.vpout[k] = MAKE_OUTPUT<CTxOutData>();
                    break;
                default:
                    return;
            };

            tx.vpout[k]->nVersion = bv;
            s >> *tx.vpout[k];
        }

        if (fAllowWitness)
        {
            for (auto &txin : tx.vin)
                s >> txin.scriptWitness.stack;
        };
        return;
    };

    if(tx.nVersion == 1){
//        printf("Unserialize: BTC_TXN_VERSION\n");
//        tx.nVersion = bv;
        s >> tx.nTime;
    }else if (tx.nVersion >= 0x03){
        printf("Unserialize nVersion = %d\n", tx.nVersion);
        tx.nVersion |= bv;
        s >> bv;
        tx.nVersion |= bv<<8;
        s >> bv;
        tx.nVersion |= bv<<16;
        s >> bv;
        tx.nVersion |= bv<<24;
    }
    unsigned char flags = 0;
    tx.vin.clear();
    tx.vout.clear();
    /* Try to read the vin. In case the dummy is there, this will be read as an empty vector. */
    s >> tx.vin;
    if (tx.vin.size() == 0 && fAllowWitness) {
        /* We read a dummy or an empty vin. */
        s >> flags;
        if (flags != 0) {
            s >> tx.vin;
            s >> tx.vout;
        }
    } else {
        /* We read a non-empty vin. Assume a normal vout follows. */
        s >> tx.vout;
    }
    if ((flags & 1) && fAllowWitness) {
        /* The witness flag is present, and we support witnesses. */
        flags ^= 1;
        for (size_t i = 0; i < tx.vin.size(); i++) {
            s >> tx.vin[i].scriptWitness.stack;
        }
    }
    if (flags) {
        /* Unknown flag in the serialization */
        throw std::ios_base::failure("Unknown transaction optional data");
    }
    s >> tx.nLockTime;
}

template<typename Stream, typename TxType>
inline void SerializeTransaction(const TxType& tx, Stream& s) {
    const bool fAllowWitness = !(s.GetVersion() & SERIALIZE_TRANSACTION_NO_WITNESS);

//    printf("Serialize: transaction\n");
    int32_t nVersion = tx.nVersion;
    if (IsWisprTxVersion(tx.nVersion))
    {
        printf("Serialize nVersion = %d\n", tx.nVersion);
//        printf("Serialize: transaction is wispr version\n");
        uint8_t bv = tx.nVersion & 0x0;
        s << bv;

        bv = (tx.nVersion>>8) & 0x0;
        s << bv; // TransactionType

        s << tx.nLockTime;
        s << tx.vin;

        WriteCompactSize(s, tx.vpout.size());
        for (size_t k = 0; k < tx.vpout.size(); ++k)
        {
            s << tx.vpout[k]->nVersion;
            s << *tx.vpout[k];
        };

        if (fAllowWitness)
        {
            for (auto &txin : tx.vin)
                s << txin.scriptWitness.stack;
        };
        return;
    };

    s << tx.nVersion;
    if(nVersion == 1) {
//        printf("Serialize: version 1\n");
        s << tx.nTime;
    }
    unsigned char flags = 0;
    // Consistency check
    if (fAllowWitness) {
        /* Check whether witnesses need to be serialized. */
        if (tx.HasWitness()) {
            flags |= 1;
        }
    }
    if (flags) {
        /* Use extended format in case witnesses are to be serialized. */
        std::vector<CTxIn> vinDummy;
        s << vinDummy;
        s << flags;
    }
    s << tx.vin;
    s << tx.vout;
    if (flags & 1) {
        for (size_t i = 0; i < tx.vin.size(); i++) {
            s << tx.vin[i].scriptWitness.stack;
        }
    }
    s << tx.nLockTime;
}


/** The basic transaction that is broadcasted on the network and contained in
 * blocks.  A transaction can contain multiple inputs and outputs.
 */
class CTransaction
{
public:
    // Default transaction version.
    static const int32_t CURRENT_VERSION=2;
    static const int32_t CURRENT_WISPR_VERSION=0x02;

    // Changing the default transaction version requires a two step process: first
    // adapting relay policy by bumping MAX_STANDARD_VERSION, and then later date
    // bumping the default CURRENT_VERSION at which point both CURRENT_VERSION and
    // MAX_STANDARD_VERSION will be equal.
    static const int32_t MAX_STANDARD_VERSION=2;
    static const int32_t MAX_STANDARD_WISPR_VERSION=0x02;

    // The local variables are made const to prevent unintended modification
    // without updating the cached hash value. However, CTransaction is not
    // actually immutable; deserialization and assignment are implemented,
    // and bypass the constness. This is safe, as they update the entire
    // structure, including the hash.
    const int32_t nVersion;
    unsigned int nTime;
    const std::vector<CTxIn> vin;
    const std::vector<CTxOut> vout;
    const std::vector<CTxOutBaseRef> vpout;
    const uint32_t nLockTime;

private:
    /** Memory only. */
    const uint256 hash;
    const uint256 m_witness_hash;

    uint256 ComputeHash() const;
    uint256 ComputeWitnessHash() const;

public:
    /** Construct a CTransaction that qualifies as IsNull() */
    CTransaction();
    ~CTransaction() = default;;

    /** Convert a CMutableTransaction into a CTransaction. */
    CTransaction(const CMutableTransaction &tx);
    CTransaction(CMutableTransaction &&tx);
    CTransaction& operator=(const CTransaction& tx);

    template <typename Stream>
    inline void Serialize(Stream& s) const {
        SerializeTransaction(*this, s);
    }

    /** This deserializing constructor is provided instead of an Unserialize method.
     *  Unserialize is not possible, since it would require overwriting const fields. */
    template <typename Stream>
    CTransaction(deserialize_type, Stream& s) : CTransaction(CMutableTransaction(deserialize, s)) {}

    bool IsNull() const {
        return vin.empty() && vout.empty() && vpout.empty();
    }

    bool IsWisprVersion() const {
        return IsWisprTxVersion(nVersion);
    }

    int GetType() const {
        return (nVersion >> 8) & 0x0;
    }

    size_t GetNumVOuts() const
    {
        return IsWisprTxVersion(nVersion) ? vpout.size() : vout.size();
    }

    const uint256& GetHash() const { return hash; }
    const uint256& GetWitnessHash() const { return m_witness_hash; };

    // Return sum of txouts.
    CAmount GetValueOut() const;
    // Compute priority, given priority of inputs and (optionally) tx size
    double ComputePriority(double dPriorityInputs, unsigned int nTxSize=0) const;
    // Compute modified tx size for priority calculation (optionally given tx size)
    unsigned int CalculateModifiedSize(unsigned int nTxSize=0) const;
    // Return sum of standard txouts and counts of output types
    CAmount GetPlainValueOut(size_t &nStandard, size_t &nCT, size_t &nRingCT) const;

    // GetValueIn() is a method on CCoinsViewCache, because
    // inputs must be known to compute value in.

    /**
     * Get the total transaction size in bytes, including witness data.
     * "Total Size" defined in BIP141 and BIP144.
     * @return Total transaction size in bytes
     */
    unsigned int GetTotalSize() const;

    bool IsZerocoinSpend() const
    {
        return (vin.size() > 0 && vin[0].prevout.hash == 0 && vin[0].scriptSig[0] == OP_ZEROCOINSPEND);
    }

    bool IsZerocoinMint() const
    {
        for(const CTxOut& txout : vout) {
            if (txout.scriptPubKey.IsZerocoinMint())
                return true;
        }
        return false;
    }

    bool ContainsZerocoins() const
    {
        return IsZerocoinSpend() || IsZerocoinMint();
    }

    CAmount GetZerocoinMinted() const;
    CAmount GetZerocoinSpent() const;
    int GetZerocoinMintCount() const;


    bool IsCoinBase() const
    {
        if(vin.size() == 1 && vin[0].prevout.IsNull() && !ContainsZerocoins()){
            return true;
        }

        return (GetType() == TXN_COINBASE
                && vin.size() == 1 && vin[0].prevout.IsNull() && !ContainsZerocoins()); // TODO [rm]?
    }

    bool IsCoinStake() const
    {
        if (vin.empty())
            return false;

        // ppcoin: the coin stake transaction is marked with the first output empty
        bool fAllowNull = vin[0].scriptSig.IsZerocoinSpend();
        if (vin[0].prevout.IsNull() && !fAllowNull)
            return false;

        if (vin.size() > 0 && vout.size() >= 2 && vout[0].IsEmpty()){
            return true;
        }

        return GetType() == TXN_COINSTAKE
            && vin.size() > 0 && vpout.size() >= 2
            && vout[0].IsEmpty()
            && vpout[0]->nVersion == OUTPUT_DATA
            && vpout[1]->nVersion == OUTPUT_STANDARD;
    }

    bool GetCoinStakeHeight(int &height) const
    {
        if (vpout.size() < 2 || vpout[0]->nVersion != OUTPUT_DATA)
            return false;

        std::vector<uint8_t> &vData = ((CTxOutData*)vpout[0].get())->vData;
        if (vData.size() < 4)
            return false;

        memcpy(&height, &vData[0], 4);
        return true;
    }

    bool GetCTFee(CAmount &nFee) const
    {
        if (vpout.size() < 2 || vpout[0]->nVersion != OUTPUT_DATA)
            return false;

        return vpout[0]->GetCTFee(nFee);
    }

    bool GetDevFundCfwd(CAmount &nCfwd) const
    {
        if (vpout.size() < 1 || vpout[0]->nVersion != OUTPUT_DATA)
            return false;

        return vpout[0]->GetDevFundCfwd(nCfwd);
    }

    friend bool operator==(const CTransaction& a, const CTransaction& b)
    {
        return a.hash == b.hash;
    }

    friend bool operator!=(const CTransaction& a, const CTransaction& b)
    {
        return a.hash != b.hash;
    }

    std::string ToString() const;

    bool HasWitness() const
    {
        for (const auto &i : vin) {
            if (!i.scriptWitness.IsNull()) {
                return true;
            }
        }
        return false;
    }
};

/** A mutable version of CTransaction. */
struct CMutableTransaction
{
    int32_t nVersion;
    unsigned int nTime;
    std::vector<CTxIn> vin;
    std::vector<CTxOut> vout;
    std::vector<CTxOutBaseRef> vpout;
    uint32_t nLockTime;

    CMutableTransaction();
    CMutableTransaction(const CTransaction& tx);

    template <typename Stream>
    inline void Serialize(Stream& s) const {
        SerializeTransaction(*this, s);
    }

    template <typename Stream>
    inline void Unserialize(Stream& s) {
        UnserializeTransaction(*this, s);
    }

    template <typename Stream>
    CMutableTransaction(deserialize_type, Stream& s) {
        Unserialize(s);
    }

    void SetType(int type) {
        nVersion |= (type & 0x0) << 8;
//        printf("%d\n", nVersion);
    }

    bool IsWisprVersion() const {
        return IsWisprTxVersion(nVersion);
    }

    int GetType() const {
        return (nVersion >> 8) & 0x0;
    }

    bool IsCoinStake() const
    {
        return GetType() == TXN_COINSTAKE
            && vin.size() > 0 && vpout.size() > 1
            && vpout[0]->nVersion == OUTPUT_DATA
            && vpout[1]->nVersion == OUTPUT_STANDARD;
    }

    size_t GetNumVOuts() const
    {
        return IsWisprTxVersion(nVersion) ? vpout.size() : vout.size();
    }

    /** Compute the hash of this CMutableTransaction. This is computed on the
     * fly, as opposed to GetHash() in CTransaction, which uses a cached result.
     */
    uint256 GetHash() const;

    bool HasWitness() const
    {
        for (const auto &i : vin) {
            if (!i.scriptWitness.IsNull()) {
                return true;
            }
        }
        return false;
    }
    friend bool operator==(const CMutableTransaction& a, const CMutableTransaction& b)
    {
        return a.GetHash() == b.GetHash();
    }

    friend bool operator!=(const CMutableTransaction& a, const CMutableTransaction& b)
    {
        return !(a == b);
    }

    std::string ToString() const;
};

using CTransactionRef = std::shared_ptr<const CTransaction>;
static inline CTransactionRef MakeTransactionRef() { return std::make_shared<const CTransaction>(); }
template <typename Tx> static inline CTransactionRef MakeTransactionRef(Tx&& txIn) { return std::make_shared<const CTransaction>(std::forward<Tx>(txIn)); }

/** Implementation of BIP69
 * https://github.com/bitcoin/bips/blob/master/bip-0069.mediawiki
 */
struct CompareInputBIP69
{
    inline bool operator()(const CTxIn& a, const CTxIn& b) const
    {
        if (a.prevout.hash == b.prevout.hash) return a.prevout.n < b.prevout.n;

        uint256 hasha = a.prevout.hash;
        uint256 hashb = b.prevout.hash;

        using rev_it = std::reverse_iterator<const unsigned char *>;
        rev_it rita = rev_it(hasha.end());
        rev_it ritb = rev_it(hashb.end());

        return std::lexicographical_compare(rita, rita + hasha.size(), ritb, ritb + hashb.size());
    }
};

struct CompareOutputBIP69
{
    inline bool operator()(const CTxOut& a, const CTxOut& b) const
    {
        return a.nValue < b.nValue || (a.nValue == b.nValue && a.scriptPubKey < b.scriptPubKey);
    }
};

#endif // BITCOIN_PRIMITIVES_TRANSACTION_H
