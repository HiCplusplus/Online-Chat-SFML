g++ -c server.cpp specific-messages.h
g++ server.o -o server.run -std=c++11 -pthread -lboost_system -lboost_thread -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -lpthread
rm *.o
