#ifndef MINT_CIRCUIT_H
#define MINT_CIRCUIT_H

#include <Arduino.h>

/**
 * Class for managing the tamper-evident circuit.
 * Handles circuit monitoring, state management, and debouncing.
 */
class MintCircuit {
public:
    /**
     * Constructor for the circuit monitor.
     * @param circuit_pin GPIO pin connected to tamper circuit
     */
    MintCircuit(uint8_t circuit_pin);
    
    /**
     * Initialize the circuit monitor.
     * @return true if initialization successful, false otherwise
     */
    bool begin();
    
    /**
     * Check if the circuit is intact (not broken).
     * Uses debouncing to avoid false detections.
     * @return true if circuit intact, false if broken
     */
    bool isIntact();
    
    /**
     * Check if circuit state has changed since last check.
     * Useful for detecting tampering events.
     * @return true if state changed, false otherwise
     */
    bool hasStateChanged();
    
    /**
     * Reset the state change detection.
     */
    void acknowledgeStateChange();
    
private:
    const uint8_t pin;           // GPIO pin connected to circuit
    bool current_state;          // Current debounced state
    bool previous_state;         // Previous debounced state
    bool state_changed;          // Flag for state change detection
    
    unsigned long last_debounce_time; // Last time the circuit was checked
    unsigned long debounce_delay;     // Debounce time in milliseconds
    
    // Read raw circuit state (without debouncing)
    bool readRawState();
};

#endif // MINT_CIRCUIT_H