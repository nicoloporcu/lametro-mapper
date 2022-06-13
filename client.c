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

static void die(const char *s) {
    perror(s);
    exit(1);
}

int main(int argc, char **argv) {
    struct hostent *he;
    char *serverName = "api.goswift.ly";
    unsigned short  port = 80;

    // get sever ip from server name
    if((he = gethostbyname(serverName)) == NULL) {
        die("gethostbyname failed");
    }
    char *ip = inet_ntoa(*(struct in_addr *)he->h_addr);
    
    // Create a socket for TCP connection
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        die("socket failed");
    } //mak change

    // Construct a server address structure
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr)); //zero out structure
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port); //network byte order

    //Establish TCP connection to server
    if(connect(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        die("connect failed");
    }
   
    char key[100];
    FILE *fp = fopen("key.txt", "r");
    if(!fp)
        die("fopen key.txt failed");

    if (fgets(key, sizeof(key), fp) <= 0)
        die("get key failed");

    // Send HTTP Request
    char request[1000];
    snprintf(request,sizeof(request),
        "GET /real-time/lametro-rail/gtfs-rt-trip-updates?format=human HTTP/1.0\r\n"
        "Host: %s:%d\r\n"
        "Authorization: %s\r\n"
        "Content-Type: application/json\r\n"
        "\r\n", serverName, port, key);

    if(send(sock, &request, strlen(request), 0) != strlen(request)) {
        die("send HTTP request failed");
    }
    
    // Wrap socket
    FILE *response;
    if((response = fdopen(sock, "r")) == NULL ) {
        die("fdopen failed");
    }
    
    // Read response
    char rsp[1000];

    // print headers
    while((fgets(rsp, sizeof(rsp), response) > 0 && rsp[0] != '\r')) {
        printf("%s", rsp);
    }
    printf("\n");

    FILE *data = fopen("results.json", "w");

    // Write to file
    char buf[4096];
    size_t r; //, limit;
    while((r = fread(buf, 1, sizeof(buf), response)) > 0 ) {
        if(r == sizeof(buf)) { //read full buf
            fwrite(buf, sizeof(buf), 1, data);
        } else { //read partial buf
            fwrite(buf, r, 1, data);
        }
    }

    if(ferror(response)) {
        die("fread failed");
    }
    if(ferror(data)) {
        die("fwrite failed");
    }

    fclose(data);
    fclose(fp);
    fclose(response);
}
