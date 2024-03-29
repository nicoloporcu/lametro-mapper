#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#undef NO_DATA
#include "client.h"
#include <cstdio>
#include <iostream>
#include <string.h>
#include <string>
#include <system_error>

#define die(msg) throw std::system_error(errno, std::generic_category(), msg)

Client::Client(const char *outfile) {
    serverName = "api.goswift.ly";
    port = 80;

    std::ifstream keyfile("key.txt", std::ios::in);
    if (!keyfile)
        die("open key.txt failed");

    std::getline(keyfile, key);
    if (keyfile.bad())
        die("get key failed");

    keyfile.close();

    file.open(outfile, std::ios::binary | std::ios::trunc);
    if (!file)
        die("error: ofstream.open failed");
}

bool Client::getGeneralInfo() {
    init();
    char request[1000];
    snprintf(request, sizeof(request),
             "GET /info/lametro-rail HTTP/1.0\r\n"
             "Host: %s:%d\r\n"
             "Authorization: %s\r\n"
             "Content-Type: application/json\r\n"
             "\r\n",
             serverName.c_str(), port, key.c_str());
    bool res = get(request);
    teardown();
    return res;
}

bool Client::getRoutesInfo() {
    init();
    char request[1000];
    snprintf(request, sizeof(request),
             "GET /info/lametro-rail/routes HTTP/1.0\r\n"
             "Host: %s:%d\r\n"
             "Authorization: %s\r\n"
             "Content-Type: application/json\r\n"
             "\r\n",
             serverName.c_str(), port, key.c_str());
    bool res = get(request);
    teardown();
    return res;
}

bool Client::getAlertsGTFS(bool human, transit_realtime::FeedMessage *message) {
    init();
    const char *format = human ? "?format=human" : "";
    char request[1000];
    snprintf(request, sizeof(request),
             "GET /real-time/lametro-rail/gtfs-rt-alerts%s HTTP/1.0\r\n"
             "Host: %s:%d\r\n"
             "Authorization: %s\r\n"
             "Content-Type: application/json\r\n"
             "\r\n",
             format, serverName.c_str(), port, key.c_str());
    bool res = getProto(request, message);
    teardown();
    return res;
}

bool Client::getTripUpdatesGTFS(bool human, transit_realtime::FeedMessage *message) {
    init();
    char request[1000];
    const char *format = human ? "?format=human" : "";
    snprintf(request, sizeof(request),
             "GET /real-time/lametro-rail/gtfs-rt-trip-updates%s HTTP/1.0\r\n"
             "Host: %s:%d\r\n"
             "Authorization: %s\r\n"
             "Content-Type: application/json\r\n"
             "\r\n",
             format, serverName.c_str(), port, key.c_str());
    std::cout << request << std::endl;
    bool res = getProto(request, message);
    teardown();
    return res;
}

bool Client::getVehiclePositionsGTFS(bool human, transit_realtime::FeedMessage *message) {
    init();
    char request[1000];
    const char *format = human ? "?format=human" : "";
    snprintf(request, sizeof(request),
             "GET /real-time/lametro-rail/gtfs-rt-vehicle-positions%s HTTP/1.0\r\n"
             "Host: %s:%d\r\n"
             "Authorization: %s\r\n"
             "Content-Type: application/json\r\n"
             "\r\n",
             format, serverName.c_str(), port, key.c_str());
    bool res = getProto(request, message);
    teardown();
    return res;
}

///////////// PRIVATE METHODS //////////////

bool Client::init() {
    // GET HOST BY NAME
    struct hostent *he;
    if ((he = gethostbyname(serverName.c_str())) == NULL)
        die("gethostbyname failed");
    char *ip = inet_ntoa(*(struct in_addr *)he->h_addr);

    // SOCKET
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        die("socket failed");

    // CONNECT
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        die("connect failed");

    return true;
}

bool Client::get(char *request) {

    // SEND REQUEST
    if (send(sock, request, strlen(request), 0) != strlen(request))
        die("send HTTP request failed");

    // Wrap socket
    FILE *fp = fdopen(sock, "r");
    if (!fp)
        die("fdopen failed");

    // READ HEADERS
    char buffer[1000];
    while (fgets(buffer, sizeof(buffer), fp) != NULL && buffer[0] != '\r') {
        std::cout << buffer;
    }
    std::cout << std::endl;

    // WRITE RESPONSE TO FILE
    uint32_t siz = 4000;
    char buf[siz];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        file.write(buf, n);
    }
    return true;
}

bool Client::getProto(char *request, transit_realtime::FeedMessage *message) {
    // SEND REQUEST
    if (send(sock, request, strlen(request), 0) != strlen(request))
        die("send HTTP request failed");

    // Wrap socket
    FILE *fp = fdopen(sock, "r");
    if (!fp)
        die("fdopen failed");

    // READ HEADERS
    char buffer[1000];
    while (fgets(buffer, sizeof(buffer), fp) != NULL && buffer[0] != '\r') {
        std::cout << buffer;
    }
    std::cout << std::endl;

    uint32_t size = 4096;
    char buf[size];
    size_t n = 0;
    std::string s;

    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        s.append(std::string(buf, n));
    }

    message->ParseFromString(s);
    return true;
}

bool Client::teardown() { return close(sock); }
