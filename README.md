# Bitcoin Silver (BTCS)

Bitcoin Silver is a fork of Bitcoin designed to provide a faster network and continuous development. This README provides an overview of the project, installation instructions, and details on how to contribute.

## Overview

Bitcoin Silver aims to enhance the original Bitcoin network by:
- **Faster Transactions**: Improving transaction speeds to ensure a more efficient and scalable network.
- **Continuous Development**: Regular updates and community-driven enhancements to keep the network secure and up-to-date.

## Features

- **Speed**: Reduced block generation time to increase transaction throughput.
- **Security**: Enhanced security protocols to protect against attacks.
- **Scalability**: Optimized to handle a larger number of transactions per second.
- **Community-Driven**: Open-source project with contributions from developers worldwide.

## Specification

<table>
<tr><td>Coin Name</td><td>Bitcoin Silver</td></tr>
<tr><td>Ticker</td><td>BTCS</td></tr>
<tr><td>Type</td><td>PoW</td></tr>
<tr><td>Maturity</td><td>200</td></tr>
<tr><td>Max supply</td><td>21 000 000</td></tr>
<tr><td>P2P port</td><td>10566</td></tr>
<tr><td>RPC port</td><td>10567</td></tr>
<tr><td>Prefix</td><td>"B" / "bs1"</td></tr>
<tr><td>Halving</td><td>210 000</td></tr>
<tr><td>Block time</td><td>300 seconds</td></tr>
<tr><td>Diff time</td><td>~96 blocks</td></tr>
<tr><td>Block reward</td><td>50</td></tr>
<tr><td>Algo</td><td>SHA256</td></tr>
</table>

## Getting Started

### Prerequisites

Before you begin, ensure you have met the following requirements:
- **Operating System**: Linux, macOS, or Windows.
- **Dependencies**: Ensure you have `git`, `cmake`, and `gcc` or equivalent installed.

### Installation

1. **Clone the Repository**
```bash
   git clone https://github.com/mrvistos/engine.git
   cd engine
```

2. **Build the Source Code**
```bash
   cmake -B build
   cmake --build build    # use "-j N" for N parallel jobs
   cmake --install build  # optional
```

   3. **Run the Node**
```bash
   ./build/bin/bitcoinsilverd
```
## Usage

### Wallet Setup

1.  **Generate a New Wallet**
```bash
   ./bitcoinsilver-cli createwallet "mywallet"
```
    
-   **Get Wallet Address**
```bash
   ./bitcoinsilver-cli getnewaddress
```
### Mining

Start mining with the following command:
```bash
  ./bitcoinsilver-cli generatetoaddress n "mywalletaddress"
```
Replace `n` with the number of blocks you want to mine and `"mywalletaddress"` with your actual wallet address.

## Contributing

We welcome contributions from the community! To get started:

1.  Fork the repository on GitHub.
2.  Create a new branch (`git checkout -b feature/YourFeature`).
3.  Make your changes and commit them (`git commit -am 'Add new feature'`).
4.  Push to the branch (`git push origin feature/YourFeature`).
5.  Create a new Pull Request.

### Reporting Issues

If you encounter any issues, please report them on our [GitHub Issues](https://github.com/bitcoin-silver/core/issues) page.
