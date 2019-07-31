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

#ifndef HASH_CPP
#define HASH_CPP

#include "hash.h"

inline hash::Hash::Hash(){
    TABLE_S = TOTAL_SLOT_NUM;
    start_num = 0;
    end_num = TOTAL_SLOT_NUM - 1;
    size = new int[TABLE_S];
    for(int i=0; i<TABLE_S; i++) size[i] = 0;
    h = new hashnode::hashNode*[TABLE_S];
    for(int i =0; i<TABLE_S; i++){
      h[i] = NULL;
    }
}

inline hash::Hash::Hash(int port){
    TABLE_S = TOTAL_SLOT_NUM;
    port_num = port;
    start_num = 0;
    end_num = TOTAL_SLOT_NUM - 1;
    size = new int[TABLE_S];
    for(int i=0; i<TABLE_S; i++) size[i] = 0;
    h = new hashnode::hashNode*[TABLE_S];
    for(int i =0; i<TABLE_S; i++){
      h[i] = NULL;
    }
}

inline hash::Hash::Hash(int start_n, int end_n, int port){
  TABLE_S = end_n - start_n + 1;
  port_num = port;
  start_num = start_n;
  end_num = end_n;
  size = 0;
  h = new hashnode::hashNode*[TABLE_S];
  for(int i =0; i<TABLE_S; i++){
    h[i] = NULL;
  }
}

inline hash::Hash::~Hash(){
    for(int i=0; i<TABLE_S; i++){
      hashnode::hashNode* node = h[i];
      while(node != NULL){
        hashnode::hashNode* trash = node;
        node = node->next_ptr;
        delete trash;
      }
    }
    delete[] h;
    delete size;
}

inline void hash::Hash::set_boundary(int start_n, int end_n){
  start_num = start_n;
  end_num = end_n;
}

// hash function
inline int hash::Hash::hash_func(kvstore::key key){
    int p = 53;
    int hash_val = 0;

    for(unsigned int i = 0; i < key.length(); i++){
        hash_val += (key[i] - 64) * p;
        p *= p;
    }

    //more calculations
    hash_val ^= hash_val << 3;
    hash_val += hash_val >> 5;
    hash_val ^= hash_val << 4;
    hash_val += hash_val >> 17;
    hash_val ^= hash_val << 25;
    hash_val += hash_val >> 6;

    if((hash_val%TABLE_S) < 0) hash_val = hash_val % TABLE_S + TABLE_S;
    else hash_val = hash_val % TABLE_S;

    return hash_val;
}

// Return such key's node and if there is no such node, return NULL
inline hashnode::hashNode* hash::Hash::find_node(kvstore::key key, int* index){
    *index = hash_func(key);
    hashnode::hashNode* node = h[*index];

    if(node == NULL) {return NULL;}
    else {
        while(node != NULL) {
            if(node->hashnode::hashNode::get_k() == key) return node;
            node = node->next_ptr;
        }
        return NULL;
    }
}

inline void hash::Hash::set(kvstore::key key, kvstore::value value, uint8_t authr, uint8_t authw)
{
    // just set new_node at the front of the proper index node, no HDR in list
    int index = 0;
    hashnode::hashNode* node = find_node(key, &index);

    if(node == NULL)
    {
        hashnode::hashNode* new_node = new hashnode::hashNode(key, value, authr, authw);
        std::mutex* mut = new_node->get_thrd();
        mut->lock();
        if(h[index] == NULL)
            h[index] = new_node;
        else
        {
            new_node->next_ptr = h[index];
            h[index]->prev_ptr = new_node;
            h[index] = new_node;
        }
        size[index]++;
        mut->unlock();
    }
    else
    {
        std::mutex* mut = node->get_thrd();
        mut->lock();
        node->hashnode::hashNode::set_v(value);
        node->hashnode::hashNode::set_authr(authr);
        node->hashnode::hashNode::set_authw(authw);
        mut->unlock();
    }
}

// get value of such key
inline kvstore::value hash::Hash::get(kvstore::key key){
    kvstore::value v;

    int index = 0;
    hashnode::hashNode* node = find_node(key, &index);

    if(node == NULL) return v;
    else return node->hashnode::hashNode::get_v();
}

// delete value of such key
inline void hash::Hash::del(kvstore::key key){
    int index = 0;
    hashnode::hashNode* node = find_node(key, &index);

    std::mutex* mut = node->get_thrd();
    mut->lock();
    if(node->next_ptr != NULL) node->next_ptr->prev_ptr = node->prev_ptr;
    if(node->prev_ptr == NULL) h[index] = node->next_ptr;
    else node->prev_ptr->next_ptr = node->next_ptr;
    delete node; size[index]--;
    mut->unlock();
}

// return all keys in string
inline kvstore::key* hash::Hash::keys()
{
    int size_all = 0;
    for(int i = 0; i<TABLE_S; i++) size_all += size[i];
    kvstore::key* result = new kvstore::key[size_all]; int index = 0;
    for(int i = 0; i<TABLE_S; i++)
    {
        hashnode::hashNode* node = h[i];
        while(node != NULL)
        {
            result[index++] = node->hashnode::hashNode::get_k();
            node = node->next_ptr;
        }
    }
    return result;
}

// print for debugging
inline void hash::Hash::print(){
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Size : " << size << std::endl;

    for(int i=0; i<TABLE_S; i++){
        std::cout << "index : " << i << std::endl;
        hashnode::hashNode* node = h[i];
        while(node != NULL) {
            node->print();
            std::cout << " -> ";
            node = node->next_ptr;
        }
        std::cout << std::endl;
    }

    std::cout << "========================================" << std::endl;
}

// save as port_num.csv with encryption
inline bool hash::Hash::save(){

    std::ofstream out(std::string(std::to_string(port_num) + ".csv"));

    if(!out.is_open()) return false;

    for(int i = 0; i < TABLE_S; i++){
        hashnode::hashNode* node = h[i];
        while(node != NULL){
            std::string str = node->get_kv_str();
            for(int i = 0; i < str.length(); i++)
                out << encrypt_::encrypt(1483513, 28969, (uint8_t)str[i]) << "\\";
            out << "\n";
            node = node->next_ptr;
        }
    }
    out.close();
    return true;
}

// read as port_num.csv with decryption
inline bool hash::Hash::read(std::string pw){
    std::string port = std::to_string(this->port_num);
    std::ifstream in(std::string(port + ".csv"));
    if(!in.is_open()) return false;

    std::pair<bool, int> pw_to_int = kvstore::strtoint(pw);
    if(!pw_to_int.first) return false;
    int password = pw_to_int.second;

    while(in){
        std::string line; in >> line;
        if(line == "") break;

        std::string* data_splited; int len;
        kvstore::split(line, '\\', data_splited, len);

        // decrypting
        char* decrypted_message = new char[len];
        for(int i = 0; i < len - 1; i++)
        {
            std::pair<bool, int> data_to_int = kvstore::strtoint(data_splited[i]);
            if(!data_to_int.first) return false;
            decrypted_message[i] = (char)encrypt_::decrypt(1483513, password, data_to_int.second);
        }
        decrypted_message[len - 1] = '\0';

        std::string message = std::string(decrypted_message);

        delete[] data_splited;
        delete[] decrypted_message;

        std::string* args;
        kvstore::split(message, ',', args, len);

        // check if the file is damaged - 1. need to be longer than 5
        if(len < 5) return false;

        // check if the file is damaged - 2. dtype and authorities need to be number
        std::pair<bool, int> a0_to_int = kvstore::strtoint(args[0]);
        std::pair<bool, int> a1_to_int = kvstore::strtoint(args[1]);
        std::pair<bool, int> a2_to_int = kvstore::strtoint(args[2]);

        if(!a0_to_int.first) return false;
        if(!a1_to_int.first) return false;
        if(!a2_to_int.first) return false;

        int dtype = a0_to_int.second; if(dtype < 0 && dtype > 3) return false;
        int authw = a1_to_int.second;
        int authr = a2_to_int.second;
        kvstore::key key = args[3];
        kvstore::value value;

        // encoding with proper dtypes
        if(dtype == 0)
        {
            value = hash::data_encode_node(args[4], kvstore::String);
        }
        else if(dtype == 1)
        {
            for(int i=5; i < len; i++) args[4] = args[4] + "," + args[i];
            value = hash::data_encode_node(args[4], kvstore::List);
        }
        else if(dtype == 2)
        {
            for(int i=5; i < len; i++) args[4] = args[4] + "," + args[i];
            value = hash::data_encode_node(args[4], kvstore::Set);
        }
        else if(dtype == 3)
        {
            for(int i=5; i < len; i++) args[4] = args[4] + "," + args[i];
            value = hash::data_encode_node(args[4], kvstore::Hashmap);
        }
        delete[] args;

        this->hash::Hash::set(key, value, authr, authw);
    }
    return true;
}

inline int hash::Hash::get_size(){
  int size_all = 0;
  for(int i = 0; i<TABLE_S; i++) size_all += size[i];
  return size_all;
}

inline int hash::Hash::get_port(){
  return port_num;
}

// delete random expired value
inline void hash::Hash::ttl_expire_background(){
  srand(time(NULL));
  while(1){
    int size_all = 0;
    for(int i = 0; i<TABLE_S; i++) size_all += size[i];

    if((size_all-1) <= 0) { sleep(10); continue; }

    int random = rand() % (size_all-1) + 1;

    for(int i =0; i<TABLE_S; i++){
      if((random - size[i]) > 0) random -= size[i];
      else{
        hashnode::hashNode* node = h[i];
        while(random > 1){
          node = node->next_ptr;
          random--;
        }
        time_t curr_time = time(NULL);
        if(node->is_expired(curr_time)) this->hash::Hash::del(node->get_k());
      }
    }
  }
}


inline kvstore::value hash::data_encode_node(std::string data, kvstore::dtype data_type)
{
    kvstore::value encoded;
    encoded.data_dtype = data_type;

    switch(data_type){

    case kvstore::String: {
        std::string *data_save = new std::string(data);

        encoded.data_addr = (uint64_t)data_save;
        return encoded;
    }
    case kvstore::List: {
      std::string *data_splited; int len;
      kvstore::split(data, ',', data_splited, len);

      linkedlist::List *data_save = new linkedlist::List();

      encoded.data_addr = (uint64_t)data_save;

      for(int i = 0; i < len; i++)
          data_save->push_back(data_splited[i]);

      delete[] data_splited;
      return encoded;
    }
    case kvstore::Set: {
      std::string *data_splited; int len;
      kvstore::split(data, ',', data_splited, len);

      hashset::Hashset *data_save = new hashset::Hashset();
      encoded.data_addr = (uint64_t)data_save;

      for(int i = 0; i < len; i++)
          data_save->push(data_splited[i]);

      delete[] data_splited;
      return encoded;
    }
    case kvstore::Hashmap: {

      std::string *data_splited; int len;
      kvstore::split(data, ',', data_splited, len);

      hashmap::Hashmap *data_save = new hashmap::Hashmap();
      encoded.data_addr = (uint64_t)data_save;

      for(int i=0; i<len/2; i++){
        data_save -> set_fv(data_splited[2*i], data_splited[2*i+1]);
      }

      delete[] data_splited;
      return encoded;
    }
    default: {
        return encoded;
    }
  }
}

#endif
