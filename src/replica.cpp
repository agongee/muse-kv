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
This is cpp file for building cluster with open servers
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

#include "hash.h"
#include "command.h"
#include "server_th.h"


int main(int argc, char** argv){

    std::cout << "------------------------REPLICA MODES ON------------------------" << std::endl;
    std::cout << "MUSE will automatically build replica based on the configuration" << std::endl;

    std::string lr, lr_s;
    std::ifstream in (argv[1]);
    std::string trimmed_lr;
    std::string* temp;
    int temp_int;
    int s, s_m, s_s;
    std::string* replica_ip;
    std::string* replica_com;
    int* replica_port;
    char buf [1024];

    int replica_num;
    struct sockaddr_in replica_addr;
    std::string quit = "quit";

    getline(in, lr);
    trimmed_lr = kvstore::trim(lr);
    lr_s = trimmed_lr;
    kvstore::split(trimmed_lr, ' ', temp, temp_int);
    std::cout << temp[0] << std::endl;
    
    if(temp[0] != "replicanumber"){
        std::cerr << "ERROR : replcia_number # \n replica ip port \n ..." << std::endl;
        exit(1);
    }
    else{
        replica_num = atoi(temp[1].c_str());
        replica_ip = new std::string [replica_num];
        replica_com = new std::string [replica_num];
        replica_port = new int [replica_num];
    }

    for(int i = 0; i < replica_num; i++){
        std::string* temp_f;
        int temp_int_f;

        getline(in, lr);
        trimmed_lr = kvstore::trim(lr);
        kvstore::split(trimmed_lr, ' ', temp_f, temp_int_f);

        std::cout << temp_f[0] << std::endl;

        if(temp_f[0] != "replica"){
            std::cerr << "ERROR : configuration: replcia_num # \nreplica ip port \n..." << std::endl;
            exit(1);
        }
        else{
            replica_ip[i] = temp_f[1];
            replica_com[i] = trimmed_lr;
            replica_port[i] = atoi(temp_f[2].c_str());
        }
    }

    std::cout << "------------REPLICA INFO PROCESS DONE------------" << std::endl;
    for(int i = 0; i < replica_num; i++){
        std::cout << i << "---ip: " << replica_ip[i] << ", port: " << replica_port[i] << std::endl;
    }

    for(int i = 0; i < replica_num; i++){

        if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            std::cerr << "ERROR : CAN'T OPEN STREAM SOCKET!"  << std::endl;
            exit(1);
        }

        std::cout << "CONNECTION with " << i << " for REPLICA INFO TRNASFER" << std::endl;

        bzero((char*)&replica_addr, sizeof(replica_addr));
        replica_addr.sin_family = AF_INET;
        replica_addr.sin_addr.s_addr = inet_addr(replica_ip[i].c_str());
        replica_addr.sin_port = htons(replica_port[i]);

        if(connect(s, (struct sockaddr *)&replica_addr, sizeof(replica_addr)) == -1){
            std::cerr << "ERROR : CAN'T CONNECT!" << std::endl;
            std::cout << strerror(errno) << std::endl;
            exit(2);
        }

        if(send(s, lr_s.data(), lr_s.size(), 0) <= 0){
            std::cerr << "ERROR : SEND TO REPLICA" << std::endl;
            exit(1);
        }
        memset(buf, 0, 1024);
        if(recv(s, buf, 1024, 0) <= 0){
            std::cerr << "ERROR : SEND TO REPLICA" << std::endl;
            exit(1);
        }

        for(int j = 0; j < replica_num; j++){
            if(j != i){
                std::cout << replica_com[j] << std::endl;
                if(send(s, replica_com[j].data(), replica_com[j].size(), 0) <= 0){
                    std::cerr << "ERROR : SEND TO MASTER" << std::endl;
                    exit(1);
                }
                std::cout << "Sent info of replica " << j << " to " << i << std::endl;
                memset(buf, 0, 1024);
                if(recv(s, buf, 1024, 0) <= 0){
                    std::cerr << "ERROR : RECV FROM MASTER" << std::endl;
                    exit(1);
                }
            }
            else{
                
            }
        }

        send(s, quit.data(), quit.size(), 0);

        close(s);
    }
}
