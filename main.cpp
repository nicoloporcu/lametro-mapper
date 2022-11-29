#include "client.h"
#include "proto_parser.h"

int main(int argc, char **argv) {

    Client clnt { "results.txt" };
    ProtoParser parser;
    // clnt.getGeneralInfo();
    // clnt.getRoutesInfo();

    transit_realtime::FeedMessage message;

    clnt.getVehiclePositionsGTFS(false, &message);

    std::cout<<"Message is "<<message.DebugString();

}