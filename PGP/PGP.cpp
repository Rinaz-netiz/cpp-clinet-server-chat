#include "pgp.hpp"
#include <stdexcept>

PGPpacket PGP::PGP_encrypt(const BigInt& e, const BigInt& n, const std::string& message) 
{
    int desKey[64];
    Des des;
    des.gen_des_key(desKey);
    
    std::string ciphertext = des.DES_encrypt(message, desKey);

    BigInt desKeyBigInt(desKey, 64); 
    BigInt encryptedKey = desKeyBigInt.modPow(e, n);

    return {ciphertext, encryptedKey};
}

std::string PGP::PGP_decrypt(const BigInt& d, const BigInt& n, const PGPpacket& packet) 
{
    BigInt desKeyBigInt = packet.key.modPow(d, n);

    int desKey[64];
    desKeyBigInt.toBits(desKey);
    
    Des des;
    return des.DES_decrypt(packet.message, desKey);
}