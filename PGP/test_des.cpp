#include "des.hpp"
#include <iostream>
#include <limits>

using namespace std;

int main() 
{
    Des des;
    int key[64] = {0};  

    des.gen_des_key(key);
    
    string message;
    cout << "Enter: ";
    getline(cin, message);
    
    string encrypted = des.DES_encrypt(message, key);
    cout << "\nEncrypted:";
    for (unsigned char c : encrypted) {
        printf("%02x ", c);  
    }
    cout << endl;
    
    cout << "\nClick ...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    string decrypted = des.DES_decrypt(encrypted, key);
    cout << "\nDecrypted: " << decrypted << endl;
    
    return 0;
}