#define _CRT_SECURE_NO_WARNINGS

#include "des.hpp"
#include <string.h>
#include <iostream>
#include <random>
#include <stdexcept>

using namespace std;

void Des::gen_des_key(int key[64]) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1);

    for (int i = 0; i < 64; ++i) {
        key[i] = dis(gen);
    }
}

void Des::PermChoice1(const int key[64]) {
    const int pc1_table[56] = {
        57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
        10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
        63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
        14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4
    };

    for (int i = 0; i < 56; ++i) {
        pc1[i] = key[pc1_table[i] - 1];
    }
}

void Des::PermChoice2() {
    const int pc2_table[48] = {
        14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
        23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
        41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
        44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
    };

    int combined[56];
    memcpy(combined, ck, 28 * sizeof(int));
    memcpy(combined + 28, dk, 28 * sizeof(int));

    for (int i = 0; i < 48; ++i) {
        z[i] = combined[pc2_table[i] - 1];
    }
}

void Des::keygen(const int key[64]) {
    PermChoice1(key);
    memcpy(ck, pc1, 28 * sizeof(int));
    memcpy(dk, pc1 + 28, 28 * sizeof(int));

    for (int round = 0; round < 16; ++round) {
        int shifts = (round == 0 || round == 1 || round == 8 || round == 15) ? 1 : 2;

        while (shifts--) {
            int c0 = ck[0], d0 = dk[0];
            for (int i = 0; i < 27; ++i) {
                ck[i] = ck[i + 1];
                dk[i] = dk[i + 1];
            }
            ck[27] = c0;
            dk[27] = d0;
        }

        PermChoice2();
        memcpy(keyi[round], z, 48 * sizeof(int));
    }
}

void Des::IP() {
    const int ip_table[64] = {
        58, 50, 42, 34, 26, 18, 10, 2,
        60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6,
        64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1,
        59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5,
        63, 55, 47, 39, 31, 23, 15, 7
    };

    for (int i = 0; i < 64; ++i) {
        ip[i] = total[ip_table[i] - 1];
    }

    memcpy(left, ip, 32 * sizeof(int));
    memcpy(right, ip + 32, 32 * sizeof(int));
}

void Des::Expansion() {
    const int e_table[48] = {
        32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
        8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
        24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1
    };

    for (int i = 0; i < 48; ++i) {
        expansion[i] = right[e_table[i] - 1];
    }
}

void Des::xor_one(int round, bool encrypt) {
    const int* current_key = encrypt ? keyi[round] : keyi[15 - round];
    for (int i = 0; i < 48; ++i) {
        xor1[i] = expansion[i] ^ current_key[i];
    }
}

void Des::substitution() {
    const int s[8][4][16] = {
        { // S1
            {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
            {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
            {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
            {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
        },
        { // S2
            {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
            {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
            {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
            {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
        },
        { // S3
            {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
            {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
            {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
            {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
        },
        { // S4
            {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
            {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
            {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
            {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
        },
        { // S5
            {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
            {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
            {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
            {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
        },
        { // S6
            {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
            {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
            {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
            {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
        },
        { // S7
            {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
            {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
            {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
            {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
        },
        { // S8
            {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
            {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
            {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
            {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
        }
    };

    for (int i = 0; i < 8; i++) {
        int row = (xor1[i*6] << 1) + xor1[i*6 + 5];
        int col = (xor1[i*6 + 1] << 3) | (xor1[i*6 + 2] << 2) | (xor1[i*6 + 3] << 1) | xor1[i*6 + 4];
        int val = s[i][row][col];

        for (int j = 0; j < 4; j++) {
            sub[i*4 + j] = (val >> (3-j)) & 1;
        }
    }
}

void Des::permutation() {
    const int p_table[32] = {
        16,7,20,21,29,12,28,17,
        1,15,23,26,5,18,31,10,
        2,8,24,14,32,27,3,9,
        19,13,30,6,22,11,4,25
    };

    for (int i = 0; i < 32; ++i) {
        p[i] = sub[p_table[i] - 1];
    }
}

void Des::xor_two() {
    for (int i = 0; i < 32; ++i) {
        xor2[i] = left[i] ^ p[i];
    }
}

void Des::inverse() {
    const int ip_inv_table[64] = {
        40,8,48,16,56,24,64,32,
        39,7,47,15,55,23,63,31,
        38,6,46,14,54,22,62,30,
        37,5,45,13,53,21,61,29,
        36,4,44,12,52,20,60,28,
        35,3,43,11,51,19,59,27,
        34,2,42,10,50,18,58,26,
        33,1,41,9,49,17,57,25
    };

    for (int i = 0; i < 32; ++i) temp[i] = right[i];
    for (int i = 32; i < 64; ++i) temp[i] = left[i - 32];

    for (int i = 0; i < 64; ++i) {
        inv[i/8][i%8] = temp[ip_inv_table[i] - 1];
    }
}

string Des::DES_encrypt(const string& text, const int key[64]) {
    keygen(key);
    string result;
    size_t text_len = text.length();
    size_t blocks = (text_len + 7) / 8;

    for (size_t m = 0; m < blocks; ++m) {
        char block[8] = {0};
        size_t block_len = min<size_t>(8, text_len - m * 8);
        memcpy(block, text.c_str() + m * 8, block_len);

        if (block_len < 8) {
            uint8_t pad = 8 - block_len;
            memset(block + block_len, pad, pad);
        }

        memset(total, 0, sizeof(total));
        for (int i = 0; i < 8; ++i) {
            uint8_t ch = block[i];
            for (int j = 0; j < 8; ++j) {
                total[i*8 + j] = (ch >> (7-j)) & 1;
            }
        }

        IP();

        for (int round = 0; round < 16; ++round) {
            int right_copy[32];
            memcpy(right_copy, right, sizeof(right_copy));

            Expansion();
            xor_one(round, true);
            substitution();
            permutation();
            xor_two();

            memcpy(left, right_copy, sizeof(right_copy));
            memcpy(right, xor2, sizeof(xor2));
        }

        inverse();

        char encrypted_block[8] = {0};
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                encrypted_block[i] |= inv[i][j] << (7-j);
            }
        }
        result.append(encrypted_block, 8);
    }

    return result;
}

string Des::DES_decrypt(const string& text, const int key[64]) {
    if (text.length() % 8 != 0) {
        throw runtime_error("Invalid ciphertext length (must be multiple of 8)");
    }

    keygen(key);
    string result;
    size_t blocks = text.length() / 8;

    for (size_t m = 0; m < blocks; ++m) {
        memset(total, 0, sizeof(total));
        for (int i = 0; i < 8; ++i) {
            uint8_t ch = text[m*8 + i];
            for (int j = 0; j < 8; ++j) {
                total[i*8 + j] = (ch >> (7-j)) & 1;
            }
        }

        IP();

        for (int round = 0; round < 16; ++round) {
            int right_copy[32];
            memcpy(right_copy, right, sizeof(right_copy));

            Expansion();
            xor_one(round, false);
            substitution();
            permutation();
            xor_two();

            memcpy(left, right_copy, sizeof(right_copy));
            memcpy(right, xor2, sizeof(xor2));
        }

        inverse();

        char decrypted_block[8] = {0};
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                decrypted_block[i] |= inv[i][j] << (7-j);
            }
        }

        if (m == blocks - 1) {
            uint8_t pad = decrypted_block[7];
            if (pad > 0 && pad <= 8) {
                bool valid = true;
                for (int i = 8 - pad; i < 8; ++i) {
                    if (decrypted_block[i] != pad) {
                        valid = false;
                        break;
                    }
                }
                if (valid) {
                    result.append(decrypted_block, 8 - pad);
                    continue;
                }
            }
        }
        result.append(decrypted_block, 8);
    }

    return result;
}