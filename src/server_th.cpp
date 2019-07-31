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

#ifndef SERVER_TH_CPP
#define SERVER_TH_CPP

#include "server_th.h"

server_th::~server_th(){
    delete(server_hash);
}

// server setup
int server_th::setup(int port){
    client_num = 0;
    port_num = port;
    server_hash = new hash::Hash(port);
    std::thread save_thread(server_th::auto_save);
    //std::thread ttl_thread(server_th::ttl_check);
    save_thread.detach();
    //ttl_thread.detach();

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        //socket create fail
        std::cerr << "ERROR : CAN'T OPEN STREAM SOCKET!" << std::endl;
        exit(1);
    }

    memset(&server_addr, 0x00, sizeof(server_addr));    // server_addr init to NULL

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        //socket bind fail
        std::cerr << "ERROR : CAN'T BIND LOCAL ADDRESS" << std::endl;
        std::cerr << strerror(errno) << std::endl;
        exit(1);
    }

    if(listen(server_fd, 5) < 0){
        //socket wait error
        std::cerr << "ERROR : CAN'T LISTEN" << std::endl;
        std::cerr << strerror(errno) << std::endl;
        exit(1);
    }

    log_name = "log_" + std::to_string(port) + ".txt";

    return 0;

}

// server accept connection request from clients and initialize command system
int server_th::server_accept(){
    struct socket_st* ss = new struct socket_st;
    socklen_t len = sizeof(ss->client_addr);

    std::cout << "Listening... " << std::endl;
    ss->client_fd = accept(server_fd, (struct sockaddr *)&ss->client_addr, &len);
    ss->online = 1;

    if(ss->client_fd < 0){
        std::cerr << "ERROR : ACCEPT FAILED!" << std::endl;
        std::cerr << strerror(errno) << std::endl;
        return 0;
    }
    else{
        std::cout << "---SERVER ACCEPTED CLIENT!---" << std::endl;
    }
    sock_vec.push_back(ss);

    std::thread client_thread (server_th::server_task, ss);
    client_thread.detach();
    client_num++;
    return 1;
}

// command system, communicate with client
void* server_th::server_task(void* arg){
    struct socket_st* ss = (struct socket_st*) arg;
    std::string com;

    while(1){
        if(!ss->online){
            close(ss->client_fd);
            client_num--;
            std::vector<struct socket_st*>::iterator it = std::find(sock_vec.begin(), sock_vec.end(), ss);
            sock_vec.erase(it);
            free(ss);
            break;
        }

        // receive commands from clients
        memset(ss->buf, 0x00, 1024);
        if(recv(ss->client_fd, ss->buf, 1024, 0) <= 0){
            std::cerr << "ERROR : CAN'T RECV COMMAND" << std::endl;
            std::cerr << strerror(errno) << std::endl;
            break;
        }

        com = ss->buf;
        if(com == "quit"){
            ss->online = 0;
            continue;
        }

        // scan the commands and do the requested job
        std::thread scaning_thread (server_th::scan_and_send, ss, com);
        scaning_thread.detach();
    }
}

// process the command from client
void* server_th::scan_and_send(struct socket_st* ss, std::string com){
    std::string com_result, trimmed_com, com_auth, com_enc;
    char clust_buf [1024];
    int key_val;
    int index;
    std::string try_result;
    std::string magic = " ";
    magic += char(8);
    std::string auth_ch = " ";
    auth_ch += char(8);
    auth_ch += " ";
    auth_ch += std::to_string(ss->auth);
    size_t bsp = com.find((char)8);
    int new_auth = atoi(com.substr(bsp+1).c_str());
    command::comtype val = command::COMMAND(com, 0).command_type(server_hash);

    // authority check for commands from other node in cluster
    if(bsp != std::string::npos){
        com = com.substr(0, bsp);
        com_result = command::COMMAND(com, new_auth).execute(server_hash);
    }
    else{
        com_result = command::COMMAND(com, ss->auth).execute(server_hash);
    }

    std::cout << "Command result is // " << com_result << " //" << std::endl;

    // keys and save command needs to get info from other nodes in cluster
    trimmed_com = kvstore::trim(com);
    if(trimmed_com.substr(0, 4) == "keys"){
        com_result += all_keys(ss);
    }
    else if(trimmed_com.substr(0, 4) == "save"){
        all_save();
    }

    // special command for setup

    // server get client's authority information and send replica number
    if(com_result == magic + "client"){
        authority_init(ss, atoi(com.substr(7).c_str()));
        return 0;
    }
    // server send replica information
    if(com_result == magic + "give"){
        give_replica_info(ss, com);
        return 0;
    }
    // cluster node information
    else if(com_result == magic + "clust"){
        cluster_connect(com);
    }
    // this node's hash value range
    else if(com_result == magic + "myinfo"){
        hash_reset(com);
    }
    // seutp replica number
    else if(com_result == magic + "replicanumber"){
        set_replica_num(com);
    }
    // replica node information
    else if(com_result == magic + "replica"){
        replica_add(com);
    }
    //
    else if(com_result == magic +  "replicaaccept"){
        replica_accept(ss);
        return 0;
    }

    // command which needs request from another node (out of hash value range)
    else if(com_result.find("hashval") != std::string::npos){
        std::cout << "Cluster request for key value: " << com_result.substr(8) << std::endl;
        key_val = std::atoi(com_result.substr(8).c_str());
        for (index = 0; index < cluster_num; index++){
            if(key_val >= slot_start[index] && key_val <= slot_end[index])
                break;
        }

        com += auth_ch;
        com += "\0";

        // IMPORTANT!
        // If, connected node is fail-over, server send the command to one of its replica
        // The replica, then send the command to another cluster node
        if(send(cluster_vec[index], com.data(), com.size(), 0) < 0){
            std::cerr << "ERROR: send failed!" << std::endl;
            std::cerr << strerror(errno) << std::endl;
            std::cerr << "TRY TO ANOTHER CLUSTER NODE!" << std::endl;
            try_result = try_cluster(com);
            if(try_result != magic)
                com_result = try_result;
        }
        else{
            std::cout << "Send to another cluster success!" << std::endl;
            memset(clust_buf, 0x00, sizeof(clust_buf));
            recv(cluster_vec[index], clust_buf, 1024, 0);
            std::cout << "Received: " << clust_buf << std::endl;
            com_result = clust_buf;
        }
    }

    // In this range
    else{
        std::cout << "This key value is MINE!!!" << std::endl;
        com_auth = com;
        com_auth += char(8);
        com_auth += " ";
        if(bsp != std::string::npos){
            com_auth += std::to_string(new_auth);
        }
        else{
            com_auth += std::to_string(ss->auth);
        }
        if(val == command::Write){
            std::thread log_append_th (server_th::log_append, ss, com_auth);
            log_append_th.detach();
        }

        if(ss->auto_send_yes && val == command::Write){
            std::thread auto_send_th (server_th::auto_send, com);
            auto_send_th.detach();
        }
    }
    std::cout << "Sending back: " << com_result << std::endl;
    com_result += "\0";
    if(send(ss->client_fd, com_result.data(), com_result.size(), 0) < 0){
        std::cerr << "ERROR: Can't send the command result to client" << std::endl;
        std::cerr << strerror(errno) << std::endl;
    }
    std::cout << "Send Done!" << std::endl;
}

// cluster initialize
void* server_th::cluster_connect(std::string com){
    std::string trimmed_com;
    std::string* temp;
    int temp_int;
    int cluster_fd;
    std::vector<int>::iterator iter;
    struct sockaddr_in cluster_addr;
    std::string cl_com;

    // initialize socket connected to cluster node

    cluster_vec.push_back(cluster_fd);
    iter = cluster_vec.end();
    iter--;
    cluster_num++;

    trimmed_com = kvstore::trim(com);
    kvstore::split(trimmed_com, ' ' ,temp, temp_int);
    cluster_addr.sin_family = AF_INET;
    cluster_addr.sin_addr.s_addr = inet_addr(temp[1].c_str());
    cluster_addr.sin_port = htons(atoi(temp[2].c_str()));

    if((*iter =  socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cerr << "ERROR : CLUSTER SOCKET CONNECTION FAILED!" << std::endl;
        std::cerr << strerror(errno) << std::endl;
        return 0;
    }

    if(connect(*iter, (struct sockaddr *)&(cluster_addr), sizeof(cluster_addr)) == -1){
        std::cerr << "ERROR : CAN'T CONNECT TO " << temp[1] << ":" << temp[2] << "FOR CLUSTER" << std::endl;
        std::cout << strerror(errno) << std::endl;
        return 0;
    }

    std::cout << "Cluster Connection with " << temp[1] << " " << temp[2] << std::endl;

    slot_start.push_back(atoi(temp[3].c_str()));
    slot_end.push_back(atoi(temp[4].c_str()));
    std::cout << "cluster_fd is " << *iter << std::endl;

    client_num--;
    return 0;
}

// Because of cluster building, the hash value range of the node can be changed
// This function changes the range
void* server_th::hash_reset(std::string com){
    std::string trimmed_com;
    std::string* temp;
    int temp_int;
    int st, en;

    trimmed_com = kvstore::trim(com);
    kvstore::split(trimmed_com, ' ' ,temp, temp_int);

    std::cout << com << std::endl;

    server_hash->set_boundary(atoi(temp[3].c_str()), atoi(temp[4].c_str()));

    std::cout << "set boundary: start index is " << server_hash->get_start() << ", end index is " << server_hash->get_end() << std::endl;
    return 0;
}

// set client's authority value
// send clients the replica number
void* server_th::authority_init(struct socket_st* ss, int a){

    // set client's authority value
    std::string repnum_st = std::to_string(replica_num);
    ss->auth = a;

    std::cout << "Authority level of client is -------->" << ss->auth << std::endl;

    // send clients the replica number
    repnum_st += "\0";
    if(send(ss->client_fd, repnum_st.data(), repnum_st.size(), 0) <= 0){
        std::cerr << "Error: Can't send replica initial info " << std::endl;
        std::cout << strerror(errno) << std::endl;
        return 0;
    }

}

// give replica information to client
void* server_th::give_replica_info(struct socket_st* ss, std::string com){
    int index;
    std::string* arguments;
    int arg_num;
    std::string trimmed_com;

    trimmed_com = kvstore::trim(com);
    kvstore::split(trimmed_com, ' ', arguments, arg_num);
    index = atoi(arguments[1].c_str());

    std::cout << index << "'s INFO Request ARG[1] is " << index << std::endl;

    if(send(ss->client_fd, replica_com[index].data(), replica_com[index].size(), 0) <= 0){
        std::cerr << "Error: Can't send replica info " << std::endl;
        std::cout << strerror(errno) << std::endl;
        return 0;
    }
}

// set replica number from configuration text
void* server_th::set_replica_num(std::string com){
    std::string trimmed_com;
    std::string* temp;
    int temp_int;

    trimmed_com = kvstore::trim(com);
    kvstore::split(trimmed_com, ' ' ,temp, temp_int);

    replica_num = atoi(temp[1].c_str());
    repnum = 0;
    replica_com = new std::string [replica_num-1];
    replica_ip = new std::string [replica_num-1];
    replica_port = new int [replica_num-1];
    replica_sock = new int [replica_num-1];
}

// add a single replica
void* server_th::replica_add(std::string com){
    std::string trimmed_com;
    std::string* temp;
    int temp_int;
    struct sockaddr_in replica_addr;
    std::string replicaaccept = "replicaaccept";

    trimmed_com = kvstore::trim(com);
    kvstore::split(trimmed_com, ' ' ,temp, temp_int);

    if(repnum < replica_num-1){
        replica_com[repnum] = trimmed_com;
        replica_ip[repnum] = temp[1];
        replica_port[repnum] = atoi(temp[2].c_str());

        memset(&replica_addr, 0x00, sizeof(replica_addr));

        replica_addr.sin_family = AF_INET;
        replica_addr.sin_addr.s_addr = inet_addr(temp[1].c_str());
        replica_addr.sin_port = htons(replica_port[repnum]);

        std::cout << "[rep add] port: " << replica_addr.sin_port << std::endl;
        std::cout << "[rep add] ip: " << replica_addr.sin_addr.s_addr << std::endl;

        if((replica_sock[repnum] = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            std::cerr << "cluster socket connection failed!" << std::endl;
            std::cout << strerror(errno) << std::endl;
            return 0;
        }

        if(connect(replica_sock[repnum], (struct sockaddr *)&(replica_addr), sizeof(replica_addr)) == -1){
            std::cerr << "Error: Can't connect to replica " << std::endl;
            std::cout << strerror(errno) << std::endl;
            return 0;
        }

        if(send(replica_sock[repnum], replicaaccept.data(), replicaaccept.size(), 0) <= 0){
            std::cerr << "Error: Can't send to replica " << std::endl;
            std::cout << strerror(errno) << std::endl;
            return 0;
        }
        repnum++;
    }
}

// invalidate automatically sending commands from certain replica to other replicas
void* server_th::replica_accept(struct socket_st* ss){
    ss->auto_send_yes = false;
}

// automatically send commands to replicas
void* server_th::auto_send(std::string com){

    for(int i = 0; i < replica_num-1; i++){
        std::cout << "auto sending to " << i << std::endl;
        std::thread each_slave_th (server_th::auto_send_each, com, replica_sock[i]);
        each_slave_th.detach();
    }
    std::cout << "AUTO SEND DONE" << std::endl;
}

// automatically send commands to single replica
void* server_th::auto_send_each(std::string com, int fd){
    char buf [1024];
    com += "\0";
    if(send(fd, com.data(), com.size(), 0) <= 0){
        std::cerr << "Error: Sending new slave info to other slaves" << std::endl;
        std::cerr << strerror(errno) << std::endl;
        return 0;
    }
    std::cout << "-----AUTO SEND EACH------" << com << std::endl;

    memset(buf, 0x00, 1024);

    if(recv(fd, buf, 1024, 0) <= 0){
        std::cerr << "Error: Recving new slave add result to other slaves" << std::endl;
        std::cerr << strerror(errno) << std::endl;
        return 0;
    }

    std::cout << "-----AUTO RECV EACH------" << buf << std::endl;

    return 0;
}

// automatically snapshot
void* server_th::auto_save(){
    while(1){
        sleep(120);
        command::COMMAND().save(server_hash);
    }
}

// check ttl value and expire kv which satisfies ttl condition
void* server_th::ttl_check(){
    server_hash->ttl_expire_background();
}

// save command log
void* server_th::log_append(struct socket_st* ss, std::string com){
    std::mutex* l_mutex = &log_mutex;
    std::ofstream fout;
    std::string res = "";

    com += " ";
    com += char(8);
    com += " ";
    com += std::to_string(ss->auth);

    int N = 1021*1453, e = 28969;
    for(int i = 0; i < com.length(); i++)
    {
        uint64_t x = 1;
        for(int j = 0; j < e; j++)
            x = (x * (uint64_t)com[i]) % N;
        res += std::to_string(x) + "\\";
    }

    l_mutex -> lock();
    fout.open(log_name, std::ios::app);
    fout << res << std::endl;
    fout.close();
    l_mutex -> unlock();
}

// try to send command to another cluster nodes when there is fail-over
std::string server_th::try_cluster(std::string com){
    char buf [1024];
    std::string result;
    std::string magic = " ";
    magic += char(8);
    com += "\0";
    for (int i = 0; i < replica_num - 1; i++){
        if(send(replica_sock[i], com.data(), com.size(), 0) <= 0){
            std::cerr << "ERROR : trying sending to another cluster no." << i << std::endl;
            std::cerr << strerror(errno) << std::endl;
            continue;
        }
        memset(buf, 0x00, 1024);
        if(recv(replica_sock[i], buf, 1024, 0) <= 0){
            std::cerr << "ERROR: trying recving from another cluster no." << i << std::endl;
            std::cerr << strerror(errno) << std::endl;
            continue;
        }
        result = buf;
        return result;
    }
    return magic;
}

// get keys from every node
std::string server_th::all_keys(struct socket_st* ss){
    std::string key_all = ",";
    std::vector<int>::iterator iter;
    std::string keys_com = "xkeys ";
    std::string temp;
    char buf[1024];

    keys_com += char(8);
    keys_com += " ";
    keys_com += std::to_string(ss->auth);
    keys_com += "\0";

    for(iter = cluster_vec.begin(); iter != cluster_vec.end(); iter++){
        if(send(*iter, keys_com.data(), keys_com.size(), 0) <= 0){
            std::cerr << "ERROR : SENDING ALL KEYS" << std::endl;
            std::cerr << strerror(errno) << std::endl;
            return 0;
        }

        memset(buf, 0x00, 1024);

        if(recv(*iter, buf, 1024, 0) <= 0){
            std::cerr << "ERROR : RECVING ALL KEYS" << std::endl;
            std::cerr << strerror(errno) << std::endl;
            return 0;
        }

        temp = buf;
        key_all += temp;
        key_all += ",";
    }

    key_all += "\n";
    return key_all;
}

// save snapshot at every node
void* server_th::all_save(){
    std::vector<int>::iterator iter;
    std::string save_com = "xsave";
    char buf [1024];
    save_com += "\0";
    for(iter = cluster_vec.begin(); iter != cluster_vec.end(); iter++){
        if(send(*iter, save_com.data(), save_com.size(), 0) <= 0){
            std::cerr << "ERROR : SENDNING ALL SAVES" << std::endl;
            std::cerr << strerror(errno) << std::endl;
            return 0;
        }

        memset(buf, 0x00, 1024);

        if(recv(*iter, buf, 1024, 0) <= 0){
            std::cerr << "ERROR : RECVING ALL SAVES" << std::endl;
            std::cerr << strerror(errno) << std::endl;
            return 0;
        }
    }
}

bool server_th::over_client(){
    return client_num > MAX_CLIENT_NUM;
}

int server_th::get_client_num(){
    return client_num;
}

#endif
