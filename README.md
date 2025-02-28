# 🔥 MINT: The Bitcoin Bearer Device 🔥

[![Status: Alpha](https://img.shields.io/badge/Status-Alpha-orange.svg)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)]()

[![Join our Telegram Channel](https://img.shields.io/badge/Join-Our%20Telegram-blue?logo=telegram)](https://t.me/bitcoinmint)


## 🚀 Project Overview

**Mint** is a physical **Bitcoin bearer instrument**, enabling secure, **trustless** transfers of Bitcoin value **offline**. Inspired by physical bearer bonds, this device ensures **tamper-proof ownership**—a true bitcoin bearer asset.

## 🏆 Core Features

✅ **USB Drive Interface** – Plug it in like a standard USB stick 

✅ **Tamper-Evident Security** – Physically breaking the circuit **permanently** reveals the private key 

✅ **Bitcoin Key Vault** – Stores a Bitcoin private key secured by the SE050 secure element

✅ **One-Time Programmable (OTP) Memory** – Ensures **irreversible circuit break detection** 

✅ **Visual Status Indicator** – RGB LED shows real-time device state

✅ **Secure Hardware** – Integrates NXP SE050 secure element for cryptographic operations

## 🔒 Security Architecture

Mint employs a defense-in-depth security approach with multiple layers:

1. **Hardware Security Module**: The SE050 secure element provides:
   - Hardware-backed secure key generation and storage
   - Tamper-resistant execution environment
   - Hardware cryptographic acceleration for secp256k1 operations
   - One-Time Programmable (OTP) memory for permanent tamper detection

2. **Physical Security**:
   - Break-circuit tamper detection with permanent state recording
   - Irreversible physical state change when opened

3. **Cryptographic Security**:
   - Industry-standard BIP32/39/44 key derivation
   - Constant-time implementations of all cryptographic operations
   - True hardware random number generation with health monitoring

4. **Operational Security**:
   - All sensitive operations occur within the secure element
   - Zero sensitive data exposure in main MCU memory
   - Explicit memory zeroing of all sensitive buffers

## 🔩 Hardware Breakdown

| **Component**                        | **Purpose**                                                |
| ------------------------------------ | ---------------------------------------------------------- |
| **RP2040 MCU**                       | Main processor, system control, and USB interface          |
| **NXP SE050 Secure Element**         | Cryptographic operations, tamper detection, key storage    |
| **WS2812B RGB LED**                  | Provides visual feedback on device state                   |
| **W25Q128JV Flash Memory**           | Storage for public address and device state                |
| **Physical Breakable Circuit Trace** | Mechanism for tamper detection                             |
| **USB Mass Storage Interface**       | Allows interaction with the device like a flash drive      |

## 🛠️ How It Works

### 1️⃣ **Circuit Intact (Sealed Mode)**

- Private key remains securely stored within the SE050 secure element
- Device functions as a USB drive, showing only the **public Bitcoin address**
- Status LED: **🔴 Red (Secure Mode)**

### 2️⃣ **Circuit Broken (Revealed Mode)**

- The private key is **permanently revealed** in WIF format
- OTP memory in SE050 is **burned** to prevent state reversal
- Status LED: **🟢 Green (Spent Mode)**

## 🧠 Technical Implementation

### Cryptographic Operations

- **Key Generation**: Hardware TRNG with NIST SP 800-90B health tests
- **BIP39**: Generates seed phrases from secure entropy
- **BIP32**: Hierarchical deterministic wallet support
- **BIP44**: Full derivation path support with multiple account types
- **Secure Element**: All operations executed in the SE050 secure boundary

### Tamper Evidence

1. When circuit is broken, an interrupt is triggered
2. Current state is captured and verified
3. OTP register in SE050 is permanently burned with tamper status
4. Device transitions to revealed state, exposing private key
5. Hardware enforced irreversibility prevents unauthorized state changes

### USB Mass Storage

- Device appears as a small USB drive with FAT12 filesystem
- User provides entropy by dropping any file onto the drive
- README file displays current device state and relevant information
- In tampered state, private key is displayed in WIF format for easy import

## 🏗️ Development Roadmap

1️⃣ ✅ Implement **USB Mass Storage** 

2️⃣ ✅ Add **LED Status Indication** 

3️⃣ ✅ Integrate **Circuit Monitoring** 

4️⃣ ✅ Implement **SE050 Secure Element Integration**

5️⃣ ✅ Implement **Bitcoin Wallet Generation** with BIP standards

6️⃣ ✅ Develop **Tamper-Evident OTP Security**

7️⃣ 🛡️ **Extensive Security Testing & Auditing**

8️⃣ 🔄 **Hardware Production & Distribution**

## 🏭 Manufacturing

The Mint device is designed for:

- Simple assembly with standard SMT processes
- Minimal component count for reliability
- Secure provisioning process for initial device setup
- Tamper-evident packaging

See `Fabrication Files/` directory for:
- Complete BOM (Bill of Materials)
- Gerber files for PCB manufacturing
- Pick and place files for automated assembly

## ⚖️ Security Considerations

The Mint device balances several key security considerations:

1. **Offline Security**: Functions completely without internet connectivity
2. **Physical Verification**: Anyone can visually inspect if the circuit is intact
3. **User Autonomy**: No third-party trust or external verification required
4. **Transparent Design**: Open-source hardware and software for security review

## 🔥 The Mint Difference: A True Bearer Asset

Unlike traditional **hardware wallets**, Mint is a **physical Bitcoin instrument**, designed for **offline trustless transactions**. Once the circuit is broken, **there's no going back**—just like a real-world bearer bond or cash.

## 🚀 Getting Started

### For Users

1. **Setup**: Plug in the device via USB
2. **Wallet Generation**: Drop any file onto the USB drive
3. **Funding**: Send Bitcoin to the displayed address
4. **Transfer**: Give the device to another person
5. **Redemption**: Break the circuit to reveal the private key

### For Developers

1. **Build Environment**: Set up Arduino IDE with RP2040 support
2. **Libraries**: Install required libraries (Adafruit_TinyUSB, Adafruit_NeoPixel, etc.)
3. **Hardware**: Connect SE050 to RP2040 via I2C
4. **Compile & Flash**: Upload the firmware to your RP2040 board
5. **Testing**: Verify all functionality using the test suite

## 🤝 Contributing

We welcome contributions from:
- Security researchers
- Hardware engineers
- Cryptographers
- Bitcoin developers
- UI/UX designers

Please see [CONTRIBUTING.md](CONTRIBUTING.md) for contribution guidelines.

## 📜 License

MIT License - Freedom to innovate, obligation to attribute.

## 📞 Contact

- Telegram: [Bitcoin Mint Channel](https://t.me/bitcoinmint)
- GitHub Issues: For bug reports and feature requests
