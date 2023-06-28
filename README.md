# Online-Chat-SFML
A Simple Online Broadcast Chat (Server &amp; Multi-Client) by using SFML and Boost via C++ on UDP


## some note:
this project is on local so didnt do anything to save/map clients ip, client ips are same 127.0.0.1

after client lunched application have to send a message to server,
then server add them to chat, if client didnt send any message at first time to the server,
client will receive nothing from server. 

message buffer is 1024 charecter and there is no limit or filter to check that.


# screenshot:
<img src="" width=100% title=""/>



# How to setup project
## install some library and tools
### install boost on linux
`sudo apt-get install libboost-all-dev`
### install sfml on linux
`sudo apt-get install libsfml-dev`
### install g++ compiler
`sudo apt-get install g++`

## project assests:
### use a .ttf font or use current arial.ttf then import inside projects folder


# How to compile and run
## by using script (compile-code.sh)
`./compile-code.sh`

## manual compile:
#### to compile chat (client) by using command
`
g++ -c chat.cpp
g++ chat.o -o chat.run -std=c++11 -pthread -lboost_system -lboost_thread -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -lpthread
`
#### to compile server by using command
`
g++ -c server.cpp
g++ server.o -o server.run -std=c++11 -pthread -lboost_system -lboost_thread -lpthread
`


## How execute project:
`
chmod +x server.run
chmod +x chat.run
`
### run server: (after lunch, you have to enter a main port for server)
`./server.run`

### run client:
`./chat.run`
