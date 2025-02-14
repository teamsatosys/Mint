MINT: Physical Bitcoin Bearer Device
[![Status: Alpha](https://img.shields.io/badge/Status-Alpha-orange.svg)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)]()
![mintv2](https://github.com/user-attachments/assets/9efd55ef-63d9-461b-b596-ebfd1218a14b)
![Mint_Top_PCB-removebg-preview](https://github.com/user-attachments/assets/c22058bf-2529-41b7-841b-06c61c55fcee)
![Mint_Layers_PCB-removebg-preview](https://github.com/user-attachments/assets/bf18814a-841f-4cd4-857b-7f719fb3d106)
![Mint Component List](https://github.com/user-attachments/assets/a64945a3-2fc1-44b0-96ae-0211bd5cb7f3)



Overview

Mint is a revolutionary physical Bitcoin bearer instrument that enables trustless, offline transfer of Bitcoin value through a sophisticated hardware implementation. Inspired by traditional bearer bonds, Mint combines cutting-edge cryptography with tamper-evident hardware to create a truly innovative financial instrument.

Core Technology

Mint operates on a "break-to-reveal" principle:
- **Secure at Rest**: Bitcoin private key remains encrypted while the circuit is intact
- **Physical Authentication**: Tamper-evident circuit paired with OTP memory burning
- **USB Interface**: Appears as a standard mass storage device
- **Visual Feedback**: RGB LED provides clear status indication
- **One-Time Use**: Physical breaking of the circuit reveals the private key

Hardware Architecture

Core Components
- RP2040 microcontroller (dual-core ARM Cortex M0+)
- WS2812B RGB LED for status indication
- Tamper-evident circuit implementation
- W25Q128JV Flash memory with OTP capability

Security Features

**Physical Security Layer**
```
Circuit Intact ─────┐
                    ├── LED Status
OTP Memory   ───────┘
```

**Cryptographic Layer**
```
User Entropy ──┐
               ├── Secure Key Generation ──► Encrypted Storage
Device ID ─────┘
```

Implementation Roadmap

Phase 1: Core Infrastructure
- [x] USB Mass Storage Implementation (FAT12)
- [x] LED Status System
- [x] Circuit Monitoring
- [ ] Basic Wallet Generation

Phase 2: Cryptographic Implementation
- [ ] Bitcoin Key Generation (BIP Standards)
- [ ] Secure Entropy Collection
- [ ] Private Key Encryption
- [ ] OTP Security Integration

Phase 3: User Experience
- [ ] Status LED Patterns
- [ ] USB Drive Interface
- [ ] Documentation Generation
- [ ] Error Handling

Security Architecture

Physical Security
- Tamper-evident circuit design
- OTP memory burning for permanent state recording
- Anti-tampering mechanical design

Cryptographic Security
- Standards-compliant Bitcoin key generation
- Hardware-enhanced entropy collection
- Secure key storage while circuit remains intact

State Machine
```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│              │     │              │     │              │
│   STANDBY    │────►│ INITIALIZING │────►│   MINTED     │
│              │     │              │     │              │
└──────────────┘     └──────────────┘     └──────────────┘
                                                │
                                                │
                                          ┌─────▼──────┐
                                          │            │
                                          │  REVEALED  │
                                          │            │
                                          └────────────┘
```

LED Status Indicators

| State        | LED Color | Pattern     | Meaning                    |
|--------------|-----------|-------------|----------------------------|
| Standby      | Red       | Solid       | Circuit intact, secure     |
| Initializing | Blue      | Pulsing     | Setting up device          |
| Generating   | Yellow    | Spinning    | Creating wallet            |
| Revealed     | Green     | Solid       | Private key accessible     |

Development Setup

Prerequisites
- Arduino IDE with RP2040 support
- USB Mass Storage libraries
- Bitcoin cryptography libraries
- Hardware testing equipment

Build Process
1. Clone repository
2. Install dependencies
3. Configure hardware settings
4. Build and flash

Testing Protocol

Component Testing
1. USB mounting verification
2. LED status accuracy
3. Circuit integrity monitoring
4. Wallet generation validation
5. OTP burning confirmation

Integration Testing
1. Full system functionality
2. Error handling
3. Edge case behavior
4. Long-term reliability

File Structure

```
src/
├── main.ino           # Program entry
├── mint.h/cpp         # Core functionality
├── mint_storage.h/cpp # Storage handling
├── mint_led.h/cpp     # LED management
└── mint_wallet.h/cpp  # Wallet operations
```

Contributing

We welcome contributions that enhance security, improve usability, or add features. Please review our contribution guidelines before submitting pull requests.

License

This project is licensed under the MIT License - see the LICENSE file for details.

---

**Warning**: This is alpha software. Use at your own risk. Not recommended for production use without thorough security audit.
