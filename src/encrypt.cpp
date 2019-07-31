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

#ifndef ENCRYPT_CPP
#define ENCRYPT_CPP

#include<iostream>
#include<math.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include "encrypt.h"
#include "kvstore.h"

//get two distinct prime numbers
inline std::pair<int, int> encrypt_::get_prime(int lowerbdd, int upperbdd)
{
    int p, q;

    srand(time(NULL));
    while(true)
    {
        p = rand() % (upperbdd - lowerbdd) + lowerbdd;
        bool flag = true;

        for(int i = 2; i <= (int)(sqrt((double)p)); i++)
            if(p % i == 0)
            {
                flag = false;
                break;
            }

        if(flag)
            break;
    }
    while(true)
    {
        q = rand() % (upperbdd - lowerbdd) + lowerbdd;
        if(q == p)
            continue;
        bool flag = true;

        for(int i = 2; i < (int)(sqrt((double)q)); i++)
            if(q % i == 0)
            {
                flag = false;
                break;
            }

        if(flag)
            break;
    }

    return std::pair<int, int>(p, q);
}

//generate encrypt key from P (RSA Algorithm)
inline int encrypt_::get_encrypt_key(int P)
{
    while(true)
    {
        int e = rand() % P;
        if(GCD(e, P) == 1)
            return e;
    }
}

//generate decrypt key from P and e (RSA Algorithm)
inline int encrypt_::get_decrypt_key(int P, int e)
{
    for(int d = 1; d < P; d++)
    {
        uint64_t prod = (uint64_t)d * (uint64_t)e;
        if(prod % P == 1)
            return d;
    }
    return -1;
}

//encrypt 1byte information, and return encrypted message as integer type
int encrypt_::encrypt(int N, int e, uint8_t message)
{
    uint64_t res = 1;

    for(int i = 0; i < e; i++)
        res = (res * (uint64_t)message) % N;
    return res;
}

//encrypt string information, and return encrypted message
std::string encrypt_::encrypt_string(int N, int e, std::string message)
{
    std::string res = "";
    for(int i = 0; i < message.length(); i++)
        res += std::to_string(encrypt_::encrypt(N, e, (uint8_t)message[i])) + "\\";
    return res;
}

//return 1byte information by decrypting encrypted_message
inline uint8_t encrypt_::decrypt(int N, int d, int encrypted_message)
{
    uint64_t res = 1;

    for(int i = 0; i < d; i++)
        res = (res * (uint64_t)encrypted_message) % N;
    return res;
}

//return string information by decrypting encrypted_message
inline std::string encrypt_::decrypt_string(int N, int d, std::string encrypted_message)
{
    std::string* encrypted_words;
    int len;
    kvstore::split(encrypted_message, '\\', encrypted_words, len);

    char* words = new char[len];
    for(int i = 0; i < len - 1; i++)
    {
        std::pair<bool, int> data_to_int = kvstore::strtoint(encrypted_words[i]);
        if(!data_to_int.first) return "THIS LINE IS CRACKED";
        words[i] = (char)encrypt_::decrypt(N, d, data_to_int.second);
    }
    words[len - 1] = '\0';

    std::string res = std::string(words);
    delete[] encrypted_words;
    delete[] words;

    return res;
}

#endif // ENCRYPT_CPP
