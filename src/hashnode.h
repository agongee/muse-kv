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
This is class of hashnode (hash structure's one node)
*/

#ifndef HASHNODE_H
#define HASHNODE_H

#include <iostream>
#include <string>
#include <cstring>
#include <time.h>
#include "kvstore.h"

namespace hashnode{
// list_version
class hashNode{
public:
    /* constructor & destructor */
    hashNode(kvstore::key key, kvstore::value value, uint8_t authr, uint8_t authw);
    ~hashNode();

    /* member functions */
    kvstore::key get_k();
    kvstore::value get_v();
    std::mutex* get_thrd();
    uint8_t get_authr();
    uint8_t get_authw();
    int get_ttl(time_t curr_time);

    void set_k(kvstore::key key);
    void set_v(kvstore::value value);
    void set_ttl(int time_ex);
    void set_authr(uint8_t authr);
    void set_authw(uint8_t authw);
    void print();

    bool is_expired(time_t curr_time);
    std::string data_decode_node(kvstore::value v);
    std::string get_kv_str();

    /* member variables */
    kvstore::kv* key_value;
    hashNode* next_ptr;
    hashNode* prev_ptr;
};

}
#include "hashnode.cpp"
#endif
