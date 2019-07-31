g++ -std=c++11 -o muse-cluster src/cluster.cpp -lpthread
g++ -std=c++11 -o muse-server src/server.cpp -lpthread
g++ -std=c++11 -o muse-client src/client.cpp -lpthread
g++ -std=c++11 -o muse-replica src/replica.cpp -lpthread
g++ -std=c++11 -o muse-recover src/recover.cpp -lpthread
g++ -std=c++11 -o muse-dist src/distribute.cpp -lpthread
