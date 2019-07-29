# MUSE
MUSE: MUltithreading SEcure In-Memory Key-Value Store

### What is MUSE?

![logo](readme_src/MUSE_LOGO.png "MUSE")

MUSE[múse] is a in-memory key-value store system. MUSE can be referred as a key-value server system. MUSE stores, updates and retrieve data via a set of commands which are sent on a server-client model with TCP socket. 

MUSE has some key properties which are distinguishable from other key-value store system.

* MUSE provides multithreading. Server can serve multiple commands from multiple clients simultaneously by multithreading. This makes MUSE faster when there are many clients attached to the server.
* MUSE considers persistency and safety of system. While all the commands and values are handled in main memory of the system, MUSE stores the current status of key-value store(Snapshot) and log of all the commands which changes the status. In order to provides scalable and secure system, MUSE forks extra threads which do such jobs and encrypt all the data.
* MUSE supports different kind of value: Strings, Lists, Sets, Hashmap. MUSE stores the value not in raw form, instead it abstracts the value into single form which makes the system scalable and easy to handle.
* MUSE can offer different level of authority to each client and each key. If the authority condition of clients and requested key doesn't match the condition, MUSE blocks the access of the key. By differentiating the authority level, user can handle the database far easier especially for the system which includes large number of clients.
* MUSE is server for distributed system. It supports cluster and replication methods. Cluster provides a way to automatically share across multiple MUSE server. Replication of each server nodes help to handle when one of the node fail-over.

### How to install and use?





