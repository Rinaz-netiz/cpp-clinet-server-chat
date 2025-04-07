#pragma once

#include <string>
#include <cstdint>

class Des 
{
private:
    // --- Внутренние структуры данных ---
    int keyi[16][48];   // Ключи для каждого раунда
    int total[64];      // Входной блок данных
    int left[32];       // Левая половина блока
    int right[32];      // Правая половина блока
    int ck[28];         // Половина ключа (C)
    int dk[28];         // Половина ключа (D)
    int expansion[48];  // Расширенная правая половина
    int z[48];
    int xor1[48];       // Результат XOR после расширения
    int sub[32];        // Результат подстановки (S-блоки)
    int p[32];          // Результат перестановки P
    int xor2[32];       // Результат XOR левой половины
    int temp[64];       // Временный буфер
    int pc1[56];        // Ключ после PC-1
    int ip[64];         // Блок после начальной перестановки
    int inv[8][8];      // Блок после конечной перестановки

    // --- Приватные методы ---
    void IP();                          // Начальная перестановка
    void PermChoice1(const int key[]);  // Перестановка PC-1
    void PermChoice2();                 // Перестановка PC-2
    void Expansion();                   // Расширение E
    void inverse();                     // Конечная перестановка
    void xor_two();                     // XOR двух половин
    void xor_one(int round, bool encrypt);           // XOR для шифрования
    void substitution();                // Подстановка (S-блоки)
    void permutation();                 // Перестановка P
    void keygen(const int key[]);       // Генерация раундовых ключей

public:    
    void gen_des_key(int key[64]);
    
    std::string DES_encrypt(const std::string& text, const int key[64]);
    std::string DES_decrypt(const std::string& text, const int key[64]);
};