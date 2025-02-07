// mint_wallet.cpp
#include "mint_wallet.h"

MintWallet::MintWallet() : wallet_generated(false) {
    memset(private_key, 0, sizeof(private_key));
    memset(public_key, 0, sizeof(public_key));
}

void MintWallet::generateFromEntropy(uint8_t* entropy, size_t size) {
    // For testing: just convert entropy to hex string
    for(size_t i = 0; i < 32 && i < size; i++) {
        sprintf(&private_key[i*2], "%02x", entropy[i]);
    }
    // For testing: public key is just reversed private key
    for(int i = 0; i < 64; i++) {
        public_key[i] = private_key[63-i];
    }
    wallet_generated = true;
}

String MintWallet::getPublicAddress() {
    if(!wallet_generated) return "No wallet generated";
    return String("TEST_") + String(public_key);
}

String MintWallet::getPrivateKey() {
    if(!wallet_generated) return "No wallet generated";
    return String("TEST_") + String(private_key);
}

bool MintWallet::isGenerated() {
    return wallet_generated;
}