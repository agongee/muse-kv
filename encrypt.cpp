#ifndef ENCRYPT_CPP
#define ENCRYPT_CPP

#include<iostream>
#include<math.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include "encrypt.h"
#include "kvstore.h"

inline std::pair<int, int> encrypt::get_prime(int lowerbdd, int upperbdd)
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

inline int encrypt::get_encrypt_key(int P)
{
    while(true)
    {
        int e = rand() % P;
        if(GCD(e, P) == 1)
            return e;
    }
}

inline int encrypt::get_decrypt_key(int P, int e)
{
    for(int d = 1; d < P; d++)
    {
        uint64_t prod = (uint64_t)d * (uint64_t)e;
        if(prod % P == 1)
            return d;
    }
    return -1;
}

inline int encrypt::encrypt(int N, int e, uint8_t message)
{
    uint64_t res = 1;

    for(int i = 0; i < e; i++)
        res = (res * (uint64_t)message) % N;
    return res;
}

inline std::string encrypt::encrypt_string(int N, int e, std::string message)
{
    std::string res = "";
    for(int i = 0; i < message.length(); i++)
        res += std::to_string(encrypt::encrypt(N, e, (uint8_t)message[i])) + "\\";
    return res;
}

inline uint8_t encrypt::decrypt(int N, int d, int encrypted_message)
{
    uint64_t res = 1;

    for(int i = 0; i < d; i++)
        res = (res * (uint64_t)encrypted_message) % N;
    return res;
}

inline std::string encrypt::decrypt_string(int N, int d, std::string encrypted_message)
{
    std::string* encrypted_words;
    int len;
    kvstore::split(encrypted_message, '\\', encrypted_words, len);

    char* words = new char[len];
    for(int i = 0; i < len - 1; i++)
    {
        std::pair<bool, int> data_to_int = kvstore::strtoint(encrypted_words[i]);
        if(!data_to_int.first) return "THIS LINE IS CRACKED";
        words[i] = (char)encrypt::decrypt(N, d, data_to_int.second);
    }
    words[len - 1] = '\0';

    std::string res = std::string(words);
    delete[] encrypted_words;
    delete[] words;

    return res;
}

#endif // ENCRYPT_CPP
