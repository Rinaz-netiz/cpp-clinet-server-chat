#include "rsa.hpp"
#include <stdexcept>

BigInt RSA::generatePrime(int bits, int millerRabinTests) 
{
    if (bits < 2) throw std::invalid_argument("Prime bits must be at least 2");

    while (true) {
        // Генерируем число
        BigInt candidate = BigInt::random(bits);

        if (candidate % BigInt(2) == BigInt(0)) {
            candidate = candidate + BigInt(1); // Делаем нечётным
        }

        // Проверяем деление на малые простые (ускоряет проверку)
        const std::vector<uint32_t> smallPrimes = {3, 5, 7, 11, 13, 17, 19, 23};
        bool hasSmallFactor = false;
        for (uint32_t prime : smallPrimes) {
            if (candidate % BigInt(prime) == BigInt(0)) {
                hasSmallFactor = true;
                break;
            }
        }
        if (hasSmallFactor) continue;

        // Основная проверка
        if (candidate.isProbablePrime(millerRabinTests)) {
            return candidate;
        }
    }
}

void RSA::generateKeys(int keySize) 
{
    if (keySize < 64) throw std::invalid_argument("Key size must be at least 64 bits");

    while (true) {
        // Генерируем p и q
        p = generatePrime(keySize / 2, 10);
        q = generatePrime(keySize / 2, 10);

        n = p * q;
        phi = (p - BigInt(1)) * (q - BigInt(1));
        e = BigInt(65537);

        // Если e и phi(n) взаимно просты, выходим из цикла
        BigInt gcd, x, y;
        BigInt::extendedEuclidean(e, phi, gcd, x, y);
        if (gcd == BigInt(1)) {
            d = e.modInverse(phi);
            break;
        }
    }
}

std::pair<BigInt, BigInt> RSA::getPublicKey() const {
    return {e, n};
}

std::pair<BigInt, BigInt> RSA::getPrivateKey() const {
    return {d, n};
}

BigInt RSA::RSA_encrypt(const BigInt& message) const {
    if (message >= n) {
        throw std::invalid_argument("Message must be less than n");
    }
    return message.modPow(e, n);
}

BigInt RSA::RSA_decrypt(const BigInt& ciphertext) const {
    if (ciphertext >= n) {
        throw std::invalid_argument("Ciphertext must be less than n");
    }
    return ciphertext.modPow(d, n);
}

BigInt RSA::sign(const BigInt& message) const {
    return RSA_decrypt(message);
}

BigInt RSA::verify(const BigInt& signature) const {
    return RSA_encrypt(signature);
}