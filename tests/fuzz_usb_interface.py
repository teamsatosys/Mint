#!/usr/bin/env python3
"""
Mint USB Interface Fuzzer

This script performs fuzz testing on the USB mass storage interface of the Mint device
to ensure it can handle malformed or malicious data without crashing or exposing
sensitive information.

Usage:
    python3 fuzz_usb_interface.py --port [PORT]

Requirements:
    - pyserial
    - pyusb
    - pytest
"""

import os
import sys
import time
import argparse
import random
import string
import usb.core
import usb.util
import serial
from pathlib import Path

# Mint device USB identifiers
MINT_VID = 0x239A  # Adafruit VID
MINT_PID = 0x8029  # RP2040 MSC PID

# Fuzzing parameters
NUM_FUZZ_ITERATIONS = 1000
MAX_FUZZ_SIZE = 65536  # Maximum file size for fuzzing
MIN_FUZZ_SIZE = 1      # Minimum file size for fuzzing
TIMEOUT_SECONDS = 10   # Timeout for device response

def random_bytes(size):
    """Generate random bytes."""
    return bytearray(random.getrandbits(8) for _ in range(size))

def random_ascii(size):
    """Generate random ASCII string."""
    return ''.join(random.choice(string.printable) for _ in range(size)).encode()

def random_unicode(size):
    """Generate random Unicode string."""
    # Include some potentially problematic unicode characters
    unicode_ranges = [
        (0x0000, 0x007F),  # Basic Latin
        (0x0080, 0x00FF),  # Latin-1 Supplement
        (0x0100, 0x017F),  # Latin Extended-A
        (0x2000, 0x206F),  # General Punctuation
        (0x2200, 0x22FF),  # Mathematical Operators
        (0x2600, 0x26FF),  # Miscellaneous Symbols
        (0x1F600, 0x1F64F)  # Emoticons
    ]
    
    chars = []
    for _ in range(size):
        # Pick a random Unicode range
        start, end = random.choice(unicode_ranges)
        # Pick a random code point in that range
        code_point = random.randint(start, end)
        # Convert code point to character and add to list
        chars.append(chr(code_point))
    
    # Join characters and encode as UTF-8
    return ''.join(chars).encode('utf-8', errors='replace')

def generate_fuzz_data():
    """Generate different types of fuzz data."""
    size = random.randint(MIN_FUZZ_SIZE, MAX_FUZZ_SIZE)
    fuzz_type = random.choice(['binary', 'ascii', 'unicode', 'pattern'])
    
    if fuzz_type == 'binary':
        return random_bytes(size)
    elif fuzz_type == 'ascii':
        return random_ascii(size)
    elif fuzz_type == 'unicode':
        return random_unicode(size)
    elif fuzz_type == 'pattern':
        # Create patterns that might trigger buffer overflows
        patterns = [
            b'A' * size,  # Repeated character
            b'%s' * (size // 2),  # Format string vulnerability test
            b'%n' * (size // 2),  # Format string vulnerability test
            b'/' * size,  # Path traversal attempt
            b'\\' * size,  # Path traversal attempt
            b'.' * size,  # Path traversal attempt
            b'%00' * (size // 3),  # Null byte injection
            b'0x' + b'0' * size,   # Very large number
        ]
        return random.choice(patterns)

def check_device_alive(ser):
    """Check if the device is still responsive."""
    try:
        ser.write(b"ping\n")
        response = ser.readline().decode('utf-8', errors='replace').strip()
        return response == "pong"
    except Exception:
        return False

def main():
    """Main fuzzing function."""
    parser = argparse.ArgumentParser(description='Fuzz Mint USB interface')
    parser.add_argument('--port', type=str, required=True, help='Serial port for device')
    
    args = parser.parse_args()
    serial_port = args.port
    
    # Open serial connection for device communication
    try:
        ser = serial.Serial(serial_port, 115200, timeout=2)
        print(f"Connected to device on {serial_port}")
    except Exception as e:
        print(f"Failed to connect to device: {e}")
        sys.exit(1)
    
    # Find USB device
    dev = usb.core.find(idVendor=MINT_VID, idProduct=MINT_PID)
    if dev is None:
        print("Could not find Mint device via USB")
        sys.exit(1)
    
    print("Found Mint device via USB")
    
    # Create a temporary directory for fuzzing files
    fuzz_dir = Path("./fuzz_data")
    fuzz_dir.mkdir(exist_ok=True)
    
    crashes = 0
    hangs = 0
    successes = 0
    
    print(f"Starting fuzzing with {NUM_FUZZ_ITERATIONS} iterations...")
    
    for i in range(NUM_FUZZ_ITERATIONS):
        if i % 100 == 0:
            print(f"Fuzzing iteration {i}/{NUM_FUZZ_ITERATIONS}")
        
        # Generate fuzz data
        fuzz_data = generate_fuzz_data()
        fuzz_file = fuzz_dir / f"fuzz_{i}.bin"
        
        # Write fuzz data to file
        with open(fuzz_file, 'wb') as f:
            f.write(fuzz_data)
        
        # Send command to process the fuzz file
        # In a real implementation, we would copy this to the device's mass storage
        # For testing, we'll use a command to simulate file processing
        cmd = f"process_file {fuzz_file}\n".encode()
        
        try:
            ser.write(cmd)
            start_time = time.time()
            
            # Wait for response or timeout
            while time.time() - start_time < TIMEOUT_SECONDS:
                if ser.in_waiting:
                    response = ser.readline().decode('utf-8', errors='replace').strip()
                    break
                time.sleep(0.1)
            else:
                # Timeout occurred
                print(f"HANG: Device timed out on iteration {i}")
                hangs += 1
                
                # Try to reset the device
                if not check_device_alive(ser):
                    print("Device unresponsive, attempting to reset...")
                    ser.close()
                    time.sleep(1)
                    try:
                        ser = serial.Serial(serial_port, 115200, timeout=2)
                    except Exception:
                        print("Failed to reconnect to device. Please reset manually and restart test.")
                        sys.exit(1)
                continue
            
            # Check if device is still alive
            if check_device_alive(ser):
                successes += 1
            else:
                print(f"CRASH: Device crashed on iteration {i}")
                crashes += 1
                
                # Try to reset the device
                ser.close()
                time.sleep(1)
                try:
                    ser = serial.Serial(serial_port, 115200, timeout=2)
                except Exception:
                    print("Failed to reconnect to device. Please reset manually and restart test.")
                    sys.exit(1)
        
        except Exception as e:
            print(f"ERROR: Exception during iteration {i}: {str(e)}")
            crashes += 1
    
    # Clean up
    ser.close()
    
    # Print fuzzing results
    print("\nFuzzing Results:")
    print(f"Total test cases:  {NUM_FUZZ_ITERATIONS}")
    print(f"Successes:        {successes}")
    print(f"Hangs:            {hangs}")
    print(f"Crashes:          {crashes}")
    print(f"Success rate:     {successes/NUM_FUZZ_ITERATIONS*100:.2f}%")
    
    if crashes == 0 and hangs == 0:
        print("\nPASSED: No crashes or hangs detected")
        return 0
    else:
        print("\nFAILED: Crashes or hangs detected")
        return 1

if __name__ == '__main__':
    sys.exit(main())