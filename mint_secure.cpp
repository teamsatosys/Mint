#include "mint_secure.h"
#include <string.h>

// OTP memory locations in SE050
#define OTP_TAMPER_LOCATION 0x7FFFF0

// Key identifiers in SE050
#define MASTER_KEY_ID 0x10000001
#define DERIVE_TEMP_ID 0x10000002

MintSecure::MintSecure() : 
    wallet_generated(false), 
    tampered_state(false),
    master_key_id(MASTER_KEY_ID),
    otp_tamper_id(OTP_TAMPER_LOCATION) {
}

bool MintSecure::begin() {
    // Initialize I2C for SE050 communication
    Wire.begin();
    
    // Initialize SE050
    if (!se050.begin()) {
        return false;
    }
    
    // Read the current tamper state from OTP memory
    tampered_state = readOTPState();
    
    // Check if we have a wallet already stored
    wallet_generated = se050.objectExists(master_key_id);
    
    return true;
}

bool MintSecure::generateEntropy(uint8_t* output, size_t length) {
    // Generate random bytes from SE050 hardware TRNG
    if (!se050.getRandomBytes(output, length)) {
        return false;
    }
    
    // Perform NIST health tests on entropy
    // This is a simplified placeholder - actual implementation would 
    // include full NIST SP 800-90B tests
    uint8_t ones = 0;
    for (size_t i = 0; i < length; i++) {
        uint8_t val = output[i];
        while (val) {
            ones += val & 1;
            val >>= 1;
        }
    }
    
    // Simple frequency test - should be approximately 50% ones
    float ones_ratio = (float)ones / (length * 8);
    if (ones_ratio < 0.45 || ones_ratio > 0.55) {
        return false; // Failed health check
    }
    
    return true;
}

bool MintSecure::generateWalletFromEntropy(const uint8_t* entropy, size_t entropy_len) {
    // Validate entropy length (must be 16, 24, or 32 bytes for BIP39)
    if (entropy_len != 16 && entropy_len != 24 && entropy_len != 32) {
        return false;
    }
    
    // Calculate SHA-256 of entropy to create seed
    uint8_t seed[32];
    if (!se050.calculateSHA256(entropy, entropy_len, seed)) {
        return false;
    }
    
    // Create master key from seed (inside SE050)
    if (!createMasterKey(seed)) {
        return false;
    }
    
    // Zero out sensitive data from stack
    memset(seed, 0, sizeof(seed));
    
    wallet_generated = true;
    return true;
}

bool MintSecure::createMasterKey(const uint8_t* seed) {
    // Delete existing key if present
    if (se050.objectExists(master_key_id)) {
        se050.deleteObject(master_key_id);
    }
    
    // Create secp256k1 key pair using seed as input
    return se050.createECKeyPair(master_key_id, SE05x_ECCurve_SECP256K1, seed, 32, true);
}

bool MintSecure::deriveAddress(const char* path, char* address, size_t address_len) {
    if (!wallet_generated || !address || address_len < 42) {
        return false;
    }
    
    // Parse derivation path
    // In production, this would implement full BIP32 path parsing
    // This is a simplified placeholder
    uint32_t child_indices[5] = {0}; // m/a/b/c/d/e
    size_t path_depth = 0;
    
    const char* ptr = path;
    if (*ptr == 'm') ptr += 2; // Skip "m/"
    
    while (*ptr && path_depth < 5) {
        bool hardened = false;
        uint32_t index = 0;
        
        // Parse number
        while (*ptr >= '0' && *ptr <= '9') {
            index = index * 10 + (*ptr - '0');
            ptr++;
        }
        
        // Check for hardened notation
        if (*ptr == '\'') {
            hardened = true;
            index |= 0x80000000; // Hardened bit
            ptr++;
        }
        
        child_indices[path_depth++] = index;
        
        // Skip slash
        if (*ptr == '/') ptr++;
    }
    
    // In production, this would perform proper BIP32 key derivation
    // within the secure element. For now, we'll generate a placeholder address.
    uint8_t public_key[65];
    if (!se050.getECCPublicKey(master_key_id, public_key, sizeof(public_key))) {
        return false;
    }
    
    // Format as a simplified Bitcoin address
    // In production, this would implement proper BIP32/BIP44 derivation and address formatting
    snprintf(address, address_len, "bc1q%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
             public_key[0], public_key[1], public_key[2], public_key[3], public_key[4],
             public_key[5], public_key[6], public_key[7], public_key[8], public_key[9]);
    
    return true;
}

bool MintSecure::isCircuitIntact() {
    // Read GPIO pin for tamper circuit
    // LOW = intact, HIGH = broken
    return digitalRead(CIRCUIT_PIN) == LOW;
}

bool MintSecure::recordPermanentTamperState() {
    // Only burn OTP if circuit actually broken and not already tampered
    if (isCircuitIntact() || tampered_state) {
        return false;
    }
    
    // Burn OTP memory in SE050 - this is irreversible
    if (!writeOTPState(true)) {
        return false;
    }
    
    tampered_state = true;
    return true;
}

bool MintSecure::readOTPState() {
    uint8_t otp_data[1] = {0xFF};
    
    // Read OTP data from SE050
    if (!se050.readMemory(otp_tamper_id, otp_data, sizeof(otp_data))) {
        // If read fails, assume not tampered for safety
        return false;
    }
    
    // 0x00 = tampered, 0xFF = not tampered
    return (otp_data[0] == 0x00);
}

bool MintSecure::writeOTPState(bool tampered) {
    uint8_t otp_data[1] = {tampered ? 0x00 : 0xFF};
    
    // Write OTP data to SE050
    return se050.writeOTPMemory(otp_tamper_id, otp_data, sizeof(otp_data));
}

bool MintSecure::revealPrivateKey(uint8_t* key_out, size_t key_len) {
    // Only allow key revealing if tampered and wallet exists
    if (!tampered_state || !wallet_generated || !key_out || key_len < 32) {
        return false;
    }
    
    // Obtain private key from SE050
    // In production, this would be properly implemented with additional
    // authentication and verification steps
    if (!se050.getECCPrivateKey(master_key_id, key_out, key_len)) {
        return false;
    }
    
    return true;
}

bool MintSecure::hasWallet() const {
    return wallet_generated;
}

bool MintSecure::isTampered() const {
    return tampered_state;
}

bool MintSecure::secureCompare(const uint8_t* a, const uint8_t* b, size_t length) {
    // Constant-time comparison to prevent timing attacks
    uint8_t result = 0;
    for (size_t i = 0; i < length; i++) {
        result |= a[i] ^ b[i];
    }
    return result == 0;
}