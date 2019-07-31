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
This is class of lowest level of kvstore
*/

#ifndef KVSTORE_CPP
#define KVSTORE_CPP

#include "kvstore.h"

inline kvstore::kv::~kv()
{
    switch(v.data_dtype)
    {
    case String:
    {
        delete (std::string *)v.data_addr;
        break;
    }
    case List:
    {
        delete (linkedlist::List *)v.data_addr;
        break;
    }
    case Set:
    {
        delete (hashset::Hashset *)v.data_addr;
        break;
    }
    case Hashmap:
    {
        delete (hashmap::Hashmap *)v.data_addr;
        break;
    }
    default:
    {
      std::cout << "ERROR!!!" << std::endl;
      break;
    }
    }
}

// split with token and save at string pointer splited
inline void kvstore::split(std::string str, char token, std::string *&splited, int& len)
{
    if(str == "")
    {
        len = 0;
        return;
    }

    len = 1;
    for(unsigned int i = 0; i < str.length(); i++)
        if(str.at(i) == token)
            len++;

    splited = new std::string[len];

    int index = 0;
    while(true)
    {
        std::string::size_type pos = str.find(token);
        if(pos == std::string::npos)
            break;

        splited[index++] = str.substr(0, pos);
        str.erase(0, pos + 1);
    }
    splited[index] = str;
}

// merge with token
inline void kvstore::merge(std::string& str, char token, std::string *splited, int len)
{
    if(len == 0)
        str = "";
    else
    {
        str = splited[0];
        for(int i = 1; i < len; i++)
        {
            str += token + splited[i];
        }
    }
}

// trim all spaces
inline std::string kvstore::trim(std::string str){
    int i = 0;
    if(str == "")
        return str;

    while(str[i] == ' ' && i < str.length())
        i++;
    str.erase(0, i);

    i = str.length() - 1;
    while(str[i] == ' ' && i >= 0)
        i--;
    str.erase(i + 1);

    for(i = 0; i < int(str.length() - 1); i++)
    {
        if(str[i] == ' ' && str[i + 1] == ' ')
        {
            str.erase(i, 1);
            i -= 1;
        }
    }

    return str;
}

// get int from string, and boolean value of is_int
inline std::pair<bool, int> kvstore::strtoint(std::string str){
    short sgn = 1;
    int i = 0, res = 0;

    if(str.at(0) == '-')
    {
        sgn = -1;
        i = 1;
    }

    for(; i < str.length(); i++)
    {
        if(str.at(i) < '0' || str.at(i) > '9') return std::pair<bool, int>(false, 0);
        res = res * 10 + (str.at(i) - '0');
    }
    return std::pair<bool, int>(true, sgn * res);
}

#endif
