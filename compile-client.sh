g++ -c chat.cpp specific-messages.h
g++ chat.o -o chat.run -std=c++11 -pthread -lboost_system -lboost_thread -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -lpthread
rm *.o
rm *.gch
