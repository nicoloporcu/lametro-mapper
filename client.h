#include <arpa/inet.h>
#include <stdio.h>
#include <string>

class HttpRequester{
    public:
        HttpRequester();
        bool init();
        bool teardown();
        bool sendGetRequest(char *request, FILE * response_message);
        bool getGeneralInfo(FILE * response_message);
        bool getRoutesInfo(FILE* response_message);
        bool getVehiclePositionsGTFS(FILE * outfile);
        bool getTripUpdatesGTFS(FILE * outfile);
        bool getAlertsGTFS(FILE * outfile);
    
    private:
        std::string serverName;
        unsigned short port;
        std::string key;
        std::string ip;
        int sock;  
};