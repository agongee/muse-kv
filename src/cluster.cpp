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

#define BUF_LEN 1024


int main(int argc, char ** argv){
    if(argc != 2){
        std::cerr << "usage: " << argv[0] << " [cluster_info.txt]" << std::endl;
        exit(1);
    }

    std::string lr;
    std::ifstream in (argv[1]);
    int line_num = 0;
    int cluster_num = 0;
    char** ip_list;
    int** port_list;
    int start = 0;
    int each_slot;
    std::string* addr_list;
    std::string* my_list;

    while(getline(in, lr)){
        if(line_num == 0){
            // const char* lr_temp = lr.c_str();
            for(int i = 0; i < lr.size(); i++){
                if(lr.at(i) >= '0' && lr.at(i) <= '9'){
                    std::string cluster_num_char = lr.substr(i);
                    if(atoi(cluster_num_char.c_str()) == 0){
                        std::cerr << "ERROR : CONFIGURATION WRONG FORM!" << std::endl;
                    }
                    else{
                        cluster_num = atoi(cluster_num_char.c_str());
                    }
                }
            }
            each_slot = TOTAL_SLOT_NUM / cluster_num;
            addr_list = new std::string [cluster_num];
            my_list = new std::string [cluster_num];
            std::cout << "each slot is "<< each_slot << " and total cluster is " << cluster_num << std::endl;
            line_num++;
        }
        else if(line_num == cluster_num){
            addr_list[line_num-1] = "clust " + lr + " " + std::to_string(start) + " " + std::to_string(TOTAL_SLOT_NUM - 1);
            my_list[line_num-1] = "myinfo " + lr + " " + std::to_string(start) + " " + std::to_string(TOTAL_SLOT_NUM - 1);
        }
        else{
            addr_list[line_num-1] = "clust " + lr + " " + std::to_string(start) + " " + std::to_string(start + each_slot - 1);
            my_list[line_num-1] = "myinfo "  + lr + " " + std::to_string(start) + " " + std::to_string(start + each_slot - 1);
            start = start + each_slot;
            line_num++;
        }
    }

    for(int i = 0; i < cluster_num; i++){
        std::cout <<addr_list[i] << std::endl;
    }

    std::cout << "///////////////////////////////////" << std::endl << std::endl;

    //////////////////// Cluster Init //////////////////////

    struct sockaddr_in server_addr;
    int s;
    char buf [BUF_LEN];
    std::string quit = "quit";
    char addr_buf [1000];
    char* tok;

    for (int i = 0; i < cluster_num; i++){
        if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            std::cerr << "ERROR : CAN'T OPEN STREAM SOCKET!" << std::endl;
            exit(1);
        }

        bzero((char*)&server_addr, sizeof(server_addr));
        strcpy(addr_buf, addr_list[i].c_str());
        server_addr.sin_family = AF_INET;
        tok = strtok(addr_buf, " ");
        tok = strtok(NULL, " ");
        server_addr.sin_addr.s_addr = inet_addr(tok);
        std::cout << "/// " << i << "th cluster's information" << "///" << std::endl;
        std::cout << "ip address is "<< tok << std::endl;
        tok = strtok(NULL, " ");
        server_addr.sin_port = htons(atoi(tok));
        std::cout << "port number is " << tok << std::endl;

        if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
            std::cerr << "ERROR : CAN'T CONNECT FOR CLUSTER BUILDING!" << std::endl;
            std::cout << strerror(errno) << std::endl;
            exit(2);
        }

        std::cout << "/// " << i << "th cluster connection success" << "///" << std::endl;

        for(int j = 0; j < cluster_num; j++){
            if(i == j){
                memset(buf, 0, sizeof(buf));
                send(s, my_list[j].data(), my_list[j].size(), 0);
                std::cout << my_list[j] << std::endl;
                recv(s, buf, 1024, 0);
                std::cout << j << "th cluster sending..." <<  buf <<std::endl;
                if(strcmp(buf,"cluster fail")==0){
                    exit(1);
                }
            }
            else{
                memset(buf, 0, sizeof(buf));
                send(s, addr_list[j].data(), addr_list[j].size(), 0);
                std::cout << addr_list[j] << std::endl;
                recv(s, buf, 1024, 0);
                std::cout << j << "th cluster sending..." <<  buf <<std::endl;
                if(strcmp(buf,"cluster fail")==0){
                    exit(1);
                }
            }
        }

        send(s, quit.data(), quit.size(), 0);

        close(s);
        std::cout << "////////////////////////" << std::endl << std::endl;
    }

}
