#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <cstdio>

#include <system_error>
#include <iostream>
#include <format>
#include "client.h"

#define die(msg) \
    throw std::system_error(errno, std::generic_category(), msg)

Client::Client(const char *outfile){
    serverName = "api.goswift.ly";
    port = 80;

    std::ifstream keyfile("key.txt", 0 );
    if(!keyfile)
        die("open key.txt failed");

    std::getline(keyfile, key);
    if(keyfile.bad())
        die("get key failed");

    keyfile.close();

    file.open(outfile, std::ios::binary | std::ios::trunc);
    if (!file)
        die("error: ofstream.open failed");
}

bool Client::getGeneralInfo(){
    init();
    char request[1000];
    snprintf(request, sizeof(request),
        "GET /info/lametro-rail HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n", serverName.c_str(), port, key.c_str());
    bool res = get(request);
    teardown();
    return res;
}

bool Client::getRoutesInfo(){
    init();
    char request[1000];
    snprintf(request, sizeof(request),
        "GET /info/lametro-rail/routes HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n", serverName.c_str(), port, key.c_str());

    return get(request);
}

bool Client::getAlertsGTFS(){
    init();
    char request[1000];
    snprintf(request, sizeof(request),
        "GET /real-time/lametro-rail/gtfs-rt-alerts HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n", serverName.c_str(), port, key.c_str());
    return get(request);
}

bool Client::getTripUpdatesGTFS(){
    init();
    char request[1000];
    snprintf(request, sizeof(request),
        "GET /real-time/lametro-rail/gtfs-rt-trip-updates HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n", serverName.c_str(), port, key.c_str());
    return get(request);
}

bool Client::getVehiclePositionsGTFS(){
    init();
    char request[1000];
    snprintf(request, sizeof(request),
        "GET /real-time/lametro-rail/gtfs-rt-vehicle-positions HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n", serverName.c_str(), port, key.c_str());
    return get(request);
}

///////////// PRIVATE METHODS //////////////

bool Client::init(){
    // GET HOST BY NAME
    struct hostent *he;
    if ((he = gethostbyname(serverName.c_str())) == NULL)
        die("gethostbyname failed");
    
    // SOCKET
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        die("socket failed");

    // CONNECT
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = (*(struct in_addr *)he->h_addr).s_addr;
    servaddr.sin_port = htons(port);

    if(connect(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        die("connect failed");
    
    return true;
}


bool Client::get(char *request){
    // SEND REQUEST
    if(send(sock, request, strlen(request), 0) != strlen(request))
        die("send HTTP request failed");

    // Wrap socket
    FILE *fp = fdopen(sock, "r");
    if (!fp)
        die("fdopen failed");

    // READ HEADERS
    char buffer[1000];
    while(fgets(buffer, sizeof(buffer), fp) != NULL && buffer[0] != '\r'){
        std::cout << buffer;
    }
    std::cout << std::endl;

    // WRITE RESPONSE TO FILE
    int n;
    while((n = fread(buffer, 1, sizeof(buffer), fp) > 0)){
        file.write(buffer, n);
    }
    file << std::endl;

    return true;
}

bool Client::teardown(){
    return close(sock); 
}
