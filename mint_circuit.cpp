#include "mint_circuit.h"

// Circuit definitions
#define CIRCUIT_DEBOUNCE_MS 50  // Debounce time in milliseconds
#define CIRCUIT_INTACT_VALUE LOW // Circuit intact = LOW, broken = HIGH

MintCircuit::MintCircuit(uint8_t circuit_pin) : 
    pin(circuit_pin),
    current_state(true),       // Default to intact
    previous_state(true),
    state_changed(false),
    last_debounce_time(0),
    debounce_delay(CIRCUIT_DEBOUNCE_MS) {
}

bool MintCircuit::begin() {
    // Configure the circuit pin as input with pullup
    pinMode(pin, INPUT_PULLUP);
    
    // Initialize state
    current_state = readRawState();
    previous_state = current_state;
    
    return true;
}

bool MintCircuit::isIntact() {
    // Update state with debouncing
    bool raw_state = readRawState();
    
    // Reset debounce timer if state changed
    if (raw_state != current_state) {
        last_debounce_time = millis();
    }
    
    // Update current state if debounce period passed
    if ((millis() - last_debounce_time) > debounce_delay) {
        // Only update if state is stable
        if (raw_state != current_state) {
            previous_state = current_state;
            current_state = raw_state;
            state_changed = true;
        }
    }
    
    return current_state;
}

bool MintCircuit::hasStateChanged() {
    return state_changed;
}

void MintCircuit::acknowledgeStateChange() {
    state_changed = false;
}

bool MintCircuit::readRawState() {
    // Read the circuit pin
    // LOW = intact, HIGH = broken
    return (digitalRead(pin) == CIRCUIT_INTACT_VALUE);
}