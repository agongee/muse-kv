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
This is cpp file for recovering server with commnads log.
*/


#include <iostream>
#include <fstream>
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
#include <sys/time.h>
#include <time.h>

#include "kvstore.h"
#include "hashnode.h"
#include "hash.h"
#include "command.h"
#include "linkedlist.h"
#include "encrypt.h"

#define BUF_LEN 1024

std::string* replica_ip;
int* replica_port;
int replica_num;
int auth = 0;

int get_replica_info(int s, char* buf){
    std::string is_client = "client " + std::to_string(auth);
    std::string ending = "done";
    std::string give = "give ";
    std::string tmpst;
    std::string temp, trimmed_temp;
    std::string* arguments;
    int arg_num;

    if(send(s, is_client.data(), is_client.size(), 0) <= 0){
        std::cerr << "ERROR : CAN'T SEND INITIAL MESSAGE" << std::endl;
        std::cout << strerror(errno) << std::endl;
        return 0;
    }

    memset(buf, 0, BUF_LEN);
    if(recv(s, buf, 1024, 0) <= 0){
        std::cerr << "ERROR : CAN'T RECV INITIAL MESSAGE" << std::endl;
        std::cout << strerror(errno) << std::endl;
        return 0;
    }

    temp = buf;
    std::cout << temp << std::endl;
    trimmed_temp = kvstore::trim(temp);
    kvstore::split(trimmed_temp, ' ', arguments, arg_num);
    replica_num = atoi(arguments[0].c_str());
    replica_ip = new std::string [replica_num];
    replica_port = new int [replica_num];

    for(int i = 0; i < replica_num-1; i++){
        tmpst = give + std::to_string(i);
        std::cout << tmpst << std::endl;
        if(send(s, tmpst.data(), tmpst.size(), 0) <= 0){
            std::cerr << "ERROR : CAN'T SEND REPLICA INFO" << std::endl;
            std::cout << strerror(errno) << std::endl;
            return 0;
        }

        memset(buf, 0, BUF_LEN);
        if(recv(s, buf, 1024, 0) <= 0){
            std::cerr << "Error: Can't RECV REPLY" << std::endl;
            std::cout << strerror(errno) << std::endl;
            return 0;
        }

        temp = buf;

        std::cout << temp << std::endl;

        trimmed_temp = kvstore::trim(temp);
        kvstore::split(trimmed_temp, ' ', arguments, arg_num);

        replica_ip[i] = arguments[1];
        replica_port[i] = atoi(arguments[2].c_str());

        std::cout << "IP is " << replica_ip[i] << ", PORT is " << replica_port[i] << std::endl;
        std::cout << "////////////////////////////////////////" << std::endl;
    }

    return 1;
}

int task_client(int s, char* buf, char* txt_name){
    std::string com;
    ssize_t n;

    std::string lr;
    std::ifstream in (txt_name);

    struct timeval st, et;
    int elapsed;

    gettimeofday(&st, NULL);

    while(getline(in, lr)){
        com = lr;
        // If you have encrypted string use below 
        //com = encrypt_::decrypt_string(1021*1453, 28969, lr);
        if(send(s, com.data(), com.size(), 0) <= 0){
            std::cerr << "ERROR : CLIENT SEND FAIL" << std::endl;
            std::cerr << strerror(errno) << std::endl;
        }

        memset(buf, 0, sizeof(buf));
        if(recv(s, buf, 1024, 0) <= 0){
            std::cerr << "ERROR : CLIENT RECV FAIL" << std::endl;
            std::cerr << strerror(errno) << std::endl;
        }

        std::cout << "Received: " << buf << std::endl;

    }

    gettimeofday(&et, NULL);
    elapsed = ((et.tv_sec - st.tv_sec) * 1000000 + (et.tv_usec - st.tv_usec));
    std::cout << "Elapsed time is " << elapsed << std::endl;

    return 1;
}

int main(int argc, char ** argv){
    struct sockaddr_in server_addr;
    int s, n;
    char buf [BUF_LEN];

    if(argc != 5){
        std::cerr << "usage: " << argv[0] << " [ip address] [port number] [authority level] [text file]" << std::endl;
        exit(1);
    }

    else if (argc == 4){
        auth = atoi(argv[3]);
    }

    if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cerr << "ERROR : CAN'T OPEN STREAM SOCKET!" << std::endl;
        exit(1);
    }

    bzero((char*)&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    std::cout << "try..." << std::endl;

    if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        std::cerr << "ERROR : CAN'T CONNECT!" << std::endl;
        std::cout << strerror(errno) << std::endl;
        exit(2);
    }

    get_replica_info(s, (char*)buf);

    task_client(s, (char*)buf, argv[4]);

    close(s);
}
