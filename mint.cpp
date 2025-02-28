#include "mint.h"

// Additional hardware entropy sources
#define ANALOG_NOISE_PIN A0  // Analog pin for noise sampling
#define SAMPLE_COUNT 32      // Number of samples to collect

MintDevice::MintDevice() : 
    device_state(MINT_STATE_INITIALIZING),
    circuit(CIRCUIT_PIN),
    wallet(secure),
    processing_file(false),
    entropy_collected(0) {
    memset(entropy_buffer, 0, sizeof(entropy_buffer));
}

bool MintDevice::begin() {
    // Initialize all subsystems
    led.begin();
    led.setInitializing(); // Blue during initialization
    
    // Initialize secure element
    if (!secure.begin()) {
        // Failed to initialize secure element
        led.setError();
        return false;
    }
    
    // Initialize circuit monitoring
    if (!circuit.begin()) {
        // Failed to initialize circuit monitoring
        led.setError();
        return false;
    }
    
    // Initialize wallet
    if (!wallet.begin()) {
        // Non-critical failure
        // We'll continue without a wallet
    }
    
    // Initialize storage last
    if (!storage.begin()) {
        // Failed to initialize storage
        led.setError();
        return false;
    }
    
    // Register file changed callback
    storage.setFileChangedCallback([this](uint8_t* buffer, size_t size) {
        this->processNewEntropyFile(buffer, size);
    });
    
    // Determine initial state
    if (circuit.isIntact()) {
        // Circuit intact - normal operation
        if (wallet.isGenerated()) {
            device_state = MINT_STATE_READY_WITH_WALLET;
        } else {
            device_state = MINT_STATE_READY_NO_WALLET;
        }
    } else {
        // Circuit broken - tampered state
        device_state = MINT_STATE_TAMPERED;
        secure.recordPermanentTamperState();
    }
    
    // Update LED to match initial state
    updateLEDFromState();
    
    return true;
}

void MintDevice::loop() {
    // Check for circuit state changes
    if (!circuit.isIntact() && device_state != MINT_STATE_TAMPERED) {
        // Circuit just broken
        handleCircuitBreak();
    }
    
    // Handle storage operations
    storage.task();
    
    // Update LED if needed
    static unsigned long last_led_update = 0;
    if (millis() - last_led_update > 100) {
        updateLEDFromState();
        last_led_update = millis();
    }
    
    // Handle state-specific actions
    switch (device_state) {
        case MINT_STATE_GENERATING_WALLET:
            // Collecting entropy or processing wallet
            // This is handled by the file callback
            break;
            
        case MINT_STATE_TAMPERED:
            // In tampered state, ensure storage shows private key
            if (wallet.isGenerated()) {
                String private_key = wallet.getPrivateKey();
                
                // Update readme file with private key
                char readme[512];
                snprintf(readme, sizeof(readme),
                    "MINT DEVICE - TAMPERED STATE\n\n"
                    "This device has been opened and the private key is exposed.\n\n"
                    "Bitcoin Private Key (WIF format):\n%s\n\n"
                    "Bitcoin Address:\n%s\n\n"
                    "CAUTION: Anyone with access to the private key can spend the funds.",
                    private_key.c_str(), wallet.getPublicAddress().c_str());
                
                storage.updateReadmeFile(readme);
            }
            break;
            
        case MINT_STATE_READY_WITH_WALLET:
            // In sealed state, ensure storage shows only public address
            {
                String public_address = wallet.getPublicAddress();
                
                // Update readme with public address only
                char readme[512];
                snprintf(readme, sizeof(readme),
                    "MINT DEVICE - SEALED STATE\n\n"
                    "This device is securely sealed. To access the private key,\n"
                    "you must physically break the security circuit.\n\n"
                    "Bitcoin Address:\n%s\n\n"
                    "WARNING: Breaking the circuit is IRREVERSIBLE and will\n"
                    "permanently expose the private key.",
                    public_address.c_str());
                
                storage.updateReadmeFile(readme);
            }
            break;
            
        default:
            // Other states don't need special handling here
            break;
    }
}

bool MintDevice::processNewEntropyFile(const uint8_t* buffer, size_t buffer_size) {
    // Only process if we don't have a wallet and circuit is intact
    if (processing_file || device_state == MINT_STATE_TAMPERED || 
        device_state == MINT_STATE_READY_WITH_WALLET) {
        return false;
    }
    
    // Set processing flag and update state
    processing_file = true;
    device_state = MINT_STATE_GENERATING_WALLET;
    updateLEDFromState();
    
    // Generate secure entropy by mixing user-provided data with hardware entropy
    uint8_t final_entropy[32]; // 256 bits of entropy
    if (!mixEntropySources(buffer, buffer_size, final_entropy, sizeof(final_entropy))) {
        // Failed to generate secure entropy
        device_state = MINT_STATE_READY_NO_WALLET;
        processing_file = false;
        updateLEDFromState();
        return false;
    }
    
    // Generate wallet from entropy
    if (!wallet.generateFromEntropy(final_entropy, sizeof(final_entropy))) {
        // Failed to generate wallet
        device_state = MINT_STATE_READY_NO_WALLET;
        processing_file = false;
        updateLEDFromState();
        
        // Zero out sensitive data
        memset(final_entropy, 0, sizeof(final_entropy));
        return false;
    }
    
    // Zero out sensitive data
    memset(final_entropy, 0, sizeof(final_entropy));
    
    // Update state
    device_state = MINT_STATE_READY_WITH_WALLET;
    processing_file = false;
    updateLEDFromState();
    
    return true;
}

bool MintDevice::generateSecureEntropy(uint8_t* output_buffer, size_t buffer_size) {
    // Validate parameters
    if (!output_buffer || buffer_size < 32) {
        return false;
    }
    
    // Use secure element hardware TRNG
    return secure.generateEntropy(output_buffer, buffer_size);
}

bool MintDevice::mixEntropySources(const uint8_t* external_data, size_t external_size,
                                  uint8_t* output_buffer, size_t buffer_size) {
    // Validate parameters
    if (!external_data || external_size == 0 || !output_buffer || buffer_size < 32) {
        return false;
    }
    
    // Generate hardware entropy
    uint8_t hardware_entropy[32];
    if (!generateSecureEntropy(hardware_entropy, sizeof(hardware_entropy))) {
        return false;
    }
    
    // Mix hardware entropy with external data
    // In a real implementation, this would use HMAC or a similar mixing function
    // For now, use secure element to calculate SHA-256 of combined data
    
    // Create a buffer to hold hardware entropy and external data
    const size_t combined_size = sizeof(hardware_entropy) + external_size;
    uint8_t* combined_data = (uint8_t*)malloc(combined_size);
    
    if (!combined_data) {
        return false;
    }
    
    // Copy data to combined buffer
    memcpy(combined_data, hardware_entropy, sizeof(hardware_entropy));
    memcpy(combined_data + sizeof(hardware_entropy), external_data, external_size);
    
    // Hash combined data using secure element
    bool result = secure.se050.calculateSHA256(combined_data, combined_size, output_buffer);
    
    // Zero out and free the combined buffer
    memset(combined_data, 0, combined_size);
    free(combined_data);
    
    // Zero out hardware entropy
    memset(hardware_entropy, 0, sizeof(hardware_entropy));
    
    return result;
}

void MintDevice::handleCircuitBreak() {
    // Update state
    device_state = MINT_STATE_TAMPERED;
    
    // Record permanent tamper state in OTP memory
    secure.recordPermanentTamperState();
    
    // Update LED
    updateLEDFromState();
}

void MintDevice::updateLEDFromState() {
    switch (device_state) {
        case MINT_STATE_INITIALIZING:
            led.setInitializing(); // Blue
            break;
            
        case MINT_STATE_READY_NO_WALLET:
            led.setNoWallet(); // White
            break;
            
        case MINT_STATE_GENERATING_WALLET:
            led.setGeneratingWallet(); // Yellow
            break;
            
        case MINT_STATE_READY_WITH_WALLET:
            led.setSecure(); // Red
            break;
            
        case MINT_STATE_TAMPERED:
            led.setTampered(); // Green
            break;
            
        default:
            led.setError(); // Rapid flashing red
            break;
    }
}

MintDevice::MintState MintDevice::getState() const {
    return device_state;
}

bool MintDevice::hasWallet() const {
    return wallet.isGenerated();
}

String MintDevice::getPublicAddress() {
    return wallet.getPublicAddress();
}

String MintDevice::getPrivateKey() {
    // Only allow access to private key in tampered state
    if (device_state != MINT_STATE_TAMPERED) {
        return "Error: Device not in tampered state";
    }
    
    return wallet.getPrivateKey();
}