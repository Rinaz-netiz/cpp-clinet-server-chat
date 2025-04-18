#include <iostream>
#include "rsa.hpp"

int main() {
    RSA rsa;
    
    rsa.generateKeys(512); 
    
    // Получаем ключи
    auto [e, n] = rsa.getPublicKey();
    auto [d, _] = rsa.getPrivateKey();

    std::cout << "Public Key (e, n): " << e.toString() << ", " << n.toString() << "\n";
    std::cout << "Private Key (d): " << d.toString() << "\n";

    // Шифруем и расшифровываем
    BigInt message("123456789");
    BigInt ciphertext = rsa.RSA_encrypt(message);
    BigInt decrypted = rsa.RSA_decrypt(ciphertext);

    std::cout << "Original: " << message.toString() << "\n";
    std::cout << "Encrypted: " << ciphertext.toString() << "\n";
    std::cout << "Decrypted: " << decrypted.toString() << "\n";

    // Подпись и проверка
    BigInt signature = rsa.sign(message);
    BigInt verified = rsa.verify(signature);

    std::cout << "Signature: " << signature.toString() << "\n";
    std::cout << "Verified: " << verified.toString() << "\n";

    return 0;
}