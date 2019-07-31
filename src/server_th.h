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
This is server_th class. It contains variables and functions to execute MUSE server.
*/

#pragma once
#ifndef SERVER_TH_H
#define SERVER_TH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <time.h>

#include "kvstore.h"
#include "hashnode.h"
#include "hash.h"
#include "linkedlist.h"
#include "command.h"
#include "encrypt.h"

#define MAX_CLIENT_NUM 100
#define MAX_PACKET_SIZE 4096


struct socket_st{
    int online = 0;
    int client_fd;
    struct sockaddr_in client_addr;
    char buf [MAX_PACKET_SIZE];
    int buf_size = sizeof(buf);
    bool auto_send_yes = true;
    int auth = 0;
};

class server_th{
    public:
    ~server_th();
    int setup(int port);
    int init_slave(std::string ip_addr, std::string port_num);
    int server_accept();
    static void* server_task(void* arg);
    static void* scan_and_send(struct socket_st* ss, std::string com);
    static void* cluster_connect(std::string addr_info);
    static void* repinfo_to_cluster(struct socket_st* ss);
    static void* hash_reset(std::string my_info);

    static void* authority_init(struct socket_st* ss, int a);
    static void* give_replica_info(struct socket_st* ss, std::string com);

    static void* set_replica_num(std::string com);
    static void* replica_add(std::string com);
    static void* replica_accept(struct socket_st* ss);
   
    static void* auto_send(std::string com);
    static void* auto_send_each(std::string com, int fd);
    static void* auto_save();
    static void* ttl_check();
    static void* log_append(struct socket_st* ss, std::string com);
    static std::string try_cluster(std::string com);
    static std::string all_keys(struct socket_st* ss);
    static void* all_save();
    bool over_client();
    int get_client_num();
    
    protected:
    static int server_fd; // socket descriptor
    int port_num;   // port number
    static int client_num;  // attached clients number
    static struct sockaddr_in server_addr;  // server address
    static std::vector<socket_st*> sock_vec;    // open socket list

    // DB using HASH
    static hash::Hash* server_hash;
    
    // cluster member
    static std::vector<int> cluster_vec;
    static std::vector<int> slot_start;
    static std::vector<int> slot_end;
    static std::vector<std::string> cluster_com_vec;
    static int cluster_num;

    //replica member
    static int replica_num;
    static int repnum;
    static std::string* replica_com;
    static std::string* replica_ip;
    static int* replica_port;
    static int* replica_sock;
    //static in_addr_t* replica_s_addr;
    //static in_port_t* replica_sin_port;


    // logging
    static std::string log_name;
    // static std::ofstream* log_out;
    static std::mutex log_mutex;


};

int server_th::server_fd;
struct sockaddr_in server_th::server_addr;
int server_th::client_num;
std::vector<socket_st*> server_th::sock_vec;

hash::Hash* server_th::server_hash;

std::vector<int> server_th::cluster_vec;
std::vector<int> server_th::slot_start;
std::vector<int> server_th::slot_end;
std::vector<std::string> server_th::cluster_com_vec;
int server_th::cluster_num;

std::string server_th::log_name;
std::mutex server_th::log_mutex;

int server_th::replica_num;
int server_th::repnum;
std::string* server_th::replica_com;
std::string* server_th::replica_ip;
int* server_th::replica_port;
int* server_th::replica_sock;


#include "server_th.cpp"
#endif