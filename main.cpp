#include "client.h"

int main(int argc, char **argv) {

    Client clnt { "results.txt" };

    // clnt.getGeneralInfo();

    // clnt.getRoutesInfo();

    clnt.getVehiclePositionsGTFS(false);
}