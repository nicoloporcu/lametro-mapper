#include <string>
#include <fstream>
#include "gtfs-realtime.pb.h"

class Client{
    public:
        Client(const char *outfile);
        ~Client() = default;
        bool getGeneralInfo();
        bool getRoutesInfo();
        bool getVehiclePositionsGTFS(bool human, transit_realtime::FeedMessage * message);
        bool getTripUpdatesGTFS(bool human, transit_realtime::FeedMessage * message);
        bool getAlertsGTFS(bool human, transit_realtime::FeedMessage * message);
    
    private:
        std::string serverName;
        unsigned short port;
        std::ofstream file;
        std::string key;
        int sock;

        bool get(char *request);
        bool getProto(char *request, transit_realtime::FeedMessage * message);
        bool init();
        bool teardown();
};