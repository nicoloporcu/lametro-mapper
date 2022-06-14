CC = g++
CXX = g++

CXXFLAGS = -g -Wall -std=c++20

main: client.o

main.o: client.h

client.o: 

.PHONY: clean
clean:
	rm -f *.o main