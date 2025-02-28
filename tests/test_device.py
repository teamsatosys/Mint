#!/usr/bin/env python3
"""
Mint Hardware Test Suite

This script performs automated testing of the Mint Bitcoin bearer device.
It tests USB connectivity, file operations, wallet generation, and circuit monitoring.

Usage:
    python3 test_device.py --port [PORT]

Requirements:
    - pyserial
    - pyusb
    - pytest
"""

import os
import sys
import time
import argparse
import unittest
import random
import string
import usb.core
import usb.util
import serial

# Mint device USB identifiers
MINT_VID = 0x239A  # Adafruit VID
MINT_PID = 0x8029  # RP2040 MSC PID

class MintDeviceTests(unittest.TestCase):
    """Test suite for Mint Bitcoin bearer device."""
    
    @classmethod
    def setUpClass(cls):
        """Set up test environment."""
        parser = argparse.ArgumentParser(description='Test Mint device')
        parser.add_argument('--port', type=str, required=True, help='Serial port for device')
        
        args = parser.parse_args()
        cls.serial_port = args.port
        
        # Open serial connection for device communication
        try:
            cls.ser = serial.Serial(cls.serial_port, 115200, timeout=2)
            print(f"Connected to device on {cls.serial_port}")
        except Exception as e:
            print(f"Failed to connect to device: {e}")
            sys.exit(1)
            
        # Find USB device
        cls.dev = usb.core.find(idVendor=MINT_VID, idProduct=MINT_PID)
        if cls.dev is None:
            print("Could not find Mint device via USB")
            sys.exit(1)
            
        print("Found Mint device via USB")
        
        # Wait for device to initialize
        time.sleep(2)
    
    @classmethod
    def tearDownClass(cls):
        """Clean up after tests."""
        if hasattr(cls, 'ser') and cls.ser:
            cls.ser.close()
    
    def test_01_device_initialization(self):
        """Test that device initializes properly."""
        # Send command to get device state
        self.ser.write(b"state\n")
        response = self.ser.readline().decode('utf-8').strip()
        
        # Verify device is in either READY_NO_WALLET or READY_WITH_WALLET state
        self.assertTrue("READY_NO_WALLET" in response or "READY_WITH_WALLET" in response,
                        f"Unexpected device state: {response}")
        
        print(f"Device state: {response}")
    
    def test_02_usb_mass_storage(self):
        """Test USB mass storage functionality."""
        # Verify device appears as USB drive
        # This is a simplified test - in a real test, we would mount the drive and check
        # filesystem details
        
        # Check USB configuration
        self.assertIsNotNone(self.dev.get_active_configuration(),
                            "Device has no active USB configuration")
        
        # Find MSC interface
        cfg = self.dev.get_active_configuration()
        intf = None
        
        for i in cfg:
            if i.bInterfaceClass == 8:  # Mass Storage Class
                intf = i
                break
        
        self.assertIsNotNone(intf, "No MSC interface found")
        print("USB Mass Storage interface detected")
    
    def test_03_led_functionality(self):
        """Test LED status indicator functionality."""
        # Test different LED states
        states = ["INITIALIZING", "READY_NO_WALLET", "GENERATING_WALLET", 
                 "READY_WITH_WALLET", "TAMPERED"]
        
        for state in states:
            # Send command to set LED state for testing
            self.ser.write(f"test_led {state}\n".encode())
            response = self.ser.readline().decode('utf-8').strip()
            
            self.assertTrue(response.startswith("LED set to"), 
                           f"Failed to set LED state {state}: {response}")
            
            # Allow time to visually verify LED state
            print(f"LED state: {state} - verify correct color")
            time.sleep(1)
        
        # Reset LED to current device state
        self.ser.write(b"reset_led\n")
        self.ser.readline()  # Discard response
    
    def test_04_wallet_generation(self):
        """Test wallet generation functionality."""
        # Check current wallet state
        self.ser.write(b"has_wallet\n")
        has_wallet = self.ser.readline().decode('utf-8').strip()
        
        if has_wallet == "true":
            print("Device already has a wallet")
            
            # Check public address
            self.ser.write(b"get_address\n")
            address = self.ser.readline().decode('utf-8').strip()
            
            self.assertTrue(address.startswith("bc1") or address.startswith("1") or address.startswith("3"),
                           f"Invalid Bitcoin address format: {address}")
            
            print(f"Bitcoin address: {address}")
        else:
            print("Generating new wallet")
            
            # Generate random entropy file
            entropy = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(256))
            
            # Send entropy to device (normally would be done via USB file)
            self.ser.write(f"generate_wallet {entropy}\n".encode())
            response = self.ser.readline().decode('utf-8').strip()
            
            self.assertEqual(response, "Wallet generated", 
                            f"Failed to generate wallet: {response}")
            
            # Check for address
            self.ser.write(b"get_address\n")
            address = self.ser.readline().decode('utf-8').strip()
            
            self.assertTrue(address.startswith("bc1") or address.startswith("1") or address.startswith("3"),
                           f"Invalid Bitcoin address format: {address}")
            
            print(f"Generated new Bitcoin address: {address}")
    
    def test_05_circuit_monitoring(self):
        """Test circuit monitoring (simulation only, no actual breakage)."""
        # Query current circuit state
        self.ser.write(b"circuit_intact\n")
        intact = self.ser.readline().decode('utf-8').strip()
        
        # Device should report intact in normal testing
        self.assertEqual(intact, "true", "Circuit reported as broken")
        
        # Simulate circuit break for testing (doesn't actually break OTP)
        self.ser.write(b"simulate_break\n")
        response = self.ser.readline().decode('utf-8').strip()
        
        self.assertEqual(response, "Circuit break simulated", 
                        f"Failed to simulate circuit break: {response}")
        
        # Verify simulated state
        self.ser.write(b"get_state\n")
        state = self.ser.readline().decode('utf-8').strip()
        
        self.assertEqual(state, "TAMPERED", 
                        f"Device not in TAMPERED state after simulated break: {state}")
        
        # Reset simulation
        self.ser.write(b"reset_simulation\n")
        self.ser.readline()  # Discard response
    
    def test_06_key_revelation(self):
        """Test private key revelation in tampered state."""
        # Simulate tampered state
        self.ser.write(b"simulate_break\n")
        self.ser.readline()  # Discard response
        
        # Request private key
        self.ser.write(b"get_private_key\n")
        private_key = self.ser.readline().decode('utf-8').strip()
        
        # Verify key format (WIF)
        # Private keys in WIF format start with 5, K, or L for mainnet
        self.assertTrue(private_key.startswith(('5', 'K', 'L')), 
                       f"Invalid private key format: {private_key}")
        
        print(f"Private key revealed correctly in simulated tampered state")
        
        # Reset simulation
        self.ser.write(b"reset_simulation\n")
        self.ser.readline()  # Discard response
    
    def test_07_security_boundaries(self):
        """Test security boundaries and validation."""
        # Test private key access in non-tampered state
        self.ser.write(b"reset_simulation\n")
        self.ser.readline()  # Discard response
        
        # Device should refuse to provide private key in intact state
        self.ser.write(b"get_private_key\n")
        response = self.ser.readline().decode('utf-8').strip()
        
        self.assertTrue("Error" in response, 
                       f"Device provided private key in intact state: {response}")
        
        print("Security boundary test passed: device refused to reveal key in intact state")


if __name__ == '__main__':
    unittest.main(argv=['first-arg-is-ignored'], exit=False)