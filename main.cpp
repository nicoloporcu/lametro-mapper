#include "client.h"
#include "proto_parser.h"

int main(int argc, char **argv) {

    Client clnt{"results.txt"};
    ProtoParser parser;
    clnt.getGeneralInfo();
    clnt.getRoutesInfo();

    transit_realtime::FeedMessage message;
    transit_realtime::FeedMessage routes;

    clnt.getVehiclePositionsGTFS(false, &message);
    // clnt.getTripUpdatesGTFS(false, &message);
    // clnt.getRoutesInfo();

    std::cout << "Message is " << message.DebugString();

    parser.UpdateMessage(&message);

    parser.ParseVehiclePositions();

    // parser.ParseLocations();
}