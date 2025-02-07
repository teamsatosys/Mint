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
![Mint back](https://github.com/user-attachments/assets/edf9ac32-770a-43b5-8e4c-3350719f92b8)![Mint front](https://github.com/user-attachments/assets/83d60661-bbfe-471f-9669-bb18b0b0bc70)
[Schematic_matrixlbc-PCB-Design_2025-02-01.pdf](https://github.com/user-attachments/files/18704674/Schematic_matrixlbc-PCB-Design_2025-02-01.pdf)






