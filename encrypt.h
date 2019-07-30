#ifndef ENCRYPT_H
#define ENCRYPT_H

namespace encrypt
{
inline int GCD(int n1, int n2)
{
    if(n1 == 0)
        return n2;
    return GCD(n2%n1, n1);
}

std::pair<int, int> get_prime(int lowerbdd, int upperbdd);
int get_encrypt_key(int P);
int get_decrypt_key(int P, int e);
int encrypt(int N, int e, uint8_t message);
std::string encrypt_string(int N, int e, std::string message);
uint8_t decrypt(int N, int d, int message);
std::string decrypt_string(int N, int d, std::string encrypted_message);

inline void test()
{
    while(true)
    {
        std::pair<int, int>primes = get_prime(256, 1024);
        int p = primes.first;
        int q = primes.second;

        int N = p * q;
        int P = (p - 1) * (q - 1);

        int e = get_encrypt_key(P);
        int d = get_decrypt_key(P, e);

        if(d < 0)
            continue;

        std::cout << "prime numbers : " << p << ", " << q << std::endl;
        std::cout << "encrypt key : " << e << std::endl;
        std::cout << "decrypt key : " << d << std::endl;

        int num;
        std::cin >> num;
        int encrypted_message = encrypt(N, e, num);
        std::cout << encrypted_message << std::endl;
        std::cout << (int)decrypt(N, d, encrypted_message) << std::endl;
    }
}

}

#include "encrypt.cpp"

#endif
