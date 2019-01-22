// Copyright (c) 2018 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef WISPR_INVALID_H
#define WISPR_INVALID_H

#endif //WISPR_INVALID_H

#include <libzerocoin/bignum.h>
#include <primitives/transaction.h>
#include <univalue/include/univalue.h>

namespace invalid_out {
extern std::set<CBigNum> setInvalidSerials;
extern std::set<COutPoint> setInvalidOutPoints;

UniValue read_json(const std::string& jsondata);

bool ContainsOutPoint(const COutPoint& out);
bool ContainsSerial(const CBigNum& bnSerial);
bool LoadOutpoints();
bool LoadSerials();
} // namespace invalid_out