#ifndef DES_H
#define DES_H

#include <vector>
#include <string>

#define KEY_LEN 64

class Des
{
public:
    int keyi[16][48],
        total[64],
        left[32],
        right[32],
        ck[28],
        dk[28],
        expansion[48],
        z[48],
        xor1[48],
        sub[32],
        p[32],
        xor2[32],
        temp[64],
        pc1[56],
        ip[64],
        inv[8][8];

    char final[1000];

    void IP();
    void PermChoice1(std::vector<int> &key);
    void PermChoice2();
    void Expansion();
    void inverse();
    void xor_two();
    void xor_oneE(int);
    void xor_oneD(int);
    void substitution();
    void permutation();
    void keygen(std::vector<int> &key);
    char * Encrypt(const char *, std::vector<int> &key);
    char * Decrypt(const char *, std::vector<int> &key);
};

void key_to_binary(std::vector<int> &, std::string&);

#endif //DES_H


