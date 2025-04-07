#pragma once

#include "des.hpp"
#include "rsa.hpp"
#include <string>

struct PGPpacket {
    std::string message;  
    BigInt key;          
};

class PGP {
public:
    static PGPpacket PGP_encrypt(const BigInt& e, const BigInt& n, const std::string& message);

    static std::string PGP_decrypt(const BigInt& d, const BigInt& n, const PGPpacket& packet);
};