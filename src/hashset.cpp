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
This class supports to use hashset as data type
*/

#ifndef HASHSET_CPP
#define HASHSET_CPP

#include "hashset.h"

//default constructor of the hashset
inline hashset::Hashset::Hashset()
{
    table = new linkedlist::List*[TABLE_S];
    for(int i = 0; i < TABLE_S; i++)
        table[i] = new linkedlist::List();
}

//deconstructor of the hashset
inline hashset::Hashset::~Hashset()
{
    for(int i = 0; i < TABLE_S; i++)
        delete table[i];
    delete[] table;
}

//hash function for mapping string to int
inline int hashset::Hashset::hash_func(std::string str)
{
    int p = 53;
    int hash_val = 0;

    for(unsigned int i = 0; i < str.length(); i++)
    {
        hash_val += (str[i] - 'a' + 1) * p;
        p *= p;
    }

    if((hash_val%TABLE_S) < 0)
        hash_val = hash_val % TABLE_S + TABLE_S;
    else
        hash_val = hash_val % TABLE_S;

    return hash_val;
}

//push content to hashset
inline void hashset::Hashset::push(std::string content)
{
    int index = hash_func(content);

    if(table[index]->find(content) < 0)
        table[index]->push_back(content);
}

//pop arbitrary element from hashset, and return the element
inline std::string hashset::Hashset::pop()
{
    int i;
    for(i = 0; i < TABLE_S; i++)
        if(table[i]->get_length() > 0)
            break;

    if(i == TABLE_S)
        return "";
    else
        return table[i]->pop_back();
}

//remove content from hashset, if not exist, return false
inline bool hashset::Hashset::del(std::string content)
{
    for(int i = 0; i < TABLE_S; i++)
    {
        int index = table[i]->find(content);
        if(index >= 0)
        {
            table[i]->del(index);
            return true;
        }
    }
    return false;
}

//return the length of the hashset
inline int hashset::Hashset::get_length()
{
    int res = 0;
    for(int i = 0; i < TABLE_S; i++)
        res += table[i]->get_length();

    return res;
}

//declare print format of the hashset
inline std::string hashset::Hashset::print()
{
    bool start = true;
    std::string str = "";

    for(int i = 0; i < TABLE_S; i++)
    {
        if(table[i]->is_empty())
        {
            continue;
        }

        if(start)
        {
            str += table[i]->print();
            start = false;
        }
        else
            str += "," + table[i]->print();
    }
    return str;
}

#endif
