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
This is cpp file for building both cluster and replica with open servers
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

int cluster_num;
int* replica_num;
std::string** replica_ip;
int** replica_port;;
std::string** replica_com;
std::string* addr_list;
std::string* my_list;
int each_slot = TOTAL_SLOT_NUM;
int max_rep = 1;
std::string quit_msg = "quit";


int main(int argc, char** argv){

    if(argc != 2){
        std::cerr << "usage: " << argv[0] << " [configuration text]" << std::endl;
        exit(1);
    }

    std::cout << "-----------------------------DISTRIBUTE MODES ON----------------------------" << std::endl;
    std::cout << "MUSE will automatically build replica and cluster based on the configuration" << std::endl;
    std::cout << "-------error check 1 " << std::endl;

    std::string lr;
    std::ifstream in (argv[1]);
    std::string trimmed_lr;
    std::string* temp;
    int temp_int;

    // get total cluster number

    getline(in, lr);
    trimmed_lr = kvstore::trim(lr);
    kvstore::split(trimmed_lr, ' ', temp, temp_int);
    cluster_num = atoi(temp[1].c_str());
    replica_num = new int [cluster_num];
    replica_ip = new std::string* [cluster_num];
    replica_port = new int* [cluster_num];
    replica_com = new std::string* [cluster_num];
    each_slot = TOTAL_SLOT_NUM / cluster_num;
    //delete[] temp;

    std::cout << "-------error check 1 " << std::endl;
    

    for (int i = 0; i < cluster_num; i++){

        // get replica number of each node
        getline(in, lr);
        trimmed_lr = kvstore::trim(lr);
        kvstore::split(trimmed_lr, ' ', temp, temp_int);
        replica_num[i] = atoi(temp[1].c_str());
        if(replica_num[i] > max_rep){
            max_rep = replica_num[i];
        }
        std::cout << "-------error check 2 " << std::endl;
        replica_ip[i] = new std::string [replica_num[i]];
        replica_port[i] = new int [replica_num[i]];
        replica_com[i] = new std::string [replica_num[i]];
        //delete[] temp;
        std::cout << "-------error check 3 " << std::endl;
        // get replica info
        for(int j = 0; j < replica_num[i]; j++){
            getline(in, lr);
            trimmed_lr = kvstore::trim(lr);
            kvstore::split(trimmed_lr, ' ', temp, temp_int);
            std::cout << "-------error check 4 " << std::endl;
            replica_ip[i][j] = temp[1];
            replica_port[i][j] = atoi(temp[2].c_str());
            replica_com[i][j] = lr;
            std::cout << "-------error check 5" << std::endl;
            //delete[] temp;
        }
    }

    std::cout << "-----------GET ALL THE INFOS------------" << std::endl;

    for (int i = 0; i < cluster_num; i++){
        std::cout << "Replica number of " << i << " is " << replica_num[i] << std::endl;
        for(int j = 0; j < replica_num[i]; j++ ){
            std::cout << "ip: " << replica_ip[i][j] << " port: " << replica_port[i][j] << std::endl;
        }
    }


    // build replica

    std::cout << "-----------replica building mode begin------------" << std::endl;

    for (int i = 0; i < cluster_num; i++){
    
        std::cout << "------------" << i << "th node replica init------------" << std::endl;

        for(int j = 0; j < replica_num[i]; j++){
            int s;
            struct sockaddr_in replica_addr;
            std::string rep_com;
            char buf [1024];

            std::cout << "CONNECTION with " << j << " for REPLICA INFO TRNASFER" << std::endl;

            if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0){
                std::cerr << "ERROR : CAN'T OPEN STREAM SOCKET!" << std::endl;
                exit(1);
            }

            bzero((char*)&replica_addr, sizeof(replica_addr));
            replica_addr.sin_family = AF_INET;
            replica_addr.sin_addr.s_addr = inet_addr(replica_ip[i][j].c_str());
            replica_addr.sin_port = htons(replica_port[i][j]);

            if(connect(s, (struct sockaddr *)&replica_addr, sizeof(replica_addr)) == -1){
                std::cerr << "ERROR : CAN'T CONNECT!" << std::endl;
                std::cout << strerror(errno) << std::endl;
                exit(2);
            }

            rep_com = "replicanumber " + std::to_string(replica_num[i]);

            if(send(s, rep_com.data(), rep_com.size(), 0) <= 0){
                std::cerr << "ERROR : SEND TO REPLICA" << std::endl;
                exit(1);
            }
            memset(buf, 0, 1024);
            if(recv(s, buf, 1024, 0) <= 0){
                std::cerr << "ERROR : RECV FROM REPLICA" << std::endl;
                exit(1);
            }

            for(int k = 0; k < replica_num[i]; k++){
                if(j != k){
                    std::cout << replica_com[i][k] << std::endl;
                    if(send(s, replica_com[i][k].data(), replica_com[i][k].size(), 0) <= 0){
                        std::cerr << "ERROR : SEND TO REPLICA" << std::endl;
                        exit(1);
                    }
                    std::cout << "Sent info of replica " << k << " to " << j << std::endl;
                    memset(buf, 0, 1024);
                    if(recv(s, buf, 1024, 0) <= 0){
                        std::cerr << "ERROR : RECV FROM REPLICA" << std::endl;
                        exit(1);
                    }
                }
                else{
                    std::cout << "My info, so not sending" << std::endl;
                }
            }
            if(send(s, quit_msg.data(), quit_msg.size(), 0) <= 0){
                std::cerr << "ERROR : SEND TO REPLICA" << std::endl;
                exit(1);
            }
            close(s);
        }
    }

    std::cout << "-----------replica building mode end------------" << std::endl;

    // build cluster on replicas

    std::cout << "-----------cluster building mode begin------------" << std::endl;
    std::cout << "---------TOTAL " << max_rep << "connection needs----------" << std::endl;

    for (int i = 0; i < max_rep; i++){
        int s;
        std::string clust_com;
        struct sockaddr_in replica_addr;
        int index, slot_start, slot_end;
        char buf [1024];
        std::string retrieve;

        std::cout << "------------" << i << "th cluster connection init------------" << std::endl;

        for(int j = 0; j < cluster_num; j++){
            index = i % replica_num[j];
            bzero((char*)&replica_addr, sizeof(replica_addr));
            replica_addr.sin_family = AF_INET;
            replica_addr.sin_addr.s_addr = inet_addr(replica_ip[j][i].c_str());
            replica_addr.sin_port = htons(replica_port[j][i]);

            if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0){
                std::cerr << "ERROR : CAN'T OPEN STREAM SOCKET!" << std::endl;
                exit(1);
            }

            if(connect(s, (struct sockaddr *)&replica_addr, sizeof(replica_addr)) == -1){
                std::cerr << "ERROR : CAN'T CONNECT!" << std::endl;
                std::cout << strerror(errno) << std::endl;
                exit(2);
            }

            for(int k = 0; k < cluster_num; k++){
                if(k != cluster_num -1){
                    slot_start = each_slot * k;
                    slot_end = each_slot * (k+1) - 1;
                }
                else{
                    slot_start = each_slot * k;
                    slot_end = TOTAL_SLOT_NUM - 1;
                }

                retrieve = replica_com[k][index].substr(8);

                if(k == j){
                    clust_com = "myinfo " + retrieve + " " + std::to_string(slot_start) + " " + std::to_string(slot_end);
                }
                else{
                    clust_com = "clust " + retrieve + " " + std::to_string(slot_start) + " " + std::to_string(slot_end);
                }

                if(send(s, clust_com.data(), clust_com.size(), 0) <= 0){
                    std::cerr << "ERROR : CLUSTER BUILDING" << std::endl;
                    std::cerr << strerror(errno) << std::endl;
                    exit(1);
                }
                
                memset(buf, 0, 1024);

                if(recv(s, buf, 1024, 0) <= 0){
                    std::cerr << "ERROR : CLUSTER BUILDING" << std::endl;
                    std::cerr << strerror(errno) << std::endl;
                    exit(1);
                }
            }
            if(send(s, quit_msg.data(), quit_msg.size(), 0) <= 0){
                std::cerr << "ERROR : SEND TO MASTER" << std::endl;
                exit(1);
            }
            close(s);
            std::cout << "/// " << i << "th connection, " << j << "th cluster ///" << std::endl;
        }
    }

    std::cout << "-----------cluster building mode end------------" << std::endl;

}