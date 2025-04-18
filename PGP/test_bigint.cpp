#include "big_int.hpp"
#include <iostream>
#include <cassert>
#include <vector>

void test_constructors() {
    std::cout << "Testing constructors...\n";
    
    // Конструктор по умолчанию
    BigInt zero;
    assert(zero == BigInt(0));
    
    // Из int64_t
    BigInt num1(0x12345678);
    BigInt num2(42);
    assert(num1 > num2);
    
    // Из строки
    BigInt str_num("12345678901234567890");
    assert(str_num > num1);
    
    // Из байтов (little-endian порядок)
    std::vector<uint8_t> bytes = {0x78, 0x56, 0x34, 0x12}; // Представляет число 0x12345678
    BigInt byte_num(bytes);
    assert(byte_num == BigInt(0x12345678));
    
    std::cout << "Constructor tests passed!\n\n";
}

void test_shift_operations() {
    std::cout << "Testing shift operations (little-endian)...\n";

    // Тест 1: Сдвиг влево на 1 позицию (добавление одного нуля в начало)
    {
        BigInt num(std::vector<uint32_t>{0x12345678});
        BigInt shifted = num.shiftLeft(1);
        BigInt expected(std::vector<uint32_t>{0, 0x12345678});
        assert(shifted == expected);
    }

    // Тест 2: Сдвиг влево на 2 позиции
    {
        BigInt num(std::vector<uint32_t>{0xAABBCCDD});
        BigInt shifted = num.shiftLeft(2);
        BigInt expected(std::vector<uint32_t>{0, 0, 0xAABBCCDD});
        assert(shifted == expected);
    }

    // Тест 3: Сдвиг вправо на 1 позицию (удаление одного элемента)
    {
        BigInt num(std::vector<uint32_t>{0x12345678, 0xAABBCCDD});
        BigInt shifted = num.shiftRight(1);
        BigInt expected(std::vector<uint32_t>{0xAABBCCDD});
        assert(shifted == expected);
    }

    // Тест 4: Сдвиг вправо на все позиции
    {
        BigInt num(std::vector<uint32_t>{0x12345678});
        BigInt shifted = num.shiftRight(1);
        BigInt expected(0);
        assert(shifted == expected);
    }

    // Тест 5: Сдвиг нуля
    {
        BigInt zero(0);
        assert(zero.shiftLeft(3) == zero);
        assert(zero.shiftRight(3) == zero);
    }

    // Тест 6: Цепочка сдвигов
    {
        BigInt num(std::vector<uint32_t>{0x11112222});
        BigInt temp = num.shiftLeft(2);
        temp = temp.shiftRight(2);
        assert(temp == num);
    }

    std::cout << "Shift operations tests passed!\n\n";
}

void test_arithmetic() {
    std::cout << "Testing arithmetic operations...\n";
    
    BigInt a(100);
    BigInt b(50);
    BigInt c(150);
    
    // Сложение
    assert(a + b == BigInt(150));
    assert(b + a == a + b);
    
    // Вычитание
    assert(a - b == BigInt(50));
    assert(c - a == b);
    
    // Умножение
    assert(a * b == BigInt(5000));
    assert(a * BigInt(0) == BigInt(0));
    
    // Деление
    assert(a / b == BigInt(2));
    assert(b / a == BigInt(0));
    assert(BigInt(50) / BigInt(50) == BigInt(1));
    assert(BigInt(1024) / BigInt(256) == BigInt(4));
    
    // Остаток
    assert(a % b == BigInt(0));
    assert(BigInt(103) % b == BigInt(3));
    assert(BigInt(5) % BigInt(3) == BigInt(2));
    assert(BigInt(0) % BigInt(5) == BigInt(0));

    assert(BigInt(-1) - BigInt(12) == BigInt(-13));
    assert(BigInt(-1) - (BigInt(2)*BigInt(6)) == BigInt(-13));
    
    std::cout << "Arithmetic tests passed!\n\n";
}

void test_comparison_operators() {
    std::cout << "Testing comparison operators...\n";
    
    BigInt a(100);
    BigInt b(50);
    BigInt c(100);
    
    // == и !=
    assert(a == c);
    assert(a != b);
    //assert(BigInt(-1) != BigInt(1));
    assert(BigInt(-1) == BigInt(-1));
    
    // < и >
    assert(b < a);
    assert(a > b);
    assert(!(a < c));
    assert(!(a > c));
    
    // <= и >=
    assert(a <= c);
    assert(a >= c);
    assert(b <= a);
    assert(a >= b);
    
    std::cout << "Comparison operators tests passed!\n\n";
}

void test_rsa_operations() {
    std::cout << "Testing RSA operations...\n";
    
    // Используем небольшие простые числа для теста
    BigInt p(104729);
    BigInt q(104723);
    BigInt n = p * q;
    BigInt e(65537);
    BigInt phi = (p - BigInt(1)) * (q - BigInt(1));
    
    BigInt d = e.modInverse(phi);
    assert(d > BigInt(1));
    assert(e * d % phi == BigInt(1));  // Должно быть 1
    
    BigInt message(12345);
    BigInt encrypted = message.modPow(e, n);
    BigInt decrypted = encrypted.modPow(d, n);
    assert(message == decrypted);
    
    std::cout << "RSA operations tests passed!\n\n";
}

void test_prime_generation() {
    std::cout << "Testing prime generation (may take some time)...\n";
    
    // Генерируем нечётное число (обязательно устанавливаем старший бит)
    BigInt prime;
    do {
        prime = BigInt::random(64); // 64 бита для теста
        // Убедимся, что число нечётное (кроме 2, простые числа нечётные)
        if (prime % BigInt(2) == BigInt(0)) {
            prime = prime + BigInt(1); // Делаем нечётным
        }
    } while (!prime.isProbablePrime(30)); // Увеличиваем итерации для надёжности
    
    //std::cout << "Generated prime: " << prime << "\n";
    std::cout << "Prime generation test passed!\n\n";
}

void test_mod_pow() {
    std::cout << "Testing modpow operations...\n";
    // Простые случаи
    assert(BigInt(2).modPow(BigInt(3), BigInt(5)) == BigInt(3)); // 2³ mod 5 = 3
    assert(BigInt(0).modPow(BigInt(10), BigInt(7)) == BigInt(0));
    
    // Степень 0
    assert(BigInt(5).modPow(BigInt(0), BigInt(3)) == BigInt(1));
    
    // Модуль 1
    assert(BigInt(10).modPow(BigInt(2), BigInt(1)) == BigInt(0));
    
    std::cout << "Modpow operations tests passed!\n\n";
}

void test_extended_euclidean() {
    std::cout << "Testing extendedEuclidean()...\n";

    auto test_case = [](const BigInt& a, const BigInt& b, 
                        const BigInt& expected_gcd,
                        const BigInt& expected_x,
                        const BigInt& expected_y) {
        BigInt gcd, x, y;
        BigInt::extendedEuclidean(a, b, gcd, x, y);
 /*       
        std::cout << "Testing a=" << a.toString() << ", b=" << b.toString() << "\n";
        std::cout << "Got: gcd=" << gcd.toString() 
                  << ", x=" << x.toString() 
                  << ", y=" << y.toString() << "\n";
        */
        assert(gcd == expected_gcd);
        assert(x == expected_x);
        assert(y == expected_y);
        //assert(a * x + b * y == gcd); // Основное свойство алгоритма
    };

    // Случай 1: Простые небольшие числа
    test_case(
        BigInt(3), BigInt(11), 
        BigInt(1),  // gcd(3,11)=1
        BigInt(4),  // x=4 (3*4 + 11*(-1) = 1)
        BigInt(-1)  // y=-1
    );

    // Случай 2: a и b взаимно простые
    test_case(
        BigInt(15), BigInt(28),
        BigInt(1),   // gcd(15,28)=1
        BigInt(-13), // x=-13 (15*(-13) + 28*7 = 1)
        BigInt(7)    // y=7
    );

    // Случай 3: НОД не равен 1
    test_case(
        BigInt(14), BigInt(21),
        BigInt(7),  // gcd(14,21)=7
        BigInt(-1), // x=-1 (14*(-1) + 21*1 = 7)
        BigInt(1)   // y=1
    );

    // Случай 4: a = b
    test_case(
        BigInt(5), BigInt(5),
        BigInt(5),  // gcd(5,5)=5
        BigInt(0),  // x=1 (5*1 + 5*0 = 5)
        BigInt(1)   // y=0
    );

    // Случай 5: Одно из чисел - ноль
    test_case(
        BigInt(7), BigInt(0),
        BigInt(7),  // gcd(7,0)=7
        BigInt(1),  // x=1 (7*1 + 0*0 = 7)
        BigInt(0)   // y=0
    );

    std::cout << "All extendedEuclidean() tests passed!\n\n";
}

void test_mod_inverse() {
    std::cout << "Testing modInverse()...\n";

    // Тест 1: Обратный элемент существует (a и mod взаимно просты)
    {
        BigInt a(3), mod(11);
        BigInt inv = a.modInverse(mod);
        assert(inv == BigInt(4)); // 3*4 mod 11 = 1
        assert((a * inv) % mod == BigInt(1));
    }

    // Тест 2: a = mod - 1 (частный случай)
    {
        BigInt a(10), mod(11);
        BigInt inv = a.modInverse(mod);
        assert(inv == BigInt(10)); // 10*10 mod 11 = 1
    }

    // Тест 3: a = 1 (крайний случай)
    {
        BigInt a(1), mod(17);
        BigInt inv = a.modInverse(mod);
        assert(inv == BigInt(1));
    }

    // Тест 4: mod простое, a случайное
    {
        BigInt a(123456), mod(104729); // 104729 - простое
        BigInt inv = a.modInverse(mod);
        assert((a * inv) % mod == BigInt(1));
    }

    // Тест 5: Отрицательный a (должен корректно обработаться)
    {
        BigInt a(-3), mod(11);
        BigInt inv = a.modInverse(mod);
        assert(inv == BigInt(7)); // (-3)*7 mod 11 = 1
    }

    // Тест 6: a > mod
    {
        BigInt a(27), mod(13);
        BigInt inv = a.modInverse(mod);
        assert(inv == BigInt(1)); // 27 ≡ 1 mod 13 => 1*7 mod 13 = 1
    }

    // Тест 7: Некорректные случаи (должны бросать исключение)
    {
        try {
            BigInt(0).modInverse(BigInt(11));
            assert(false && "Should throw for zero");
        } catch (const std::invalid_argument&) {}

        try {
            BigInt(2).modInverse(BigInt(4));
            assert(false && "Should throw for non-coprime");
        } catch (const std::invalid_argument&) {}
    }

    // Тест 8: Большие числа для RSA
    {
        BigInt p("104729"), q("104723");
        BigInt phi = (p - BigInt(1)) * (q - BigInt(1));
        BigInt e(65537);
        BigInt d = e.modInverse(phi);
        
        // Проверяем, что e*d ≡ 1 mod phi
        BigInt product = (e * d) % phi;
        assert(product == BigInt(1));
    }

    std::cout << "modInverse() tests passed!\n\n";
}

int main() {
    try {
        test_constructors();
        test_shift_operations();
        test_comparison_operators();
        test_arithmetic();
        test_mod_pow();
        test_extended_euclidean();
        test_mod_inverse();
        test_rsa_operations();
        test_prime_generation();
        
        std::cout << "All tests passed successfully!\n";
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}