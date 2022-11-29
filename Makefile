CC = g++
CXX = g++

CXXFLAGS = -g -Wall -std=c++11 
LDLIBS = -lprotobuf

main: client.o gtfs-realtime.pb.o proto_parser.o

main.o: client.h gtfs-realtime.pb.h proto_parser.h

client.o: 

gtfs-realtime.pb.o: 

proto_parser.o:

.PHONY: clean
clean:
	rm -f *.o main