CC = g++
CXX = g++

CXXFLAGS = -g -Wall -std=c++11 
LDLIBS = -lprotobuf

.PHONY: all
all: main generate_proto


main: client.o gtfs-realtime.pb.o proto_parser.o

main.o: client.h gtfs-realtime.pb.h proto_parser.h

client.o: 

gtfs-realtime.pb.o: 

proto_parser.o:

.PHONY: clean
clean:
	rm -f *.o main

.PHONY: generate_proto
generate_proto: 
	protoc --proto_path=gtfs-realtime.proto --cpp_out=.