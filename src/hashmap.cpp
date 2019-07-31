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

#ifndef HASHMAP_CPP
#define HASHMAP_CPP

#include "hashmap.h"

inline hashmap::Hashmap::Hashmap(){
  size = 0;
  h_map = new hashmap::hashmapNode*[hash_size];
  for(int i =0; i<hash_size; i++){
    h_map[i] = NULL;
  }
}

inline hashmap::Hashmap::~Hashmap(){
  for(int i=0; i<hash_size; i++){
    hashmap::hashmapNode* node = h_map[i];
    while(node != NULL){
      hashmap::hashmapNode* trash = node;
      node = node->next_ptr;
      delete trash;
    }
  }
  delete[] h_map;
}

// hash function *p multiplicator
inline int hashmap::Hashmap::hash_func(std::string field){
  int p = 53;
  int hash_val = 0;

  for(unsigned int i = 0; i < field.length(); i++){
      hash_val += (field[i] - 64) * p;
      p *= p;
  }

  if((hash_val%hash_size) < 0) hash_val = hash_val % hash_size + hash_size;
  else hash_val = hash_val % hash_size;

  return hash_val;
}

// return that field's node pointer, and return NULL if there is no such field
inline hashmap::hashmapNode* hashmap::Hashmap::find_hashmapNode(std::string field, int* index){
  for(int i=0; i<hash_size; i++){
      hashmapNode* check = h_map[i];
      while(check != NULL){
          if(check->get_f() == field) { *index = i; return check; }
          check = check->next_ptr;
      }
  }
  return NULL;
}

inline bool hashmap::Hashmap::is_empty(){
  if(size == 0) return true;
  else return false;
}

// return all fields of key
inline std::string hashmap::Hashmap::fields(){
  std::string result = std::string();
  for(int i=0; i<hash_size; i++){
      hashmap::hashmapNode* node = h_map[i];
      if(node != NULL) { result += node->hashmap::hashmapNode::get_f(); node = node->next_ptr; }
      while(node != NULL) {
          result += "," + node->hashmap::hashmapNode::get_f();
          node = node->next_ptr;
      }
  }
  return result;
}

// return all values of key
inline std::string hashmap::Hashmap::values(){
  std::string result = std::string();
  for(int i=0; i<hash_size; i++){
      hashmap::hashmapNode* node = h_map[i];
      if(node != NULL) { result += node->hashmap::hashmapNode::get_v(); node = node->next_ptr; }
      while(node != NULL) {
          result += "," + node->hashmap::hashmapNode::get_v();
          node = node->next_ptr;
      }
  }
  return result;
}

// return value of field
inline std::string hashmap::Hashmap::get_v(std::string field){
  int index = 0;
  hashmap::hashmapNode* map_node = hashmap::Hashmap::find_hashmapNode(field,&index);
  if(map_node == NULL) return "NULL";
  else return map_node -> get_v();
}

// return (field,value)s
inline std::string hashmap::Hashmap::fvs(){
  std::string result = std::string();
  for(int i=0; i<hash_size; i++){
      hashmap::hashmapNode* node = h_map[i];
      if(node != NULL) { result += node->hashmap::hashmapNode::get_f() + "," + node->hashmap::hashmapNode::get_v(); node = node->next_ptr; }
      while(node != NULL) {
          result += "," + node->hashmap::hashmapNode::get_f() + "," + node->hashmap::hashmapNode::get_v();
          node = node->next_ptr;
      }
  }
  return result;
}

inline void hashmap::Hashmap::set_fv(std::string field, std::string value){
  int index = 0;
  hashmap::hashmapNode* node = find_hashmapNode(field, &index);

  if(node == NULL){
      hashmap::hashmapNode* new_node = new hashmap::hashmapNode(field, value);
      if(h_map[index] == NULL) h_map[index] = new_node;
      else {
          new_node->next_ptr = h_map[index];
          h_map[index]->prev_ptr = new_node;
          h_map[index] = new_node;
      }
      size++;
  }
  else node->hashmap::hashmapNode::change_v(value);
}

inline void hashmap::Hashmap::del_fv(std::string field){
  int index = 0;
  hashmapNode* node = find_hashmapNode(field, &index);

  if(node->next_ptr != NULL) node->next_ptr->prev_ptr = node->prev_ptr;
  if(node->prev_ptr == NULL) h_map[index] = node->next_ptr;
  else node->prev_ptr->next_ptr = node->next_ptr;
  delete node; size--;
}

#endif
