#include <string>
#include <fstream>

class Client{
    public:
        Client(const char *outfile);
        ~Client() = default;
        bool getGeneralInfo();
        bool getRoutesInfo();
        bool getVehiclePositionsGTFS();
        bool getTripUpdatesGTFS();
        bool getAlertsGTFS();
    
    private:
        std::string serverName;
        unsigned short port;
        std::ofstream file;
        std::string key;
        int sock;

        bool get(char *request);
        bool init();
        bool teardown();
};