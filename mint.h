// mint.h
#ifndef MINT_H
#define MINT_H

#include <Arduino.h>
#include "mint_storage.h"
#include "mint_led.h"
#include "mint_secure.h"
#include "mint_wallet.h"
#include "mint_circuit.h"

// Global definitions
#define CIRCUIT_PIN 14

/**
 * Main device class coordinating all subsystems.
 * Handles state management, circuit monitoring, and user interactions.
 */
class MintDevice {
public:
    /**
     * Device states as a state machine
     */
    typedef enum {
        MINT_STATE_INITIALIZING,     // Device booting up
        MINT_STATE_READY_NO_WALLET,  // Ready but no wallet generated yet
        MINT_STATE_GENERATING_WALLET, // Processing entropy and generating wallet
        MINT_STATE_READY_WITH_WALLET, // Wallet ready, device sealed
        MINT_STATE_TAMPERED          // Circuit broken, device reveals private key
    } MintState;

    /**
     * Constructor initializes all subsystems
     */
    MintDevice();
    
    /**
     * Initialize the device and all subsystems
     * @return true if initialization successful, false otherwise
     */
    bool begin();
    
    /**
     * Main loop function, handles state updates
     * Should be called frequently from Arduino loop()
     */
    void loop();
    
    /**
     * Get current device state
     * @return Current state enum
     */
    MintState getState() const;
    
    /**
     * Check if device has a generated wallet
     * @return true if wallet exists, false otherwise
     */
    bool hasWallet() const;
    
    /**
     * Get Bitcoin address for current wallet
     * @return String containing Bitcoin address or error message
     */
    String getPublicAddress();
    
    /**
     * Get private key if device is tampered
     * @return String containing private key in WIF format or error message
     */
    String getPrivateKey();
    
private:
    MintState device_state;          // Current device state
    MintSecure secure;               // Secure element interface
    MintStorage storage;             // USB mass storage
    MintLED led;                     // Status LED
    MintCircuit circuit;             // Tamper circuit monitor
    MintWallet wallet;               // Bitcoin wallet
    
    bool processing_file;            // Flag for file processing state
    
    // Buffer for entropy collection
    uint8_t entropy_buffer[32];      // Full 32 bytes for maximum entropy
    size_t entropy_collected;        // Amount collected so far
    
    /**
     * Update LED based on current device state
     */
    void updateLEDFromState();
    
    /**
     * Handle circuit break detection and state transition
     */
    void handleCircuitBreak();
    
    /**
     * Process a file to extract entropy for wallet generation
     * @param buffer File contents
     * @param buffer_size Size of buffer
     * @return true if processing successful, false otherwise
     */
    bool processNewEntropyFile(const uint8_t* buffer, size_t buffer_size);
    
    /**
     * Generate high-quality entropy from various sources
     * @param output_buffer Buffer to store entropy
     * @param buffer_size Size of buffer
     * @return true if sufficient entropy collected, false otherwise
     */
    bool generateSecureEntropy(uint8_t* output_buffer, size_t buffer_size);
    
    /**
     * Mix external entropy with hardware-generated entropy
     * @param external_data External data to mix
     * @param external_size Size of external data
     * @param output_buffer Buffer to store mixed entropy
     * @param buffer_size Size of output buffer
     * @return true if mixing successful, false otherwise
     */
    bool mixEntropySources(const uint8_t* external_data, size_t external_size,
                           uint8_t* output_buffer, size_t buffer_size);
};

#endif // MINT_H