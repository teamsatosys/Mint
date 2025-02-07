// mint_wallet.h
#ifndef MINT_WALLET_H
#define MINT_WALLET_H

#include <Arduino.h>

class MintWallet {
public:
    MintWallet();
    void generateFromEntropy(uint8_t* entropy, size_t size);
    String getPublicAddress();
    String getPrivateKey();
    bool isGenerated();
    
private:
    char private_key[65];  // 32 bytes in hex = 64 chars + null
    char public_key[65];   // For now, just store as hex strings
    bool wallet_generated;
};

#endif // MINT_WALLET_H