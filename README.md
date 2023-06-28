# Online-Chat-SFML
A Simple Online Broadcast Chat (Server &amp; Multi-Client) by using SFML and Boost via C++ on UDP

## How to setup project
### install boost on linux
`sudo apt-get install libboost-all-dev`
### install sfml on linux
`sudo apt-get install libsfml-dev`
### install g++ compiler
`sudo apt-get install g++`


## How to compile and run
### by using compile-code.sh
`./compile-code.sh`
#### compile chat (SFML) by using command
`
g++ -c chat.cpp
g++ chat.o -o chat.run -std=c++11 -pthread -lboost_system -lboost_thread -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -lpthread
`
#### compile server by using command
`
g++ -c server.cpp
g++ server.o -o server.run -std=c++11 -pthread -lboost_system -lboost_thread -lpthread
`
