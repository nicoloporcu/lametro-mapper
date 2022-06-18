CC = g++
CXX = g++

CXXFLAGS = -g -Wall -std=c++11

main: client.o gtfs-realtime.pb.o 

main.o: client.h gtfs-realtime.pb.h

client.o: 

gtfs-realtime.pb.o: 

.PHONY: clean
clean:
	rm -f *.o main