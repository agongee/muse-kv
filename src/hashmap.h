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
This is class file of value type hashmap
*/

#ifndef HASHMAP_H
#define HASHMAP_H

#include <iostream>
#include <string>
#include <cstring>
//#include "kvstore.h"

namespace hashmap{

  class hashmapNode{
    public:
      hashmapNode(std::string field, std::string value) { prev_ptr = NULL; next_ptr = NULL; this->field = field; this->value = value;}
      std::string get_f() { return this->field; }
      std::string get_v() { return this->value; }
      void change_v(std::string value) { this->value = value; }

      /* member variables */
      std::string field;
      std::string value;
      hashmapNode* prev_ptr;
      hashmapNode* next_ptr;
  };

  class Hashmap{
    private:
      hashmapNode** h_map;
      int hash_size = 97;
      int size;
    public:
      /* Constructor and Destructor */
      Hashmap();
      ~Hashmap();

      int hash_func(std::string field);
      hashmapNode* find_hashmapNode(std::string field, int* index);
      bool is_empty();

      std::string fields();
      std::string values();
      std::string fvs();
      std::string get_v(std::string field);

      void set_fv(std::string field, std::string value);
      void del_fv(std::string field);
  };

}

#include "hashmap.cpp"
#endif