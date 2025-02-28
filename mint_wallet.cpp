#include "mint_wallet.h"
#include <string.h>

// Base58 character set
static const char BASE58_CHARS[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

// Network byte for Bitcoin mainnet private key (0x80)
static const uint8_t WIF_PREFIX = 0x80;

// SegWit version 0 prefix (P2WPKH)
static const uint8_t SEGWIT_V0_PREFIX = 0x00;
static const uint8_t SEGWIT_V0_PROGRAM_LENGTH = 0x14; // 20 bytes

MintWallet::MintWallet(MintSecure& secure_element) : 
    secure(secure_element),
    wallet_generated(false) {
    memset(bitcoin_address, 0, sizeof(bitcoin_address));
    memset(private_key_wif, 0, sizeof(private_key_wif));
}

bool MintWallet::begin() {
    // Initialize wallet state
    wallet_generated = secure.hasWallet();
    
    // If wallet exists, load the default address
    if (wallet_generated) {
        getPublicAddress(); // Use default path
    }
    
    return true;
}

bool MintWallet::generateFromEntropy(const uint8_t* entropy, size_t size) {
    // Verify parameters
    if (!entropy || (size != 16 && size != 24 && size != 32)) {
        return false;
    }
    
    // Generate wallet using secure element
    if (!secure.generateWalletFromEntropy(entropy, size)) {
        return false;
    }
    
    wallet_generated = true;
    
    // Generate default address
    getPublicAddress();
    
    return true;
}

String MintWallet::getPublicAddress(const char* path) {
    // Check if wallet is generated
    if (!wallet_generated) {
        return "No wallet generated";
    }
    
    // Derive address using secure element
    if (!secure.deriveAddress(path, bitcoin_address, sizeof(bitcoin_address))) {
        return "Address derivation failed";
    }
    
    return String(bitcoin_address);
}

String MintWallet::getPrivateKey() {
    // Check if the device is in tampered state
    if (!secure.isTampered()) {
        return "Error: Device not in tampered state";
    }
    
    // Check if wallet is generated
    if (!wallet_generated) {
        return "No wallet generated";
    }
    
    // Get raw private key from secure element
    uint8_t raw_key[32];
    if (!secure.revealPrivateKey(raw_key, sizeof(raw_key))) {
        return "Failed to retrieve private key";
    }
    
    // Convert to WIF format
    if (!rawKeyToWIF(raw_key)) {
        // Zero out the sensitive data before returning
        memset(raw_key, 0, sizeof(raw_key));
        return "WIF conversion failed";
    }
    
    // Zero out the sensitive data
    memset(raw_key, 0, sizeof(raw_key));
    
    return String(private_key_wif);
}

bool MintWallet::isGenerated() const {
    return wallet_generated;
}

bool MintWallet::rawKeyToWIF(const uint8_t* raw_key) {
    if (!raw_key) {
        return false;
    }
    
    // WIF format: [1-byte prefix][32-byte key][1-byte compressed flag][4-byte checksum]
    uint8_t wif_data[38];
    wif_data[0] = WIF_PREFIX;  // Bitcoin mainnet prefix
    memcpy(&wif_data[1], raw_key, 32);
    wif_data[33] = 0x01;  // Compressed public key flag
    
    // Calculate double SHA256 checksum
    uint8_t checksum[4];
    calculateChecksum(wif_data, 34, checksum);
    
    // Append checksum
    memcpy(&wif_data[34], checksum, 4);
    
    // Encode as Base58
    if (!base58Encode(wif_data, 38, private_key_wif, sizeof(private_key_wif))) {
        return false;
    }
    
    return true;
}

void MintWallet::calculateChecksum(const uint8_t* data, size_t len, uint8_t* output) {
    // In a real implementation, this would calculate double SHA256
    // For now, use secure element to calculate SHA256
    uint8_t hash1[32];
    secure.se050.calculateSHA256(data, len, hash1);
    secure.se050.calculateSHA256(hash1, 32, hash1);
    
    // First 4 bytes of hash are the checksum
    memcpy(output, hash1, 4);
}

bool MintWallet::base58Encode(const uint8_t* data, size_t len, char* str, size_t str_len) {
    // Maximum string length that can be encoded from len bytes is len*138/100+1
    // (see Bitcoin codebase for explanation)
    if (str_len < len * 138 / 100 + 1) {
        return false;
    }
    
    // Count leading zeros
    size_t zeros = 0;
    while (zeros < len && data[zeros] == 0) {
        zeros++;
    }
    
    // Allocate enough space for the base58 conversion
    // We need a temporary buffer for the conversion process
    uint8_t tmp_buffer[len * 2];
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    
    // Process each input byte
    size_t output_len = 0;
    for (size_t i = zeros; i < len; i++) {
        uint16_t carry = data[i];
        
        // Apply b58 to each byte
        for (size_t j = 0; j < output_len; j++) {
            carry += (uint16_t)tmp_buffer[j] * 256;
            tmp_buffer[j] = carry % 58;
            carry /= 58;
        }
        
        // Add new digits to the end
        while (carry > 0) {
            tmp_buffer[output_len++] = carry % 58;
            carry /= 58;
        }
    }
    
    // Build the output string
    size_t str_pos = 0;
    
    // Add '1' characters for leading zeros
    for (size_t i = 0; i < zeros; i++) {
        if (str_pos < str_len - 1) {
            str[str_pos++] = '1';
        }
    }
    
    // Add remaining characters in reverse order
    for (size_t i = output_len; i > 0; i--) {
        if (str_pos < str_len - 1) {
            str[str_pos++] = BASE58_CHARS[tmp_buffer[i - 1]];
        }
    }
    
    // Null terminate
    str[str_pos] = '\0';
    
    return true;
}