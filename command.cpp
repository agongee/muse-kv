#ifndef COMMAND_CPP
#define COMMAND_CPP
#include "command.h"
#include "linkedlist.h"

inline kvstore::value command::data_encode(std::string data, kvstore::dtype data_type)
{
    kvstore::value encoded;
    encoded.data_dtype = data_type;

    switch(data_type)
    {

    case kvstore::String:
    {
        std::string *data_save = new std::string(data);

        encoded.data_addr = (uint64_t)data_save;
        return encoded;
    }
    case kvstore::List:
    {
        std::string *data_splited;
        int len;
        kvstore::split(data, ',', data_splited, len);

        linkedlist::List *data_save = new linkedlist::List();

        encoded.data_addr = (uint64_t)data_save;

        for(int i = 0; i < len; i++)
            data_save->push_back(data_splited[i]);

        delete[] data_splited;
        return encoded;
    }
    case kvstore::Set:
    {
        std::string *data_splited;
        int len;
        kvstore::split(data, ',', data_splited, len);

        hashset::Hashset *data_save = new hashset::Hashset();
        encoded.data_addr = (uint64_t)data_save;

        for(int i = 0; i < len; i++)
            data_save->push(data_splited[i]);

        delete[] data_splited;
        return encoded;
    }
    case kvstore::Hashmap:
    {

        std::string *data_splited;
        int len;
        kvstore::split(data, ',', data_splited, len);

        hashmap::Hashmap *data_save = new hashmap::Hashmap();
        encoded.data_addr = (uint64_t)data_save;

        for(int i=0; i<len/2; i++)
        {
            data_save -> set_fv(data_splited[2*i], data_splited[2*i+1]);
        }

        delete[] data_splited;
        return encoded;
    }
    default:
    {
        return kvstore::value{};
    }
    }
}

inline std::string command::data_decode(kvstore::value v)
{
    switch(v.data_dtype)
    {

    case kvstore::String:
    {
        return *((std::string*)(v.data_addr));
    }
    case kvstore::List:
    {
        linkedlist::List *data = (linkedlist::List*)(v.data_addr);

        std::string print = data->print();

        if(print != "")
            return print;
        else
            return "(empty)";
    }
    case kvstore::Set:
    {
        hashset::Hashset *data = (hashset::Hashset*)(v.data_addr);

        std::string print = data->print();

        if(print != "")
            return print;
        else
            return "(empty)";
    }
    case kvstore::Hashmap:
    {
        hashmap::Hashmap *h_map_ = (hashmap::Hashmap*)(v.data_addr);

        if(h_map_->is_empty())
            return "(empty)";
        else
            return h_map_->fvs();
    }
    default:
    {
        return "BLANK";
    }
    }
}


inline std::string command::COMMAND::get(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    if(auth < node->get_authr())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    return data_decode(node->get_v());
}

inline std::string command::COMMAND::make(hash::Hash* h)
{
    kvstore::key k = args[1];
    kvstore::value v = data_encode(args[2], kvstore::String);

    int tr;
    hashnode::hashNode* node = h->find_node(k, &tr);
    if(node != NULL && auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    switch(num_args)
    {
    case 3:
    {
        h->set(k, v, 0, auth);
        return CONFIRM_MSG;
    }
    case 4:
    {
        std::pair<bool, int> temp = kvstore::strtoint(args[3]);
        if(!temp.first || temp.second < 0)
            return "ERROR09 : INVALID AUTH";
        if(auth < temp.second)
            return "ERROR11 : NOT ENOUGH PERMISSION";

        h->set(k, v, temp.second, auth);
        return CONFIRM_MSG;
    }
    case 5:
    {
        std::pair<bool, int> temp1 = kvstore::strtoint(args[3]);
        std::pair<bool, int> temp2 = kvstore::strtoint(args[4]);
        if(!temp1.first || temp1.second < 0)
            return "ERROR09 : INVALID AUTH";
        if(!temp2.first || temp2.second < 0)
            return "ERROR09 : INVALID AUTH";
        if(auth < temp1.second)
            return "ERROR11 : NOT ENOUGH PERMISSION";
        if(auth < temp2.second)
            return "ERROR11 : NOT ENOUGH PERMISSION";

        h->set(k, v, temp1.second, temp2.second);
        return CONFIRM_MSG;
    }
    default:
    {
        return "ERORR!!!";
    }
    }
}

inline std::string command::COMMAND::del(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    h->hash::Hash::del(k);
    return CONFIRM_MSG;
}

inline std::string command::COMMAND::keys(hash::Hash* h)
{
    std::string res = "";

    std::string* keys = h->keys();
    int size = h->get_size();
    bool flag = true;
    int tr;

    for(int i = 0; i < size; i++)
    {
        hashnode::hashNode* node = h->find_node(keys[i], &tr);
        if(auth < node->get_authr())
            continue;
        if(flag)
        {
            res = node->get_k();
            flag = false;
        }
        else
            res += "," + node->get_k();
    }

    return res;
}

inline std::string command::COMMAND::lmake(hash::Hash* h)
{
    kvstore::key k = args[1];

    int tr;
    hashnode::hashNode* node = h->find_node(k, &tr);
    if(node != NULL && auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    std::string str = std::string(args[2]);
    for(int i = 3; i < num_args; i++)
    {
        str = str + "," + args[i];
    }
    kvstore::value v = data_encode(str, kvstore::List);
    h->hash::Hash::set(k, v, 0, auth);

    return std::to_string( num_args - 2 );
}

inline std::string command::COMMAND::lpush(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::List)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    for(int i = 2; i < num_args; i++)
        ((linkedlist::List *)(v.data_addr))->push_front(args[i]);
    return CONFIRM_MSG;
}

inline std::string command::COMMAND::rpush(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::List)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    for(int i = 2; i < num_args; i++)
        ((linkedlist::List*)(v.data_addr))->push_back(args[i]);
    return CONFIRM_MSG;
}

inline std::string command::COMMAND::lpop(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::List)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";
    linkedlist::List* l = (linkedlist::List*)(v.data_addr);
    if(l->is_empty())
        return "ERROR04 : EMPTY LIST";

    return l->pop_front();
}

inline std::string command::COMMAND::rpop(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::List)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";
    linkedlist::List* l = (linkedlist::List*)(v.data_addr);
    if(l->is_empty())
        return "ERROR04 : EMPTY LIST";

    return l->pop_back();
}

inline std::string command::COMMAND::lget(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::List)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authr())
        return "ERROR11 : NOT ENOUGH PERMISSION";
    linkedlist::List* l = (linkedlist::List*)(v.data_addr);
    if(l->is_empty())
        return "ERROR04 : EMPTY LIST";
    std::pair<bool, int> temp = kvstore::strtoint(args[2]);
    if(!temp.first)
        return "ERROR07 : INVALID INDEX";

    return l->get(temp.second);
}

inline std::string command::COMMAND::lset(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::List)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";
    linkedlist::List* l = (linkedlist::List*)(v.data_addr);
    if(l->is_empty())
        return "ERROR04 : EMPTY LIST";
    std::pair<bool, int> temp = kvstore::strtoint(args[2]);
    if(!temp.first)
        return "ERROR07 : INVALID INDEX";

    l->set(temp.second, args[3]);
    return CONFIRM_MSG;
}

inline std::string command::COMMAND::lerase(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::List)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";
    linkedlist::List* l = (linkedlist::List*)(v.data_addr);
    if(l->is_empty())
        return "ERROR04 : EMPTY LIST";
    std::pair<bool, int> temp = kvstore::strtoint(args[2]);
    if(!temp.first)
        return "ERROR07 : INVALID INDEX";

    l->del(temp.second);
    return CONFIRM_MSG;
}

inline std::string command::COMMAND::smake(hash::Hash* h)
{
    kvstore::key k = args[1];

    int tr;
    hashnode::hashNode* node = h->find_node(k, &tr);
    if(node != NULL && auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";


    std::string str = std::string(args[2]);
    for(int i = 3; i < num_args; i++)
    {
        str = str + "," + args[i];
    }

    kvstore::value v = data_encode(str, kvstore::Set);
    h->hash::Hash::set(k, v, 0, auth);

    return std::to_string( ((hashset::Hashset*)(v.data_addr))->get_length() );
}

inline std::string command::COMMAND::spush(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::Set)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    hashset::Hashset* s = (hashset::Hashset*)(v.data_addr);
    for(int i = 2; i < num_args; i++)
        s->push(args[2]);
    return CONFIRM_MSG;
}

inline std::string command::COMMAND::spop(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::Set)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";
    hashset::Hashset* s = (hashset::Hashset*)(v.data_addr);
    if(s->is_empty())
        return "ERROR04 : EMPTY LIST";

    return s->pop();
}

inline std::string command::COMMAND::serase(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::Set)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";
    hashset::Hashset* s = (hashset::Hashset*)(v.data_addr);
    if(s->is_empty())
        return "ERROR04 : EMPTY LIST";
    if(!s->del(args[2]))
        return "ERROR02 : NO SUCH ELEMENT (" + args[2] + ")";

    return CONFIRM_MSG;
}

inline std::string command::COMMAND::sort(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::List && v.data_dtype != kvstore::Set)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authr())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    std::string str;
    if(v.data_dtype == kvstore::List)
        str = ((linkedlist::List*)(v.data_addr))->print();
    if(v.data_dtype == kvstore::Set)
        str = ((hashset::Hashset*)(v.data_addr))->print();

    std::string *splited;
    int len;
    kvstore::split(str, ',', splited, len);

    std::sort(splited, splited + len);
    kvstore::merge(str, ',', splited, len);

    delete[] splited;
    return str;
}


inline std::string command::COMMAND::hmake(hash::Hash *h)
{
    if((num_args % 2) != 0)
        return "ERROR08 : INVALID COMMAND";

    kvstore::key k = args[1];

    int tr;
    hashnode::hashNode* node = h->find_node(k, &tr);
    if(node != NULL && auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    // merge function
    std::string str = std::string(args[2]);
    for(int i = 3; i < num_args; i++)
        str = str + "," + args[i];

    kvstore::value v = data_encode(str, kvstore::Hashmap);
    h->hash::Hash::set(k, v, 0, auth);

    return CONFIRM_MSG;
}

inline std::string command::COMMAND::hpush(hash::Hash *h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::Hashmap)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    ((hashmap::Hashmap*)(v.data_addr))->set_fv(args[2], args[3]);
    return CONFIRM_MSG;
}

inline std::string command::COMMAND::hget(hash::Hash *h)
{
    kvstore::key k = args[1];
    std::string f = args[2];
    int index = 0;
    time_t curr_time = time(NULL);

    hashnode::hashNode* node = h->hash::Hash::find_node(k,&index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::Hashmap)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authr())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    hashmap::Hashmap* h_map_ = (hashmap::Hashmap*)(v.data_addr);
    if(h_map_->hashmap::Hashmap::is_empty())
        return "ERROR05 : EMPTY HASHMAP";

    std::string result = h_map_->hashmap::Hashmap::get_v(f);
    if(result == "NULL")
        return "ERROR03 : NO SUCH FIELD (" + f + ")";
    else
        return result;
}

inline std::string command::COMMAND::hfields(hash::Hash *h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);

    hashnode::hashNode* node = h->hash::Hash::find_node(k,&index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::Hashmap)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authr())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    hashmap::Hashmap* h_map_ = (hashmap::Hashmap*)(v.data_addr);
    if(h_map_->hashmap::Hashmap::is_empty())
        return "ERROR05 : EMPTY HASHMAP";
    else
        return h_map_->hashmap::Hashmap::fields();

}

inline std::string command::COMMAND::hvals(hash::Hash *h)
{
    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);

    hashnode::hashNode* node = h->hash::Hash::find_node(k,&index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::Hashmap)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authr())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    hashmap::Hashmap* h_map_ = (hashmap::Hashmap*)(v.data_addr);
    if(h_map_->hashmap::Hashmap::is_empty())
        return "ERROR05 : EMPTY HASHMAP";
    else
        return h_map_->hashmap::Hashmap::values();
}

inline std::string command::COMMAND::herase(hash::Hash *h)
{
    kvstore::key k = args[1];
    std::string f = args[2];
    int index = 0;
    time_t curr_time = time(NULL);

    hashnode::hashNode* node = h->hash::Hash::find_node(k,&index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    kvstore::value v = node->get_v();
    if(v.data_dtype != kvstore::Hashmap)
        return "ERROR06 : INVALID TYPE";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    hashmap::Hashmap* h_map_ = (hashmap::Hashmap*)(v.data_addr);
    if(h_map_->hashmap::Hashmap::is_empty())
        return "ERROR05 : EMPTY HASHMAP";

    hashmap::hashmapNode* h_map_node = h_map_->find_hashmapNode(f, &index);
    if(h_map_node == NULL)
        return "ERROR03 : NO SUCH FIELD (" + f + ")";
    else
    {
        h_map_->hashmap::Hashmap::del_fv(f);
        return CONFIRM_MSG;
    }
}

inline std::string command::COMMAND::authr(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    std::pair<bool, int> temp = kvstore::strtoint(args[2]);
    if(!temp.first || temp.second < 0)
        return "ERROR09 : INVALID AUTH";
    if(auth < temp.second)
        return "ERROR11 : NOT ENOUGH PERMISSION";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    node->set_authr(temp.second);
    return CONFIRM_MSG;
}

inline std::string command::COMMAND::authw(hash::Hash* h)
{
    kvstore::key k = args[1];
    int index = 0;
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    std::pair<bool, int> temp = kvstore::strtoint(args[2]);
    if(!temp.first || temp.second < 0)
        return "ERROR09 : INVALID AUTH";
    if(auth < temp.second)
        return "ERROR11 : NOT ENOUGH PERMISSION";
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";

    node->set_authw(temp.second);
    return CONFIRM_MSG;
}

inline std::string command::COMMAND::expire(hash::Hash* h)
{
    kvstore::key k = args[1];
    std::pair<bool, int> temp = kvstore::strtoint(args[2]);
    if(!temp.first)
        return "ERROR10 : INVALID TIME";
    int time_ex = temp.second;

    int index = 0;
    time_t curr_time = time(NULL);
    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    if(auth < node->get_authw())
        return "ERROR11 : NOT ENOUGH PERMISSION";
    node->set_ttl(time_ex);
    return CONFIRM_MSG;
}

inline std::string command::COMMAND::get_ttl(hash::Hash* h)
{

    kvstore::key k = args[1];
    int index = 0;
    time_t curr_time = time(NULL);

    hashnode::hashNode* node = h->find_node(k, &index);

    if(node == NULL)
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    if(node->is_expired(curr_time))
    {
        h->hash::Hash::del(k);
        return "ERROR01 : NO SUCH KEY (" + k + ")";
    }
    else
        return std::to_string(node->get_ttl(curr_time));
}

// NEED TO SEE
inline std::string command::COMMAND::save(hash::Hash* h)
{
    if(h->save())
        return CONFIRM_MSG;
    else
        return "ERROR12 : NO FILE";
}

inline std::string command::COMMAND::read(hash::Hash* h)
{
    if(h->read(args[1]))
        return CONFIRM_MSG;
    else
        return "ERROR13 : FAIL TO READ FILE";
}

#endif
