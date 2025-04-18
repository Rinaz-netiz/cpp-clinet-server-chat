#pragma once

#include "big_int.hpp"
#include <utility>

class RSA {
private:
    BigInt p;       
    BigInt q;       
    BigInt n;     
    BigInt phi;     
    BigInt e;       
    BigInt d;       

    static BigInt generatePrime(int bits, int millerRabinTests = 20);

public:
    void generateKeys(int keySize = 1024);

    std::pair<BigInt, BigInt> getPublicKey() const;

    std::pair<BigInt, BigInt> getPrivateKey() const;

    BigInt RSA_encrypt(const BigInt& message) const;

    BigInt RSA_decrypt(const BigInt& ciphertext) const;

    BigInt sign(const BigInt& message) const;

    BigInt verify(const BigInt& signature) const;
};