#ifndef MINT_SECURE_H
#define MINT_SECURE_H

#include <Arduino.h>
#include <Wire.h>
#include "SE05x.h" // SE050 Arduino library

/**
 * Class for handling secure operations with SE050 secure element.
 * Manages all cryptographic operations, tamper detection, and secure storage.
 */
class MintSecure {
public:
    MintSecure();
    
    /**
     * Initialize the secure element.
     * @return true if initialization successful, false otherwise
     */
    bool begin();
    
    /**
     * Generate entropy from hardware TRNG with health tests.
     * @param output Buffer to store entropy
     * @param length Number of bytes to generate
     * @return true if successful, false otherwise
     */
    bool generateEntropy(uint8_t* output, size_t length);
    
    /**
     * Generate a BIP32 master key from entropy.
     * Keys are generated and stored within the secure element.
     * @param entropy Buffer containing entropy
     * @param entropy_len Length of entropy buffer (16, 24, or 32 bytes)
     * @return true if successful, false otherwise
     */
    bool generateWalletFromEntropy(const uint8_t* entropy, size_t entropy_len);
    
    /**
     * Derive a BIP32 child key from the master key.
     * Derivation is performed within the secure element.
     * @param path BIP32 derivation path (e.g. "m/44'/0'/0'/0/0")
     * @param address Output buffer for Bitcoin address
     * @param address_len Length of address buffer
     * @return true if successful, false otherwise
     */
    bool deriveAddress(const char* path, char* address, size_t address_len);
    
    /**
     * Check if the tamper circuit is intact.
     * @return true if circuit intact, false if broken
     */
    bool isCircuitIntact();
    
    /**
     * Record permanent tamper state if circuit broken.
     * This operation is irreversible - it burns OTP memory.
     * @return true if state recorded successfully, false otherwise
     */
    bool recordPermanentTamperState();
    
    /**
     * Reveal private key if tamper state activated.
     * Only works if tamper circuit is broken and OTP has been burned.
     * @param key_out Buffer to store private key
     * @param key_len Length of key buffer (must be >= 32 bytes)
     * @return true if key revealed successfully, false otherwise
     */
    bool revealPrivateKey(uint8_t* key_out, size_t key_len);
    
    /**
     * Returns whether a wallet has been generated.
     * @return true if wallet exists, false otherwise
     */
    bool hasWallet() const;
    
    /**
     * Returns whether the device is in tampered state.
     * @return true if tampered (OTP burned), false otherwise
     */
    bool isTampered() const;

private:
    SE05x se050;
    bool wallet_generated;
    bool tampered_state;
    
    // Key handles for secure element
    uint32_t master_key_id;
    uint32_t otp_tamper_id;
    
    // Constant-time comparison for sensitive data
    bool secureCompare(const uint8_t* a, const uint8_t* b, size_t length);
    
    // Private helper methods for key operations
    bool createMasterKey(const uint8_t* seed);
    bool readOTPState();
    bool writeOTPState(bool tampered);
};

#endif // MINT_SECURE_H