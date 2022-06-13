#include <arpa/inet.h>
#include <stdio.h>

class HttpRequester{
    public:
        HttpRequester();
        bool init();
        bool teardown();
        bool sendGetRequest(char * request, FILE * response_message);
        bool getGeneralInfo(FILE * response_message);
        bool getRoutesInfo(FILE* response_message);
        bool die(const char *s);
    
    private:
        struct hostent *he;
        char serverName[100];
        char *ip;
        unsigned short  port;
        bool initialized;
        int sock;
        char key[100];
        struct sockaddr_in servaddr;
        
};