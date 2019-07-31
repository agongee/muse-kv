/*
MIT License
Copyright (c) 2019 Taehun Kang(agongee123@gmail.com), Jaewoo Pyo(jwpyo98@gmail.com) and Bogyeong Park(parkbo0201@gmail.com)
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
This file helps to encrypt and decrypt message
*/

#ifndef ENCRYPT_H
#define ENCRYPT_H

namespace encrypt_
{
//calculate GCD of n1 and n2 by euclidean method
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

//generating encrypt key and decrypt key by using RSA Algorithm
inline void key_generator()
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
