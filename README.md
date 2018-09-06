WISPR Core integration/staging repository
=====================================

[![Build Status](https://travis-ci.org/WisprProject/core.svg?branch=0.3_RC)](https://travis-ci.org/WisprProject/core) [![GitHub version](https://badge.fury.io/gh/WisprProject%2FWISPR.svg)](https://badge.fury.io/gh/WisprProject%2FWISPR)

WISPR is an open source crypto-currency focused on fast private transactions with low transaction fees & environmental footprint.  It utilizes a custom Proof of Stake protocol for securing its network and uses an innovative variable seesaw reward mechanism that dynamically balances 90% of its block reward size between masternodes and staking nodes and 10% dedicated for budget proposals. The goal of WISPR is to achieve a decentralized sustainable crypto currency with near instant full-time private transactions, fair governance and community intelligence.
- Anonymized transactions using the [_Zerocoin Protocol_](http://www.wispr.tech/zwsp).
- Fast transactions featuring guaranteed zero confirmation transactions, we call it _SwiftX_.
- Decentralized blockchain voting utilizing Masternode technology to form a DAO. The blockchain will distribute monthly treasury funds based on successful proposals submitted by the community and voted on by the DAO.

More information at [wispr.tech](http://www.wispr.tech) Visit our ANN thread at [BitcoinTalk](http://www.bitcointalk.org/index.php?topic=1262920)

Wispr Core integration/staging repository
=====================================

Wispr is an experimental cryptocurrency which aims to bring a blockchain-backed secure messaging system, along with several other state-of-the-art blockchain features, as announced in our roadmap.<br>
Forked off of [PIVX](https://github.com/PIVX-Project/PIVX), it's a fully Proof-of-Stake cryptocurrency with a fair reward system that encourages every type of user to stake and make the network more secure and efficient.

For more info, visit us at [wispr.tech](http://wispr.tech) or check out our ANN thread at [BitcoinTalk](https://bitcointalk.org/index.php?topic=2561885).

### Coin Specs

<table>
<tr><td>P2P Port</td><td>17000</td></tr>
<tr><td>RPC Port</td><td>17001</td></tr>
<tr><td>PoW Algo</td><td>Scrypt*</td></tr>
<tr><td>PoS Algo</td><td>PoS 3.0</td></tr>
<tr><td>Annual Inflation Rate</td><td>25%</td></tr>
<tr><td>Max Reorganization Depth</td><td>500 blocks</td></tr>
<tr><td>Block Time</td><td>60 Seconds</td></tr>
<tr><td>Block Reward</td><td>5 WSP + tx Fees paid to miners</td></tr>
<tr><td>Min Tx Fee</td><td>0.0001 WSP</td></tr>
<tr><td>Difficulty Retargeting</td><td>Every Block</td></tr>
<tr><td>Coin Maturity</td><td>100 blocks</td></tr>
<tr><td>Confirmations</td><td>10</td></tr>
<tr><td>Stake Confirmations</td><td>100</td></tr>
<tr><td>Max Coin Supply</td><td>120,000,000 WSP</td></tr>
<tr><td>Premine</td><td>25,125,000 WSP*</td></tr>
</table>

### Premine
The initial coins were premined in a private PoW phase up to block 450. Each block held 125,000 coins and some blocks were PoS in order to make a total of 25,000,000 WSP. Due to mining calculations an extra 125,000 WSP was mined and later [burned](https://explorer.wispr.tech/tx/ccabff166654a078da5cda2aa758e1f801f14e8886c8b2fcc9e2d32126755fb9).<br>
To find out how the premine was distributed, check our [ANN thread](https://bitcointalk.org/index.php?topic=2561885). 

### Building Wispr
Check out <b>/doc</b> for specific OS build instructions.

### Contributing
Everyone is encouraged to contribute. This project generally follows [Bitcoin Core's development process](https://github.com/bitcoin/bitcoin/blob/master/CONTRIBUTING.md), with more specific information coming soon about workflow.

### Join our community
Want to get in touch or need any help? Come say hi to us on our active social platforms!<br>
[Telegram](https://t.me/wisprchat) | [Discord](https://discord.gg/c7dvEXt) | [Twitter](http://twitter.com/WisprTech/) | [Facebook](https://facebook.com/WisprTech) | [Official Website](https://wispr.tech/)
