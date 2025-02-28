# Contributing to Mint

Thank you for your interest in contributing to Mint, an open-source Bitcoin bearer device. This document provides guidelines for contributing to the project.

## Development Environment Setup

1. **Hardware Requirements:**
   - Raspberry Pi Pico (RP2040) board
   - SE050 secure element
   - NeoPixel LED
   - Tamper-evident circuit components (see schematics)

2. **Software Setup:**
   - Install Arduino IDE or arduino-cli
   - Install required libraries:
     ```
     arduino-cli lib install "Adafruit TinyUSB Library"
     arduino-cli lib install "Adafruit NeoPixel"
     arduino-cli lib install "SE050 Arduino"
     ```
   - Clone trezor-crypto for the BIP implementations:
     ```
     git clone https://github.com/trezor/trezor-crypto.git
     ```

## Building and Testing

```
# Build the firmware
arduino-cli compile --fqbn rp2040:rp2040:rpipico main.ino

# Upload to device
arduino-cli upload -p [PORT] --fqbn rp2040:rp2040:rpipico main.ino

# Monitor serial output
arduino-cli monitor -p [PORT]

# Run tests
python3 tests/test_device.py --port [PORT]

# Fuzz USB interface
python3 tests/fuzz_usb_interface.py --port [PORT]
```

## Code Style Guidelines

- **Naming Conventions:**
  - Classes: `CamelCase`
  - Methods: `camelCase`
  - Variables: `snake_case`
  - Constants: `UPPER_SNAKE_CASE`

- **Documentation:**
  - Use Doxygen-style comments for all public interfaces
  - Include `@param` and `@return` tags in function documentation

- **Security Practices:**
  - Mark sensitive variables as `volatile`
  - Use constant-time comparisons for sensitive operations
  - Zero memory after use for sensitive data
  - Never store private keys in MCU RAM unencrypted

## Pull Request Process

1. Fork the repository and create a feature branch
2. Implement your changes following the code style guidelines
3. Add tests for new functionality
4. Ensure all tests pass
5. Update documentation as needed
6. Submit a pull request with a clear description of changes

## Security Considerations

Security is paramount for Mint. When contributing, please:

1. Never disable or weaken security features
2. Follow cryptographic best practices
3. Report security vulnerabilities privately to maintainers
4. Always validate inputs from untrusted sources
5. Consider physical security aspects of the device

## License

By contributing to Mint, you agree that your contributions will be licensed under the project's license.

## Questions?

If you have questions about contributing, please open an issue or contact the maintainers.