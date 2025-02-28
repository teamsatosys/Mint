// mint_wallet.h
#ifndef MINT_WALLET_H
#define MINT_WALLET_H

#include <Arduino.h>
#include "mint_secure.h"

/**
 * Class for managing Bitcoin wallet operations.
 * Handles key generation, derivation, and address formatting.
 * Leverages the secure element for all cryptographic operations.
 */
class MintWallet {
public:
    /**
     * Constructor requires secure element for crypto operations.
     * @param secure_element Reference to MintSecure instance
     */
    MintWallet(MintSecure& secure_element);
    
    /**
     * Initialize the wallet subsystem.
     * @return true if initialization successful, false otherwise
     */
    bool begin();
    
    /**
     * Generate a new Bitcoin wallet from provided entropy.
     * @param entropy Buffer containing entropy source
     * @param size Size of entropy buffer (16, 24, or 32 bytes)
     * @return true if wallet generation successful, false otherwise
     */
    bool generateFromEntropy(const uint8_t* entropy, size_t size);
    
    /**
     * Gets the Bitcoin address for the current wallet.
     * @param path Optional derivation path (defaults to m/84'/0'/0'/0/0 for native segwit)
     * @return Bitcoin address string or error message if failed
     */
    String getPublicAddress(const char* path = "m/84'/0'/0'/0/0");
    
    /**
     * Gets the WIF-encoded private key if device is in tampered state.
     * Only accessible when tamper circuit is broken and OTP is burned.
     * @return WIF-encoded private key or error message if unavailable
     */
    String getPrivateKey();
    
    /**
     * Checks if the wallet has been generated.
     * @return true if wallet exists, false otherwise
     */
    bool isGenerated() const;
    
private:
    MintSecure& secure;
    bool wallet_generated;
    char bitcoin_address[128];
    char private_key_wif[128];
    
    /**
     * Convert a raw private key to WIF format.
     * @param raw_key Raw 32-byte private key
     * @return true if conversion successful, false otherwise
     */
    bool rawKeyToWIF(const uint8_t* raw_key);
    
    /**
     * Calculate Bitcoin address checksum.
     * @param data Data to calculate checksum for
     * @param len Length of data
     * @param output Output buffer for checksum (must be 4 bytes)
     */
    void calculateChecksum(const uint8_t* data, size_t len, uint8_t* output);
    
    /**
     * Encode data as Base58 string.
     * @param data Data to encode
     * @param len Length of data
     * @param str Output string buffer
     * @param str_len Size of output buffer
     * @return true if encoding successful, false otherwise
     */
    bool base58Encode(const uint8_t* data, size_t len, char* str, size_t str_len);
};

#endif // MINT_WALLET_H