🔥 MINT: The Bitcoin Bearer Device 🔥

[![Status: Alpha](https://img.shields.io/badge/Status-Alpha-orange.svg)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)]()

🚀 Project Overview

**Mint** is a physical **Bitcoin bearer instrument**, enabling secure, **trustless** transfers of Bitcoin value **offline**. Inspired by physical bearer bonds, this device ensures **tamper-proof ownership**—a true bitcoin bearer asset.

🏆 Core Features

✅ **USB Drive Interface** – Plug it in like a standard USB stick 

✅ **Tamper-Evident Security** – Physically breaking the circuit **permanently** reveals the private key 

✅ **Bitcoin Key Vault** – Stores an encrypted Bitcoin private key until activated 

✅ **One-Time Programmable (OTP) Memory** – Ensures **irreversible circuit break detection** 

✅ **Visual Status Indicator** – RGB LED shows real-time device state


🔩 Hardware Breakdown

| **Component**                        | **Purpose**                                                |
| ------------------------------------ | ---------------------------------------------------------- |
| **RP2040 MCU**                       | Handles key generation, encryption, and circuit monitoring |
| **WS2812B RGB LED**                  | Provides visual feedback on device state                   |
| **W25Q128JV Flash Memory**           | Secure storage of Bitcoin private key & public address     |
| **Physical Breakable Circuit Trace** | Mechanism for tamper detection                             |
| **USB Mass Storage Interface**       | Allows interaction with the device like a flash drive      |

🛠️ How It Works

1️⃣ **Circuit Intact (Sealed Mode)**

- The private key remains **encrypted**.
- The device functions as a USB drive, showing only the **public Bitcoin address**.
- Status LED: **🔴 Red (Secure Mode)**

2️⃣ **Circuit Broken (Revealed Mode)**

- The private key is **permanently revealed**.
- OTP memory is **burned** to prevent reversal.
- Status LED: **🟢 Green (Spent Mode)**

🔧 Implementation Steps

**Step 1: USB Mass Storage Setup**

- Device appears as an **8.2KB USB drive**
- Implements **FAT12 filesystem** for maximum compatibility
- Proper boot sector initialization

**Step 2: Circuit Break Detection**

```cpp
if (gpio_get(CIRCUIT_PIN) == HIGH) {
    // Circuit broken, expose private key
    revealPrivateKey();
}
```

- **HIGH = Circuit broken** → Private key is revealed
- **LOW = Circuit intact** → Private key remains encrypted

**Step 3: Wallet Generation**

- User drops **any file** onto the USB drive
- File content **seeds entropy** for key generation
- Bitcoin keypair is derived using **BIP standards**
- Encrypted **private key** stored in flash memory

**Step 4: OTP Security Activation**

- **When circuit breaks:**
  - OTP register **burned to 0x00** (irreversible)
  - Permanently marks the device as **spent**

**Step 5: User Interface & Status LED**

| **State**             | **LED Color** | **Description**            |
| --------------------- | ------------- | -------------------------- |
| **Secure (Intact)**   | 🔴 Red        | Private key protected      |
| **Spent (Broken)**    | 🟢 Green      | Private key revealed       |
| **Initializing**      | 🔵 Blue       | Device setup in progress   |
| **Generating Wallet** | 🟡 Yellow     | Key generation in progress |

🏗️ Development Roadmap

1️⃣ Implement **USB Mass Storage** ✅\
2️⃣ Add **LED Status Indication** ✅\
3️⃣ Integrate **Circuit Monitoring** 🔄\
4️⃣ Implement **Bitcoin Wallet Generation** 🔄\
5️⃣ Develop **Tamper-Evident OTP Security** 🔄\
6️⃣ **Extensive Security Testing** 🛡️

🔥 Security Layers

**Physical Security**

- Circuit must be **difficult to repair** without detection.
- **OTP burning** provides second-layer tamper evidence.

**Cryptographic Security**

- Proper **entropy collection**
- Secure **key generation & storage**
- **AES encryption** while circuit remains intact

**User Safety & Clarity**

- **Clear LED indicators** for user state awareness
- **No accidental key exposure**
- **USB storage contains a README** explaining device states

📂 File Structure

```
main.ino          - Core firmware
mint.h/cpp        - Device logic
mint_storage.h/cpp - USB & storage handling
mint_led.h/cpp    - LED status management
mint_wallet.h/cpp - Bitcoin wallet operations
```

🛠️ Testing Protocol

✔️ USB mounting & file operations\
✔️ LED status accuracy\
✔️ Wallet generation & entropy validation\
✔️ OTP burning effectiveness\
✔️ Tamper detection & circuit integrity verification

⚡ The Mint Difference: A True Bearer Asset

Unlike traditional **hardware wallets**, Mint is a **physical Bitcoin instrument**, designed for **offline trustless transactions**. Once the circuit is broken, **there’s no going back**—just like a real-world bearer bond or cash.

---

🎨 Visual References

![mintv2](https://github.com/user-attachments/assets/358ed475-1666-4fdf-9f24-85b0c5721bf6)
![Mint_Top_PCB-removebg-preview](https://github.com/user-attachments/assets/ebfa36c4-efb8-4c33-9b49-b35448d6788a)
![Mint_Layers_PCB-removebg-preview](https://github.com/user-attachments/assets/f1ca6875-9305-4681-b5b6-e75783d2bf76)
![Mint_bottom_PCB-removebg-preview](https://github.com/user-attachments/assets/48ca20a6-6ad1-4647-9a84-f48218eb581b)

---

🤖 Component List

![Mint Component List](https://github.com/user-attachments/assets/10cf52f9-9c92-4a53-b3e6-251adaa08b6a)



---

⚡ Key Generation Process

```cpp
// Generate Bitcoin Key
void generateWallet(uint8_t* entropy, size_t size) {
    uint8_t private_key[32];
    sha256(entropy, size, private_key);
    
    uint8_t public_key[33];
    generate_public_key(private_key, public_key);
    
    char address[35];
    generate_bitcoin_address(public_key, address);
}
```

🎯 Why Mint?

🚀 **Trustless** – No need to trust anyone, it’s **physically verifiable**\
🔥 **Self-contained** – Works **without internet or online verification**\
🔒 **Tamper-proof** – Physical and cryptographic security baked-in\
💎 **Bitcoin-Native** – Designed purely for Bitcoin, no shitcoins

---

🤝 Contributing:
We welcome contributions from security researchers, hardware engineers, and bitcoiners in general.

⚠️ Security Note:
Mint implements rigorous security standards. All implementations must follow our comprehensive security guidelines.

📜 License:
MIT License - Freedom to innovate, obligation to attribute.

