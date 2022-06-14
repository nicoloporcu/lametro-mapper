CC = g++
CXX = g++

CXXFLAGS = -g -Wall

main: client.o

main.o: client.h

client.o: 

.PHONY: clean
clean:
	rm -f *.o main