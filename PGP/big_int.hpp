#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <stdexcept>

class BigInt {
private:
    std::vector<uint32_t> digits;
    bool isNegative = false;
    void removeLeadingZeros();
    static BigInt karatsubaMultiply(const BigInt& a, const BigInt& b);
    std::pair<BigInt, BigInt> splitAt(size_t n) const;
    BigInt abs() const;

public:
    BigInt shiftLeft(size_t n) const;
    BigInt shiftRight(size_t n) const;
    static void extendedEuclidean(const BigInt& a, const BigInt& b, BigInt& gcd, BigInt& x, BigInt& y);
    
    BigInt();
    BigInt(int64_t value);
    BigInt(const std::string& str);
    BigInt(const std::vector<uint8_t>& bytes);
    BigInt(const int key[], size_t size);
    BigInt(const std::vector<uint32_t>& digits);
    BigInt& operator=(const BigInt& other);
    BigInt(const BigInt& other);

    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator*(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator%(const BigInt& other) const;
    BigInt operator-() const;
    
    bool operator<(const BigInt& other) const;
    bool operator==(const BigInt& other) const;
    bool operator>=(const BigInt& other) const;
    bool operator>(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;

    BigInt modPow(const BigInt& exp, const BigInt& mod) const;
    BigInt modInverse(const BigInt& mod) const;

    bool isProbablePrime(int iterations = 20) const;
    static BigInt random(int numBits);
    std::string toString() const;
    void toBits(int key[64]) const;
};