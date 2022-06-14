#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "client.h"
#include <system_error>
#include <fstream>
#include <iostream>

#define SWIFT_API "api.goswift.ly"
#define PORT 80

#define die(msg) \
    throw std::system_error(errno, std::generic_category(), msg)

HttpRequester::HttpRequester():serverName(SWIFT_API), port(PORT){}

// TODO: what is the design decision behind not doing this in the constructor?
bool HttpRequester::init(){
    // GET HOST BY NAME
    struct hostent *he;
    if ((he = gethostbyname(serverName.c_str())) == NULL)
        die("gethostbyname failed");
    
    ip = inet_ntoa(*(struct in_addr *)he->h_addr);
    
    // SOCKET
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        die("socket failed");

    // CONNECT
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    servaddr.sin_port = htons(port);

    if(connect(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        die("connect failed");

    // GET KEY
    std::ifstream keyfile("key.txt", std::ios::in);
    if(!keyfile)
        die("open key.txt failed");

    std::getline(keyfile, key);
    if(keyfile.bad())
        die("get key failed");

    keyfile.close();
    
    return true;
}

bool HttpRequester::sendGetRequest(char *req, FILE *response_message){
    // SEND REQUEST
    if(send(sock, req, strlen(req), 0) != strlen(req))
        die("send HTTP request failed");

    // Wrap socket
    FILE *response;
    if((response = fdopen(sock, "r")) == NULL ) {
        die("fdopen failed");
    }

    char headers[1000];
    // print headers
    while((fgets(headers, sizeof(headers), response) !=NULL && headers[0] != '\r'))
        printf("%s", headers);

    printf("\n");

    // Write to file
    char buf[4096];
    size_t r; // limit;

    while((r = fread(buf, 1, sizeof(buf), response)) > 0 ) {
        if(r == sizeof(buf)) { // read full buf
            fwrite(buf, sizeof(buf), 1, response_message);
        } else { // read partial buf
            fwrite(buf, r, 1, response_message);
        }
    }
    return true;
}

bool HttpRequester::getGeneralInfo(FILE * outfile){
    init();
    char request[1000];
    snprintf(request, sizeof(request),
        "GET /info/lametro-rail HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n", serverName.c_str(), port, key.c_str());

    bool res = sendGetRequest(request, outfile);
    teardown();
    return res;
}

bool HttpRequester::getRoutesInfo(FILE * outfile){
    init();
    char request[1000];
    snprintf(request, sizeof(request),
        "GET /info/lametro-rail/routes HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n", serverName.c_str(), port, key.c_str());

    return sendGetRequest(request, outfile);
}

bool HttpRequester::getAlertsGTFS(FILE * outfile){
    init();
    char request[1000];
    snprintf(request, sizeof(request),
        "GET /real-time/lametro-rail/gtfs-rt-alerts HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n", serverName.c_str(), port, key.c_str());
    return sendGetRequest(request, outfile);
}

bool HttpRequester::getTripUpdatesGTFS(FILE * outfile){
    init();
    char request[1000];
    snprintf(request, sizeof(request),
        "GET /real-time/lametro-rail/gtfs-rt-trip-updates HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n", serverName.c_str(), port, key.c_str());
    return sendGetRequest(request, outfile);
}

bool HttpRequester::getVehiclePositionsGTFS(FILE * outfile){
    init();
    char request[1000];
    snprintf(request, sizeof(request),
        "GET /real-time/lametro-rail/gtfs-rt-vehicle-positions HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n", serverName.c_str(), port, key.c_str());
    return sendGetRequest(request, outfile);
}

bool HttpRequester::teardown(){
    return close(sock); 
}

int main(int argc, char **argv) {

    FILE * outfile = fopen("results.json", "w");;
    HttpRequester requester;
    requester.getGeneralInfo(outfile);
    //fprintf(outfile,"\n");
    //requester.getRoutesInfo(outfile);
    //fprintf(outfile,"\n");

    fclose(outfile);
}
