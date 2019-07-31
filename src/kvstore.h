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

#pragma once
#ifndef KVSTORE_H
#define KVSTORE_H

#include<iostream>
#include<string>
#include<typeinfo>
#define HAVE_STRUCT_TIMESPEC
#include<thread>
#include<mutex>
#include<time.h>

#include "hashmap.h"
#include "linkedlist.h"
#include "hashset.h"

namespace kvstore{

typedef enum dtypecode {
	String = 0, List = 1, Set = 2, Hashmap = 3,
} dtype;

typedef std::string key;

// abstracts value
typedef struct data {
	dtype data_dtype = String;
	uint64_t data_addr = -1;
} value;

class kv {
private:
	key k;
	value v;

	// for mutex lock
	std::mutex thrd;

	// for ttl
	int ttl;
	time_t timer;
	bool finished;

	// for encrypt
	uint8_t authr, authw;

public:
	/* Constructor and destructor */
	kv(key k, value v, uint8_t authr, uint8_t authw)
	{
			this->k = (std::string)k;
			this->v = v;
			ttl = -1;
			this->authr = authr;
			this->authw = authw;
			finished = false;
	}
	~kv();

	/* accessor */
	key get_k() { return k; }
	value get_v() { return v; }
	std::mutex* get_thrd() { return &thrd; }
	int get_authr() { return authr; }
	int get_authw()
	{
			return authw;
	}
	bool is_expired(time_t curr_time) {
		if(finished) return true;
		if(ttl == -1) return false;

		if(difftime(curr_time, timer) < ttl) { return false; }
		else {return true;}
	}
	int get_ttl(time_t curr_time) {
		if(ttl == -1) return -1;
		if(is_expired(curr_time)) {
			finished = true;
			return 0;
		}
		else return (ttl - difftime(curr_time, timer));
	}

	/* mutator */
	void set_k(key k) { this->k = k; }
	void set_v(value v) { this->v = v; }
	void set_ttl(int time_ex) {this->ttl = time_ex; timer = time(NULL);}
	void set_authr(uint8_t authr)
	{
			this->authr = authr;
			if(this->authr > this->authw) this->authw = this->authr;
	}
	void set_authw(uint8_t authw)
	{
			this->authw = authw;
			if(this->authr > this->authw) this->authr = this->authw;
	}

};

/* Basic Functions */
void split(std::string str, char token, std::string *&splited, int& len);
void merge(std::string& str, char token, std::string *splited, int len);
std::string trim(std::string str);
std::pair<bool, int> strtoint(std::string str);

}

#include "kvstore.cpp"
#endif
