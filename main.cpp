#include "client.h"

int main(int argc, char **argv) {

    Client clnt { "results.json" };

    // clnt.getGeneralInfo();

    // clnt.getRoutesInfo();

    clnt.getTripUpdatesGTFS(true);
}
