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
This is cpp file to open a single client node
*/

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <typeinfo>
#include <fstream>

#include "kvstore.h"
#include "hashnode.h"
#include "hash.h"
#include "command.h"
#include "linkedlist.h"

#define BUF_LEN 1024

std::string* replica_ip;
int* replica_port;
int replica_num;
int auth = 0;
int access_order;
int* s;

int get_replica_info(char* buf, std::string ip, int p){
    std::string is_client = "client " + std::to_string(auth);
    std::string ending = "done";
    std::string give = "give ";
    std::string tmpst;
    std::string temp, trimmed_temp;
    std::string* arguments;
    int arg_num;
    int ra;

    if(send(*s, is_client.data(), is_client.size(), 0) < 0){
        std::cerr << "Error: Can't send initial message" << std::endl;
        std::cout << strerror(errno) << std::endl;
        return 0;
    }

    memset(buf, 0, BUF_LEN);
    if(recv(*s, buf, 1024, 0) <= 0){
        std::cerr << "Error: Can't send initial message" << std::endl;
        std::cout << strerror(errno) << std::endl;
        return 0;
    }

    temp = buf;
    std::cout << "replica are total " << temp << std::endl;
    trimmed_temp = kvstore::trim(temp);
    kvstore::split(trimmed_temp, ' ', arguments, arg_num);
    replica_num = atoi(arguments[0].c_str());

    if(replica_num == 0){
        return 1;
    }

    replica_ip = new std::string [replica_num];
    replica_port = new int [replica_num];
    
    replica_ip [0] = ip;
    replica_port [0] = p;

    for(int i = 0; i < replica_num - 1; i++){
        tmpst = give + std::to_string(i);
        std::cout << tmpst << std::endl;
        if(send(*s, tmpst.data(), tmpst.size(), 0) < 0){
            std::cerr << "Error: Can't send initial message" << std::endl;
            std::cout << strerror(errno) << std::endl;
            return 0;
        }

        memset(buf, 0, BUF_LEN);
        if(recv(*s, buf, 1024, 0) <= 0){
            std::cerr << "Error: Can't recv replica info " << std::endl;
            std::cout << strerror(errno) << std::endl;
            return 0;
        }

        temp = buf;

        std::cout << temp << std::endl;

        trimmed_temp = kvstore::trim(temp);
        kvstore::split(trimmed_temp, ' ', arguments, arg_num); 

        replica_ip[i+1] = arguments[1].c_str();
        replica_port[i+1] = atoi(arguments[2].c_str());

        std::cout << "ip is " << replica_ip[i] << ", port is " << replica_port[i] << std::endl;
        std::cout << "////////////////////////////////////////" << std::endl;
    }

    std::cout << "GET ALL THE REPLICA INFO" << std::endl;
    
    for(int i = 0; i < replica_num; i++){
        std::cout << i << "th info: ip is "<< replica_ip[i] << ", port is " << replica_port[i] << std::endl;
    }
    return 1;
}

int task_client(char* buf){
    std::string com;
    ssize_t n;

    getline(std::cin, com);
    if(com == "quit" || com == "shutdown")
        return 0;
    else if(com == ""){
        return 1;
    }
    if(send(*s, com.data(), com.size(), 0) < 0){
        std::cerr << "ERROR: CLIENT SEND FAIL" << std::endl;
        std::cerr << strerror(errno) << std::endl;
    }

    memset(buf, 0, 1024);
    if(recv(*s, buf, 1024, 0) <= 0){
        std::cerr << "SERVER RECV FAIL" << std::endl;
        std::cerr << strerror(errno) << std::endl;
    }
    std::cout << "Received: " << buf << std::endl;
    return 1;
}

int main(int argc, char ** argv){
    struct sockaddr_in server_addr;
    int n;
    char buf [BUF_LEN];

    if(argc > 4 || argc < 3){
        std::cerr << "usage: " << argv[0] << " [IP Address] [Port Number] [Authority Level] " << std::endl;
        exit(1);
    }

    else if (argc == 4){
        auth = atoi(argv[3]);
    }

    s = new int;

    if((*s = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cerr << "ERROR : CAN'T OPEN STREAM SOCKET!" << std::endl;
        exit(1);
    }

    bzero((char*)&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));


    if(connect(*s, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        std::cerr << "ERROR : CAN'T CONNECT!" << server_addr.sin_addr.s_addr << ":" << server_addr.sin_port << std::endl;
        std::cout << strerror(errno) << std::endl;
        exit(2);
    }

    get_replica_info((char*)buf, (std::string)argv[1], atoi(argv[2]));

    n = 1;
    while (n){
        n = task_client((char*)buf);
    }

    close(*s);
    delete s;
}
