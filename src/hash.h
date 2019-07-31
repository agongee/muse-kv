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
This is main class of hash structure
*/

#ifndef HASH_H
#define HASH_H

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <time.h>
#include <thread>
#include <mutex>
#include <fstream>
#include "kvstore.h"
#include "hashnode.h"
#include "encrypt.h"

#define TOTAL_SLOT_NUM 99

namespace hash{

  class Hash{
  private:
      /* Member variables */
      hashnode::hashNode** h;
      int* size;
      int TABLE_S;
      int port_num;

      int total_num = TOTAL_SLOT_NUM;
      int start_num;
      int end_num;

  public:
      /* constructor & destructor */
      Hash();
      Hash(int port);
      Hash(int start_n, int end_n, int port);
      ~Hash();

      void set_boundary(int start_n, int end_n);
      int hash_func(kvstore::key key);
      hashnode::hashNode* find_node(kvstore::key key, int* index);

      void set(kvstore::key key, kvstore::value value, uint8_t authr, uint8_t authw);
      kvstore::value get(kvstore::key key);
      void del(kvstore::key key);
      void change_v(kvstore::key key, kvstore::value value);

      kvstore::key* keys();
      void print();
      bool save();
      bool read(std::string pw);

      int get_size();
      int get_port();
      int get_start(){return start_num;}
      int get_end(){return end_num;}

      void ttl_expire_background();
  };

kvstore::value data_encode_node(std::string data, kvstore::dtype data_type);
}

#include "hash.cpp"
#endif
