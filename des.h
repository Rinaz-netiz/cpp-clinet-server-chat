#ifndef DES_H
#define DES_H

#include <vector>
#include <string>

#define KEY_LEN 64

namespace des {
    class Des {

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


        void keygen(std::vector<int> &);
        char * Encrypt(const char *, std::vector<int> &);
        char * Decrypt(const char *, std::vector<int> &);
    };

    void key_to_binary(std::vector<int> &, std::string&);
}

#endif //DES_H
