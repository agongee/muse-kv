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

#ifndef HASHNODE_CPP
#define HASHNODE_CPP

#include "hashnode.h"

inline hashnode::hashNode::hashNode(kvstore::key key, kvstore::value value, uint8_t authr, uint8_t authw)
{
    key_value = new kvstore::kv(key,value,authr,authw);
    next_ptr = NULL;
    prev_ptr = NULL;
}


inline hashnode::hashNode::~hashNode(){
    delete key_value;
}

inline kvstore::key hashnode::hashNode::get_k(){
    return key_value->kvstore::kv::get_k();
}

inline kvstore::value hashnode::hashNode::get_v(){
    return key_value->kvstore::kv::get_v();
}

inline std::mutex* hashnode::hashNode::get_thrd(){
    return key_value->kvstore::kv::get_thrd();
}

inline int hashnode::hashNode::get_ttl(time_t curr_time){
  return key_value->kvstore::kv::get_ttl(curr_time);
}

inline uint8_t hashnode::hashNode::get_authr()
{
    return key_value->kvstore::kv::get_authr();
}

inline uint8_t hashnode::hashNode::get_authw()
{
    return key_value->kvstore::kv::get_authw();
}

inline void hashnode::hashNode::set_k(kvstore::key key){
    key_value->kvstore::kv::set_k(key);
}

inline void hashnode::hashNode::set_v(kvstore::value value){
    key_value -> kvstore::kv::set_v(value);
}

inline void hashnode::hashNode::set_ttl(int time_ex){
  key_value -> kvstore::kv::set_ttl(time_ex);
}

inline void hashnode::hashNode::set_authr(uint8_t authr)
{
    key_value->kvstore::kv::set_authr(authr);
}

inline void hashnode::hashNode::set_authw(uint8_t authw)
{
    key_value->kvstore::kv::set_authw(authw);
}


inline bool hashnode::hashNode::is_expired(time_t curr_time){
  return key_value -> kvstore::kv::is_expired(curr_time);
}

inline void hashnode::hashNode::print(){
  std::cout<<"(key : "<< key_value->kvstore::kv::get_k();
  std::cout<<", value: "<< *(std::string*)(key_value->kvstore::kv::get_v().data_addr) << ")";
}

inline std::string hashnode::hashNode::data_decode_node(kvstore::value v){
  switch(v.data_dtype){
    case kvstore::String: {
        return *((std::string*)(v.data_addr));
    }
    case kvstore::List: {
        linkedlist::List* data = (linkedlist::List*)(v.data_addr);

        std::string print = data->print();

        if(print != "") return print;
        else return "(empty)";
    }
    case kvstore::Set: {
        hashset::Hashset* data = (hashset::Hashset*)(v.data_addr);

        std::string print = data->print();

        if(print != "") return print;
        else return "(empty)";
    }
    case kvstore::Hashmap: {
        hashmap::Hashmap* h_map_ = (hashmap::Hashmap*)(v.data_addr);
        if(h_map_->is_empty()) return "(empty)";
        return h_map_->fvs();
    }
    default:{
        return "BLANK";
    }
  }
}

// return (int)dtype, authority_wirte, quthority_read, key, value
inline std::string hashnode::hashNode::get_kv_str(){
  kvstore::dtype v_type = key_value->kvstore::kv::get_v().data_dtype;
  std::string dtype;

  switch(v_type){
    case kvstore::String: { dtype = "0"; break; }
    case kvstore::List: { dtype = "1"; break; }
    case kvstore::Set: { dtype = "2"; break; }
    case kvstore::Hashmap: { dtype = "3"; break; }
  }

  std::string k = key_value->kvstore::kv::get_k();
  std::string v = hashnode::hashNode::data_decode_node(key_value->kvstore::kv::get_v());
  std::string w = std::to_string(key_value->kvstore::kv::get_authw());
  std::string r = std::to_string(key_value->kvstore::kv::get_authr());

  return dtype + "," + w + "," + r + "," + k + "," + v;
}

#endif
