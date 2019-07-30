# MUSE
MUSE: MUltithreading SEcure In-Memory Key-Value Store

### What is MUSE?

![logo](readme_src/MUSE_LOGO.png "MUSE")

MUSE[múse] is a in-memory key-value store system. MUSE works as a server system with database. MUSE stores, updates and retrieve data via a set of commands which are sent on a server-client model with TCP socket. 
(By the way, the logo stands for aengMUSE(=parrot in korean) and shields(to demonstrate that it is very secure!)
MUSE has some key properties which are distinguishable from other key-value store system.

* MUSE provides multithreading. Server can serve multiple commands from multiple clients simultaneously by multithreading. This makes MUSE faster when there are many clients attached to the server.
* MUSE considers persistency and safety of system. While all the commands and values are handled in main memory of the system, MUSE stores the current status of key-value store(Snapshot) and log of all the commands which changes the status. In order to provides scalable and secure system, MUSE forks extra threads which do such jobs and encrypt all the data.
* MUSE supports different kind of value: Strings, Lists, Sets, Hashmap. MUSE doesn't store the value in raw form, instead it abstracts the value into single form which makes the system scalable and easy to handle.
* MUSE can offer different level of authority to each client and each key. If the authority condition of clients and requested key doesn't match the condition, MUSE blocks the access of the key. By differentiating the authority level, user can handle the database far easier especially for the system which includes large number of clients.
* MUSE is server for distributed system. It supports cluster and replication methods. Cluster provides a way to automatically share across multiple MUSE server. Replication of each server nodes help to handle when one of the node fail-over.

### How to compile?

MUSE uses thread and mutex library in C++11. Therefore, it must be complied and used where such library's are available.
Until now, MUSE works best on Linux and OSX.(On OSX, there can be some warning message when you compile, but you can ignore)

To begin, first you clone the repository and compile using:

    $ bash compile.sh

When you compile, 5 objects will be created: muse-server, muse-client, muse-cluster, muse-replica, muse-restore.

### How to run?

To run the server, you can just simply type:

    $ ./muse-server [PORT NUMBER]

For the client, there are two options you can choose. In the most simple way, you can just:

    $ ./muse-client [IP ADDRESS] [PORT NUMBER]

In this way, simple MUSE clients will be open and connected to the MUSE server which matches with the argument(IP adddress and port number). In this case, authority level is 0 which is the lowest level.
To give authority level, you can give level as last argument:

    $ ./muse-client [IP ADDRESS] [PORT NUMBER] [AUTHORITY LEVEL]
    
To build cluster, first, you must run the servers to include in server. For instance, if you want to make a cluster of 3 server nodes, you need to run 3 servers to make cluster first. After you run the server, you can run muse-cluster with configuration text file:

    $ ./muse-cluster [CONFIGURATION TXT]

Configuration text file should be written in given forms:

    cluster [CLUSTER NUMBER]
    [IP ADDRESS 1] [PORT NUMBER 1]
    [IP ADDRESS 2] [PORT NUMBER 3]
    ...

For example, if you want to make a cluster with 3 server nodes:

    cluster 3
    127.0.0.1 5001
    127.0.0.1 5004
    127.0.0.1 5007

Building replication is similar to building cluster. You must run the servers to make replication of each others, and then you run muse-replica with configuration text file.


    $ ./muse-replica [CONFIGURATION TXT]

Like cluster, configuration text file should be written in given forms:

    replicanumber [CLUSTER NUMBER]
    replica [IP ADDRESS 1] [PORT NUMBER 1]
    replica [IP ADDRESS 2] [PORT NUMBER 3]
    ...

For example, if you want to make a replication with 3 server nodes:

    replicanumber 3
    replica 127.0.0.1 5001
    replica 127.0.0.1 5002
    replica 127.0.0.1 5003

In order to make a distributed system with both cluster and replication, you must build replication first and then cluster:

    $ ./muse-replica [CONFIGURATION TXT]
    $ ./muse-cluster [CONFIGURATION TXT]

