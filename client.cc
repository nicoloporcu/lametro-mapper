#include <stdio.h>
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
//
#define SWIFT_API "api.goswift.ly"
#define PORT 80

bool HttpRequester::die(const char *s) {
    perror(s);
    return false;
}

HttpRequester::HttpRequester():serverName(SWIFT_API), port(PORT), initialized(false){}

bool HttpRequester::init(){
    // get sever ip from server name
    if((he = gethostbyname(serverName)) == NULL) {
        die("gethostbyname failed");
    }
    ip = inet_ntoa(*(struct in_addr *)he->h_addr);
    
    // Create a socket for TCP connection
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        return die("socket failed");
    } 

    // Construct a server address structure
    memset(&servaddr, 0, sizeof(servaddr)); //zero out structure
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port); //network byte order

    //Establish TCP connection to server
    if(connect(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        return die("connect failed");
    }

    //GET KEY
    FILE *keyfile = fopen("key.txt", "r");
    if(!keyfile)
        return die("fopen key.txt failed");

    if (fgets(key, sizeof(key), keyfile) == NULL)
        return die("get key failed");
    fclose(keyfile);
    
    return true;
}

bool HttpRequester::sendGetRequest(char  request_url[], FILE * response_message){
    // Send HTTP Request
    char request[1000];
    snprintf(request,sizeof(request),
        request_url, serverName, port, key);
        
    if(send(sock, &request, strlen(request), 0) != strlen(request)) {
        return die("send HTTP request failed");
    }

    // Wrap socket
    FILE *response;
    if((response = fdopen(sock, "r")) == NULL ) {
        return die("fdopen failed");
    }

    char headers[1000];
    // print headers
    while((fgets(headers, sizeof(headers), response) !=NULL && headers[0] != '\r')) {
        printf("%s", headers);
    }
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

bool HttpRequester::getGeneralInfo( FILE * outfile){
    init();
    char in_url [1000] = 
        "GET /info/lametro-rail HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n";
    return sendGetRequest(in_url, outfile);
}

bool HttpRequester::getRoutesInfo(FILE * outfile){
    init();
    char in_url [1000] = 
        "GET /info/lametro-rail/routes HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n";
    return sendGetRequest(in_url, outfile);
}

bool HttpRequester::getAlertsGTFS(FILE * outfile){
    init();
    char in_url [1000] = 
        "GET /real-time/lametro-rail/gtfs-rt-alerts HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n";
    return sendGetRequest(in_url, outfile);
}

bool HttpRequester::getTripUpdatesGTFS(FILE * outfile){
    init();
    char in_url [1000] = 
        "GET /real-time/lametro-rail/gtfs-rt-trip-updates HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n";
    return sendGetRequest(in_url, outfile);
}

bool HttpRequester::getVehiclePositionsGTFS(FILE * outfile){
    init();
    char in_url [1000] = 
        "GET /real-time/lametro-rail/gtfs-rt-vehicle-positions HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n";
    return sendGetRequest(in_url, outfile);
}

bool HttpRequester::teardown(){
    return close(sock); 
}

int main(int argc, char **argv) {

    FILE * outfile = fopen("results.json", "w");;
    HttpRequester requester;
    requester.getGeneralInfo(outfile);
    fprintf(outfile,"\n");
    requester.getRoutesInfo(outfile);
    fprintf(outfile,"\n");

    fclose(outfile);
}
