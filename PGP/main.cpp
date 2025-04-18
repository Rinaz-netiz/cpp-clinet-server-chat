#include <iostream>
#include <limits>
#include "pgp.hpp"
#include "rsa.hpp"

using namespace std;

int main() 
{
    cout << "Generating RSA keys. Please wait...\n";

    RSA rsa;
    rsa.generateKeys(256); // Генерация 512-битных ключей (может занять 1-2 минуты)
    
    // Получаем ключи
    auto [e, n] = rsa.getPublicKey();
    auto [d, _] = rsa.getPrivateKey();

    cout << "Public Key (e, n): " << e.toString() << ", " << n.toString() << "\n";
    cout << "Private Key (d): " << d.toString() << "\n";

    cout << "\nEnter your message: ";
    string message;
    getline(std::cin, message);

    PGPpacket letter = PGP::PGP_encrypt(e, n, message);
    cout << "\nEncrypted text: " << letter.message << endl;

    cout << "\nClick ...\n";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string myletter = PGP::PGP_decrypt(d, n, letter);
    cout << "Decrypted text: " << myletter;

    return 0;
}