#include "gtfs-realtime.pb.h"

class ProtoParser
{

    public:
        ProtoParser();
        int getLastTimestamp();
        void updateProtobuf();
        void UpdateMessage(transit_realtime::FeedMessage * msg);
        int GetTimestamp();

        //getVehicles();
    private:
        bool has_message;
        transit_realtime::FeedMessage * message;
    /* data */

};

