#include "big_int.hpp"
#include <algorithm>
#include <random>
#include <iostream>
#include <cassert>

// === Constructors ===
BigInt::BigInt() : digits(1, 0) {}

BigInt::BigInt(int64_t value) {
    if (value == 0) {
        digits.push_back(0);
        return;
    }
    isNegative = value < 0;
    uint64_t absValue = isNegative ? -static_cast<uint64_t>(value) : value;
    while (absValue > 0) {
        digits.push_back(static_cast<uint32_t>(absValue & 0xFFFFFFFF));
        absValue >>= 32;
    }
}

BigInt::BigInt(const std::string& str) {
    if (str.empty()) {
        digits.push_back(0);
        return;
    }
    for (char c : str) {
        if (!isdigit(c)) {
            throw std::invalid_argument("BigInt string must contain only digits");
        }
    }
    BigInt result(0);
    for (char c : str) {
        result = result * BigInt(10) + BigInt(static_cast<int>(c - '0'));
    }
    *this = result;
}

BigInt::BigInt(const std::vector<uint8_t>& bytes) {
    if (bytes.empty()) {
        digits.push_back(0);
        return;
    }
    for (size_t i = 0; i < bytes.size(); i += 4) {
        uint32_t chunk = 0;
        for (size_t j = 0; j < 4 && (i + j) < bytes.size(); ++j) {
            chunk |= static_cast<uint32_t>(bytes[i + j]) << (8 * j);
        }
        digits.push_back(chunk);
    }
    removeLeadingZeros();
}

BigInt::BigInt(const int bits[64], size_t size) {
    digits.clear();
    uint32_t chunk = 0;
    int bitPos = 0;

    for (int i = 0; i < 64; ++i) {
        if (bits[i]) {
            chunk |= (1U << (bitPos % 32));
        }
        bitPos++;

        if (bitPos % 32 == 0) {
            digits.push_back(chunk);
            chunk = 0;
        }
    }

    if (bitPos % 32 != 0) {
        digits.push_back(chunk);
    }

    removeLeadingZeros();
}

BigInt::BigInt(const std::vector<uint32_t>& digits_array) : digits(digits_array) {
    removeLeadingZeros();
}

BigInt::BigInt(const BigInt& other) : digits(other.digits), isNegative(other.isNegative) {}

BigInt& BigInt::operator=(const BigInt& other) {
    if (this != &other) {
        digits = other.digits;
        isNegative = other.isNegative;
    }
    return *this;
}

// === Support ===

void BigInt::removeLeadingZeros() {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
    if (digits.empty()) digits.push_back(0);
}

BigInt BigInt::abs() const {
    BigInt result = *this;
    result.isNegative = false;
    return result;
}

BigInt BigInt::karatsubaMultiply(const BigInt& a, const BigInt& b) {
    // Гарантируем, что работаем с положительными числами
    BigInt abs_a = a.abs();
    BigInt abs_b = b.abs();

    // Базовый случай - используем обычное умножение для маленьких чисел
    if (abs_a.digits.size() <= 16 || abs_b.digits.size() <= 16) {
        BigInt result;
        result.digits.resize(abs_a.digits.size() + abs_b.digits.size(), 0);
        
        for (size_t i = 0; i < abs_a.digits.size(); ++i) {
            uint64_t carry = 0;
            for (size_t j = 0; j < abs_b.digits.size() || carry; ++j) {
                uint64_t product = result.digits[i + j] + 
                                 static_cast<uint64_t>(abs_a.digits[i]) * 
                                 (j < abs_b.digits.size() ? abs_b.digits[j] : 0) + 
                                 carry;
                result.digits[i + j] = static_cast<uint32_t>(product & 0xFFFFFFFF);
                carry = product >> 32;
            }
        }
        
        result.removeLeadingZeros();
        return result;
    }
    
    // Находим точку разбиения
    size_t m = std::max(abs_a.digits.size(), abs_b.digits.size()) / 2;
    
    // Разбиваем числа на части
    auto [a1, a0] = abs_a.splitAt(m);
    auto [b1, b0] = abs_b.splitAt(m);
    
    // Рекурсивные вычисления
    BigInt z0 = karatsubaMultiply(a0, b0);
    BigInt z2 = karatsubaMultiply(a1, b1);
    BigInt z1 = karatsubaMultiply(a0 + a1, b0 + b1) - z2 - z0;
    
    // Собираем результат
    return z2.shiftLeft(2*m) + z1.shiftLeft(m) + z0;
}

BigInt BigInt::shiftLeft(size_t n) const {
    if (digits.size() == 1 && digits[0] == 0) {
        return *this;  
    }
    
    BigInt result;
    result.digits.resize(digits.size() + n, 0);
    std::copy(digits.begin(), digits.end(), result.digits.begin() + n);
    return result;
}

BigInt BigInt::shiftRight(size_t n) const {
    if (n >= digits.size()) {
        return BigInt(0);
    }
    
    BigInt result;
    result.digits.assign(digits.begin() + n, digits.end());
    
    if (result.digits.empty()) {
        return BigInt(0);
    }
    
    result.removeLeadingZeros();
    return result;
}

std::pair<BigInt, BigInt> BigInt::splitAt(size_t n) const {
    BigInt high, low;
    
    size_t split_pos = std::min(n, digits.size());
    low.digits.assign(digits.begin(), digits.begin() + split_pos);
    
    if (split_pos < digits.size()) {
        high.digits.assign(digits.begin() + split_pos, digits.end());
    } else {
        high.digits.push_back(0);
    }
    
    return {high, low};
}

std::string BigInt::toString() const {
    if (digits.empty()) return "0";
    
    std::string result;
    if (isNegative) {
        result += "-";  
    }
    
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%08X", *it);  
        result += buf;
    }
    
    return result;
}

void BigInt::toBits(int bits[64]) const {
    std::fill_n(bits, 64, 0);

    for (size_t i = 0; i < digits.size(); ++i) {
        uint32_t chunk = digits[i];
        for (int j = 0; j < 32; ++j) {
            int bitIndex = i * 32 + j;
            if (bitIndex >= 64) break;

            bits[bitIndex] = (chunk >> j) & 1;
        }
    }
}

// === Сomparison ===

bool BigInt::operator<(const BigInt& other) const {
    if (isNegative && !other.isNegative) {
        return true;  
    }
    if (!isNegative && other.isNegative) {
        return false; 
    }
    
    // Если знаки одинаковые, сравниваем абсолютные значения
    const bool bothNegative = isNegative && other.isNegative;
    
    // Сначала сравниваем длину
    if (digits.size() != other.digits.size()) {
        return bothNegative ? (digits.size() > other.digits.size())
                           : (digits.size() < other.digits.size());
    }
    
    // Поразрядное сравнение (старшие разряды в конце вектора)
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        if (digits[i] != other.digits[i]) {
            return bothNegative ? (digits[i] > other.digits[i])
                               : (digits[i] < other.digits[i]);
        }
    }
    
    return false;
}

bool BigInt::operator==(const BigInt& other) const {
    if (isNegative != other.isNegative) {
        return false;
    }
    
    BigInt this_copy = *this;
    BigInt other_copy = other;
    this_copy.removeLeadingZeros();
    other_copy.removeLeadingZeros();
    
    if (this_copy.digits.size() != other_copy.digits.size()) {
        return false;
    }
    
    return std::equal(this_copy.digits.begin(), 
                     this_copy.digits.end(),
                     other_copy.digits.begin());
}

bool BigInt::operator>=(const BigInt& other) const {
    return !(*this < other);
}

bool BigInt::operator>(const BigInt& other) const {
    return !(*this < other || *this == other);
}

bool BigInt::operator<=(const BigInt& other) const {
    return (*this < other) || (*this == other);
}

bool BigInt::operator!=(const BigInt& other) const {
    return !(*this == other);
}

// === Arithmetic ===

BigInt BigInt::operator-() const {
    BigInt result = *this;
    if (result != BigInt(0)) {
        result.isNegative = !result.isNegative;
    }
    return result;
}

BigInt BigInt::operator+(const BigInt& other) const {
    if (isNegative != other.isNegative) {
        if (isNegative) {
            return other - (-(*this));  // -a + b = b - a
        } else {
            return *this - (-other);    // a + (-b) = a - b
        }
    }

    // Если знаки одинаковые, складываем модули и сохраняем знак
    BigInt result;
    result.digits.clear();
    result.isNegative = isNegative; 

    uint64_t carry = 0;
    size_t max_size = std::max(digits.size(), other.digits.size());

    for (size_t i = 0; i < max_size || carry; ++i) {
        uint64_t sum = carry;
        
        if (i < digits.size()) sum += digits[i];
        if (i < other.digits.size()) sum += other.digits[i];
        
        result.digits.push_back(static_cast<uint32_t>(sum & 0xFFFFFFFF));
        carry = sum >> 32;
    }

    result.removeLeadingZeros();
    return result;
}

BigInt BigInt::operator-(const BigInt& other) const {
    if (isNegative != other.isNegative) {
        return *this + (-other);
    }
    
    if (isNegative && other.isNegative) {
        return (-other) - (-(*this));
    }
    
    BigInt abs_this = this->abs();
    BigInt abs_other = other.abs();
    
    bool this_is_larger = (abs_this >= abs_other);
    
    const BigInt& larger = this_is_larger ? abs_this : abs_other;
    const BigInt& smaller = this_is_larger ? abs_other : abs_this;
    
    BigInt result;
    result.digits.clear();
    uint64_t borrow = 0;
    
    for (size_t i = 0; i < larger.digits.size(); ++i) {
        uint64_t larger_digit = larger.digits[i];
        uint64_t smaller_digit = (i < smaller.digits.size()) ? smaller.digits[i] : 0;
        
        uint64_t diff = larger_digit - smaller_digit - borrow;
        
        if (diff > larger_digit) {  
            diff += (1ULL << 32);
            borrow = 1;
        } else {
            borrow = 0;
        }
        
        result.digits.push_back(static_cast<uint32_t>(diff));
    }
    
    if (!this_is_larger) {
        result.isNegative = true;
    }
    
    result.removeLeadingZeros();
    return result;
}

BigInt BigInt::operator*(const BigInt& other) const {
    BigInt result = karatsubaMultiply(abs(), other.abs());
    
    result.isNegative = (isNegative != other.isNegative);
    
    if (result == BigInt(0)) {
        result.isNegative = false;
    }
    
    return result;
}

BigInt BigInt::operator%(const BigInt& modulus) const {
    if (modulus == BigInt(0)) {
        throw std::invalid_argument("Modulus cannot be zero");
    }

    // Вычисляем остаток от деления абсолютных значений
    BigInt abs_this = this->abs();
    BigInt abs_modulus = modulus.abs();

    // Базовые случаи для ускорения
    if (abs_this < abs_modulus) {
        return *this;  
    }
    if (abs_this == abs_modulus) {
        return BigInt(0);  
    }
    if (abs_modulus.digits.size() == 1 && abs_modulus.digits[0] == 1) {
        return BigInt(0);  
    }

    // Основная логика деления
    BigInt remainder;
    BigInt current;
    remainder.digits.resize(abs_this.digits.size(), 0);

    for (int i = static_cast<int>(abs_this.digits.size()) - 1; i >= 0; --i) {
        current = current * BigInt(0x100000000) + BigInt(abs_this.digits[i]);
        current.removeLeadingZeros();

        if (current >= abs_modulus) {
            uint32_t q = 0;
            BigInt product;
            
            uint32_t high = 0xFFFFFFFF;
            uint32_t low = 0;
            while (low <= high) {
                uint32_t mid = low + (high - low) / 2;
                product = abs_modulus * BigInt(static_cast<uint64_t>(mid));
                
                if (product <= current) {
                    q = mid;
                    low = mid + 1;
                } else {
                    high = mid - 1;
                }
            }
            
            remainder.digits[i] = q;
            current = current - abs_modulus * BigInt(static_cast<uint64_t>(q));
        }
    }

    if (this->isNegative && current != BigInt(0)) {
        current = -current;
    }

    current.removeLeadingZeros();
    return current;
}

BigInt BigInt::operator/(const BigInt& divisor) const {
    // Проверка особых случаев
    if (divisor == BigInt(0)) throw std::invalid_argument("Division by zero");
    if (*this < divisor) return BigInt(0);
    if (*this == divisor) return BigInt(1);
    if (divisor == BigInt(1)) return *this;

    BigInt dividend = this->abs();
    BigInt divisor_abs = divisor.abs();
    BigInt quotient;
    BigInt current;

    // Обрабатываем цифры от старших к младшим 
    for (int i = static_cast<int>(dividend.digits.size()) - 1; i >= 0; --i) {
        current.digits.insert(current.digits.begin(), dividend.digits[i]);
        current.removeLeadingZeros();

        uint32_t q = 0;
        if (current >= divisor_abs) {
            uint32_t low = 0;
            uint32_t high = std::numeric_limits<uint32_t>::max();
            
            while (low <= high) {
                uint32_t mid = low + (high - low) / 2;
                BigInt product = divisor_abs * BigInt(static_cast<uint64_t>(mid));
                
                if (product == current) {
                    q = mid;
                    break;
                }
                else if (product < current) {
                    q = mid;
                    low = mid + 1;
                }
                else {
                    high = mid - 1;
                }
            }
            
            current = current - divisor_abs * BigInt(static_cast<uint64_t>(q));
        }

        quotient.digits.insert(quotient.digits.begin(), q);
    }

    quotient.removeLeadingZeros();
    quotient.isNegative = (isNegative != divisor.isNegative);
    
    if (quotient.digits.empty()) {
        return BigInt(0);
    }
    
    return quotient;
}

// === For RSA ===

BigInt BigInt::modPow(const BigInt& exp, const BigInt& mod) const {
    if (mod == BigInt(1)) return BigInt(0);
    if (exp == BigInt(0)) return BigInt(1);
    if (*this == BigInt(0)) return BigInt(0);

    BigInt result(1);
    BigInt base = *this % mod;
    BigInt exponent = exp;

    while (exponent > BigInt(0)) {
        if (exponent.digits[0] & 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exponent = exponent / BigInt(2);
    }
    return result;
}

void BigInt::extendedEuclidean(const BigInt& a, const BigInt& b, BigInt& gcd, BigInt& x, BigInt& y) {
    BigInt s_prev(1), s_curr(0);
    BigInt t_prev(0), t_curr(1);
    BigInt r_prev = a.abs(), r_curr = b.abs();
    
    int iterations = 0;
    const int max_iterations = 1000; // Защита от бесконечного цикла

    while (r_curr != BigInt(0) && iterations++ < max_iterations) {
        BigInt quotient = r_prev / r_curr;
        
        BigInt r_next = r_prev - quotient * r_curr;
        r_prev = r_curr;
        r_curr = r_next;

        BigInt s_next = s_prev - quotient * s_curr;
        s_prev = s_curr;
        s_curr = s_next;

        BigInt t_next = t_prev - quotient * t_curr;
        t_prev = t_curr;
        t_curr = t_next;
    }

    if (iterations >= max_iterations) {
        std::cout << "Euclidian  iterations problem\n" ;
        throw std::runtime_error("Extended Euclidean algorithm exceeded maximum iterations");
    }

    gcd = r_prev;
    x = s_prev;
    y = t_prev;
}

BigInt BigInt::modInverse(const BigInt& mod) const {
    if (*this == BigInt(0)) throw std::invalid_argument("0 has no inverse");
    if (mod <= BigInt(1)) throw std::invalid_argument("Modulus must be > 1");

    // Нормализуем число к положительному виду
    BigInt a = *this % mod;
    if (a.isNegative) a = a + mod;

    BigInt gcd, x, y;
    extendedEuclidean(a, mod, gcd, x, y);

    if (gcd != BigInt(1)) {
        throw std::invalid_argument("Inverse does not exist (gcd != 1)");
    }

    x = x % mod;
    if (x < BigInt(0)) x = x + mod;
    
    return x;
}

bool BigInt::isProbablePrime(int iterations) const {
    // Сначала проверяем малые числа
    if (*this <= BigInt(4)) {
        return *this == BigInt(2) || *this == BigInt(3);
    }
    // Чётные числа > 2 не простые
    if (*this % BigInt(2) == BigInt(0)) {
        return false;
    }

    // Представим n-1 как d*2^s
    BigInt d = *this - BigInt(1);
    int s = 0;
    while (d % BigInt(2) == BigInt(0)) {
        d = d / BigInt(2);
        s++;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(2, 0xFFFFFFFF);

    for (int i = 0; i < iterations; i++) {
        BigInt a;
        BigInt range = *this - BigInt(3);
        if (range < BigInt(1)) {
            a = BigInt(2);
        } else {
            do {
                std::vector<uint32_t> random_digits;
                for (int j = 0; j < digits.size(); j++) {
                    random_digits.push_back(dist(gen));
                }
                a = BigInt(random_digits) % range + BigInt(2);
            } while (a >= *this - BigInt(1));
        }

        BigInt x = a.modPow(d, *this);
        if (x == BigInt(1) || x == *this - BigInt(1)) {
            continue;
        }

        bool composite = true;
        for (int j = 0; j < s; j++) {
            x = x.modPow(BigInt(2), *this);
            if (x == *this - BigInt(1)) {
                composite = false;
                break;
            }
            if (x == BigInt(1)) {
                break;
            }
        }

        if (composite) {
            return false;
        }
    }
    return true;
}

BigInt BigInt::random(int numBits) {
    if (numBits <= 0) {
        return BigInt(0);
    }

    int numWords = (numBits + 31) / 32;
    std::vector<uint32_t> random_digits(numWords);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);

    for (int i = 0; i < numWords; i++) {
        random_digits[i] = dist(gen);
    }

    int bitsInLastWord = numBits % 32;
    if (bitsInLastWord == 0) {
        bitsInLastWord = 32;  
    }

    uint32_t mask = (1ULL << bitsInLastWord) - 1;
    random_digits.back() &= mask;

    if (bitsInLastWord > 0) {
        random_digits.back() |= (1ULL << (bitsInLastWord - 1));
    }

    BigInt result(random_digits);
    result.removeLeadingZeros();

    if (result.digits.empty()) {
        throw std::runtime_error("Generated zero number unexpectedly");
    }

    return result;
}

