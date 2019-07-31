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
This class helps to handle commands
*/

#ifndef COMMAND_H
#define COMMAND_H

#include<iostream>
#include<string>
#include<typeinfo>
#include<fstream>
#include<time.h>
#include<algorithm>
#include "kvstore.h"
#include "hash.h"
#define HAVE_STRUCT_TIMESPEC
#include"pthread.h"

namespace command
{

typedef enum commandtype
{
    Invalid = 0, Read = 1, Write = 2
} comtype;

class COMMAND
{
private:
    const std::string CONFIRM_MSG = "OK!";
    std::string com;
    std::string *args;
    int num_args;
    uint8_t auth = 0;

public:

    COMMAND(std::string str, uint8_t auth)
    {
        com = str;
        this->auth = auth;
    }
    COMMAND() {}

    //main execution for given command
    std::string execute(hash::Hash *h)
    {
        com = kvstore::trim(com);
        kvstore::split(com, ' ', args, num_args);
        //split to arguments from one line command
        //from first arguments and the number of arguments, determine and call appropriate function

        if(num_args == 0)
            return "ERROR : INVALID COMMAND";

        std::string magic = " ";
        magic += char(8);
        if (args[0] == "client") return magic + "client";
        else if (args[0] == "give") return magic + "give";
        else if (args[0] == "clust") return magic + "clust";
		else if (args[0] == "myinfo") return magic + "myinfo";
		else if (args[0] == "replicanumber") return magic + "replicanumber";
		else if (args[0] == "replica") return magic + "replica";
		else if (args[0] == "replicaaccept") return magic + "replicaaccept";

		else if( num_args > 1){
			kvstore::key temp_k = args[1];
			int hash_val = h->hash_func(temp_k);
			std::cout << "key is " << hash_val << std::endl;
			if(hash_val < h->get_start() || hash_val > h->get_end()){
				std::cout << "NOT HERE!" << std::endl;
				return "hashval " + std::to_string(hash_val);
			}
		}

        if      (args[0] == "get"   &&  num_args == 2)   return get(h);
        else if (args[0] == "make"  &&  num_args == 3)   return make(h);
        else if (args[0] == "make"  &&  num_args == 4)   return make(h);
        else if (args[0] == "make"  &&  num_args == 5)   return make(h);
        else if (args[0] == "del"   &&  num_args == 2)   return del(h);

        else if (args[0] == "lmake" &&  num_args >= 3)   return lmake(h);
        else if (args[0] == "lpush" &&  num_args >= 3)   return lpush(h);
        else if (args[0] == "rpush" &&  num_args >= 3)   return rpush(h);
        else if (args[0] == "lpop"  &&  num_args == 2)   return lpop(h);
        else if (args[0] == "rpop"  &&  num_args == 2)   return rpop(h);
        else if (args[0] == "lget"  &&  num_args == 3)   return lget(h);
        else if (args[0] == "lset"  &&  num_args == 4)   return lset(h);
        else if (args[0] == "lerase"&&  num_args == 3)   return lerase(h);

        else if (args[0] == "smake" &&  num_args >= 3)   return smake(h);
        else if (args[0] == "spush" &&  num_args >= 3)   return spush(h);
        else if (args[0] == "spop"  &&  num_args == 2)   return spop(h);
        else if (args[0] == "serase"&&  num_args == 3)   return serase(h);
        else if (args[0] == "sort"  &&  num_args == 2)   return sort(h);

        else if (args[0] == "hmake" &&  num_args >= 4)   return hmake(h);
        else if (args[0] == "hpush" &&  num_args == 4)   return hpush(h);
        else if (args[0] == "hget"  &&  num_args == 3)   return hget(h);
        else if (args[0] == "hfields"&& num_args == 2)  return hfields(h);
        else if (args[0] == "hvals" &&  num_args == 2)   return hvals(h);
        else if (args[0] == "herase"&&  num_args == 3)   return herase(h);
        else if (args[0] == "xkeys" &&  num_args == 1)   return keys(h);
        else if (args[0] == "keys"  &&  num_args == 1)   return keys(h);

        else if (args[0] == "authr" &&  num_args == 3)   return authr(h);
        else if (args[0] == "authw" &&  num_args == 3)   return authw(h);

        else if (args[0] == "expire"&&  num_args == 3)   return expire(h);
        else if (args[0] == "ttl"   &&  num_args == 2)   return get_ttl(h);
        else if (args[0] == "save"  &&  num_args == 1)   return save(h);
        else if (args[0] == "xsave" &&  num_args == 1)   return save(h);
        else if (args[0] == "read"  &&  num_args == 2)   return read(h);
        else                                            return "ERROR : INVALID COMMAND";

        delete args;
    }

    //return command_type : Read / Write / Invalid
    comtype command_type(hash::Hash *h)
    {
        com = kvstore::trim(com);
        kvstore::split(com, ' ', args, num_args);

        if(num_args == 0)
            return Invalid;

        if      (args[0] == "get"   &&  num_args == 2)  return Read;
        else if (args[0] == "make"  &&  num_args == 3)  return Write;
        else if (args[0] == "make"  &&  num_args == 4)  return Write;
        else if (args[0] == "make"  &&  num_args == 5)  return Write;
        else if (args[0] == "del"   &&  num_args == 2)  return Write;

        else if (args[0] == "lmake" &&  num_args >= 3)  return Write;
        else if (args[0] == "lpush" &&  num_args >= 3)  return Write;
        else if (args[0] == "rpush" &&  num_args >= 3)  return Write;
        else if (args[0] == "lpop"  &&  num_args == 2)  return Write;
        else if (args[0] == "rpop"  &&  num_args == 2)  return Write;
        else if (args[0] == "lget"  &&  num_args == 3)  return Read;
        else if (args[0] == "lset"  &&  num_args == 4)  return Write;
        else if (args[0] == "lerase"&&  num_args == 3)  return Write;

        else if (args[0] == "smake" &&  num_args >= 3)  return Write;
        else if (args[0] == "spush" &&  num_args >= 3)  return Write;
        else if (args[0] == "spop"  &&  num_args == 2)  return Write;
        else if (args[0] == "serase"&&  num_args == 3)  return Write;
        else if (args[0] == "sort"  &&  num_args == 2)  return Read;

        else if (args[0] == "hmake" &&  num_args >= 4)  return Write;
        else if (args[0] == "hpush" &&  num_args == 4)  return Write;
        else if (args[0] == "hget" 	&&  num_args == 3)  return Read;
        else if (args[0] == "hfields"&& num_args == 2)  return Read;
        else if (args[0] == "hvals" &&  num_args == 2)  return Read;
        else if (args[0] == "herase"&&  num_args == 3)  return Write;
        else if (args[0] == "keys"  &&  num_args == 1)  return Read;

        else if (args[0] == "authr" &&  num_args == 3)  return Write;
        else if (args[0] == "authw" &&  num_args == 3)  return Write;

        else if (args[0] == "expire"&&  num_args == 3)  return Write;
        else if (args[0] == "ttl"   && 	num_args == 2)  return Read;
        else if (args[0] == "save"  &&  num_args == 1)  return Read;
        else if (args[0] == "read"  &&  num_args == 2)  return Write;
        else if (args[0] == "exit"  &&  num_args == 1)  return Read;
        else                                            return Invalid;

        delete args;
    }

    std::string get(hash::Hash *h);
    std::string make(hash::Hash *h);
    std::string del(hash::Hash *h);
    std::string keys(hash::Hash *h);
    std::string lmake(hash::Hash *h);
    std::string lpush(hash::Hash *h);
    std::string rpush(hash::Hash *h);
    std::string lpop(hash::Hash *h);
    std::string rpop(hash::Hash *h);
    std::string lget(hash::Hash *h);
    std::string lset(hash::Hash *h);
    std::string lerase(hash::Hash *h);
    std::string smake(hash::Hash *h);
    std::string spush(hash::Hash *h);
    std::string spop(hash::Hash *h);
    std::string serase(hash::Hash *h);
    std::string sort(hash::Hash *h);
    std::string hmake(hash::Hash *h);
    std::string hpush(hash::Hash *h);
    std::string hget(hash::Hash *h);
    std::string hfields(hash::Hash *h);
    std::string hvals(hash::Hash *h);
    std::string herase(hash::Hash *h);
    std::string authr(hash::Hash *h);
    std::string authw(hash::Hash *h);
    std::string expire(hash::Hash *h);
    std::string get_ttl(hash::Hash *h);
    std::string save(hash::Hash *h);
    std::string read(hash::Hash *h);
};

kvstore::value data_encode(std::string data, kvstore::dtype data_type);
std::string data_decode(kvstore::value v);

}
#include "command.cpp"
#endif
