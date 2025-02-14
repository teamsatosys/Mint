PROJECT OVERVIEW:

The Mint device is a physical Bitcoin bearer instrument that allows secure transfer of bitcoin value between parties without requiring trust or online verification. It's inspired by physical bearer bonds.

CORE CONCEPT:
- Device acts as a USB drive
- Contains a Bitcoin private key that is encrypted while circuit is intact
- Breaking the circuit reveals the private key
- Physical tamper-evidence through circuit + OTP memory burning ensures authenticity

HARDWARE REQUIREMENTS:
1. RP2040 microcontroller
2. WS2812B RGB LED for status indication
3. Physical circuit trace that can be broken
4. Flash memory with OTP capability (W25Q128JV)

IMPLEMENTATION STEPS:

1. USB Mass Storage Implementation
```cpp
- Device must appear as standard USB drive (8.2KB minimum)
- Must implement FAT12 filesystem for compatibility
- Need proper boot sector initialization
- Working code exists for this in our earlier tests
```

2. Circuit Detection
```cpp
- Monitor GPIO pin (14) with pull-up resistor
- HIGH = Circuit broken
- LOW = Circuit intact
- LED indicates status (red=intact, green=broken)
```

3. Wallet Generation
```cpp
- Triggered when user drops any file on USB drive
- Use file content as entropy source
- Generate Bitcoin keypair using proper BIP standards 
- Store encrypted private key and public address
```

4. OTP Security Implementation
```cpp
- Use W25Q128JV security registers
- When circuit breaks:
  - "Burn" OTP register to 0x00
  - This creates permanent record of circuit break
  - Cannot be reversed even if circuit is repaired
```

5. User Interface
```cpp
- LED Status:
  * Red = Circuit intact, wallet secure
  * Green = Circuit broken, private key exposed
  * Blue = Initializing
  * Yellow = Generating wallet
- USB Drive Content:
  * README explaining usage
  * Public address always visible
  * Private key visible only after circuit break
```

DEVELOPMENT SEQUENCE:

1. Start with working USB Mass Storage code
2. Add LED status indication
3. Implement circuit monitoring
4. Add basic wallet generation (test version)
5. Implement proper Bitcoin functionality
6. Add OTP security features
7. Test extensively for robustness

SECURITY CONSIDERATIONS:

1. Physical Security
```cpp
- Circuit must be difficult to repair undetectably
- OTP burning provides second layer of tamper evidence
```

2. Cryptographic Security
```cpp
- Proper entropy collection
- Secure key generation
- Encrypted storage while circuit intact
```

3. Usability Security
```cpp
- Clear status indication
- Unambiguous state (intact/broken)
- No way to accidentally expose private key
```

FILE STRUCTURE:
```
main.ino          - Main program entry
mint.h/cpp        - Core device functionality
mint_storage.h/cpp - USB and storage handling
mint_led.h/cpp    - LED status management
mint_wallet.h/cpp - Bitcoin wallet operations
```

TESTING APPROACH:
1. Test each component individually
2. Verify USB mounting and file operations
3. Confirm LED status accuracy
4. Validate wallet generation
5. Verify OTP burning effectiveness
6. Test tamper detection reliability

RECOMMENDED DEVELOPMENT STEPS:
1. Start with minimal working USB implementation
2. Add features incrementally
3. Test thoroughly after each addition
4. Only proceed when current step is fully functional

This provides a structured approach to implementing the device while maintaining security and usability. Each component can be developed and tested independently before integration.



:) 

Here's a glossary of key terms for the Mint Bitcoin Bearer Device project:

BEARER INSTRUMENT
- Traditional meaning: A document that entitles the holder to rights of ownership or specific payments (like bearer bonds or cash)
- In Mint context: A physical device that contains Bitcoin value, transferable by physically handing over the device

CIRCUIT INTEGRITY
- Traditional meaning: The proper functioning of an electronic circuit
- In Mint context: A physical trace on the PCB that, when intact, indicates the device hasn't been used/spent

OTP (One-Time Programmable)
- Traditional meaning: Memory that can be written once and never changed
- In Mint context: Security feature that permanently records when the device has been spent/circuit broken

ENTROPY
- Traditional meaning: Randomness or disorder in a system
- In Mint context: Random data (from user's dropped file) used to generate the Bitcoin private key

FAT12
- Traditional meaning: A simple file system format
- In Mint context: The file system implemented on the device's USB storage for compatibility

PRIVATE KEY
- Traditional meaning: Secret part of a cryptographic key pair
- In Mint context: The secret Bitcoin key that's encrypted while circuit is intact, revealed when broken

PUBLIC ADDRESS
- Traditional meaning: The publicly shareable part of a bitcoin wallet
- In Mint context: The Bitcoin address shown on the device's storage that can be used to verify/send funds

USB MASS STORAGE
- Traditional meaning: A device that appears as a standard storage drive when connected via USB
- In Mint context: How the device presents itself to computers for interaction

TAMPER EVIDENCE
- Traditional meaning: Physical indication that something has been altered
- In Mint context: Combination of broken circuit and burned OTP that proves the device has been spent

MINT
- Traditional meaning: A facility that produces money or coins
- In Mint context: The process of creating a new bearer device with a fresh Bitcoin wallet

BURNING
- Traditional meaning: Permanent marking or destruction
- In Mint context: The irreversible programming of the OTP memory when the circuit is broken

WALLET GENERATION
- Traditional meaning: Creating a new bitcoin wallet
- In Mint context: The process of locally creating a Bitcoin key pair when a file is dropped onto the device

![mintv2](https://github.com/user-attachments/assets/740382dc-a39b-42c2-aa2f-edde84606bc1)
![Mint_Top_PCB-removebg-preview](https://github.com/user-attachments/assets/1a48315d-c05a-4b4b-b64a-590d7c1bc9ab)
![Mint_bottom_PCB-removebg-preview](https://github.com/user-attachments/assets/baa96299-f7da-4c42-8c0a-035040abb5bf)
![Mint_Layers_PCB-removebg-preview](https://github.com/user-attachments/assets/23814ec1-a92e-40b9-8d8f-616952c8c3d6)
![Mint Component List](https://github.com/user-attachments/assets/69878a04-bdf7-4ddf-a492-b0ab1b8a6492)


The key generation process in the Mint device context:

BITCOIN KEY GENERATION:
1. Entropy Source:
- User drops any file onto the USB drive
- File contents are used as seed data
- Additional entropy added from hardware-specific sources (device ID, timestamps)

2. Key Derivation:
```
User File Data + Hardware Entropy
↓
SHA-256 Hash
↓
32-byte Private Key
↓
secp256k1 Elliptic Curve
↓
Public Key (33 bytes)
↓
Bitcoin Address
```

SECURITY STATES:

1. Circuit Intact:
```
Private Key: Encrypted in memory
Public Key: Visible in USB storage
Status: Ready for transfer/gifting
```

2. Circuit Broken:
```
Private Key: Decrypted and revealed
Public Key: Still visible
Status: Ready for wallet sweeping
```

KEY STORAGE:
- Private key stored in encrypted form while circuit intact
- Encryption key derived from device-specific parameters
- Public key/address always stored in plain text

SUGGESTED TEST IMPLEMENTATION:
```cpp
// Basic test version
void generateWallet(uint8_t* entropy, size_t size) {
// SHA256(entropy)
uint8_t private_key[32];
sha256(entropy, size, private_key);

// Generate public key (implement secp256k1)
uint8_t public_key[33];
generate_public_key(private_key, public_key);

// Generate Bitcoin address
char address[35];
generate_bitcoin_address(public_key, address);
}
```

FINAL IMPLEMENTATION NEEDS:
1. Proper Bitcoin key derivation (BIP standards)
2. Secure entropy mixing
3. Proper encryption for private key storage
4. Safe memory handling

