#include "gtfs-realtime.pb.h"
#include <map>

enum RouteId{
    INVALID = 0,
    A_LINE = 801,
    B_LINE = 802,
    C_LINE = 803,
    D_LINE = 804,
    E_LINE = 805,
    K_LINE = 807
};
 


class ProtoParser
{

    public:
        ProtoParser();
        int getLastTimestamp();
        void updateProtobuf();
        void UpdateMessage(transit_realtime::FeedMessage * msg);
        int GetTimestamp();

        void ParseLocations();
        void ParseVehiclePositions();

        //getVehicles();
    private:
        bool has_message;
        transit_realtime::FeedMessage * message;
        uint64_t timestamp;

        std::map<RouteId, std::string> route_names = {
            {INVALID, "Invalid"},
            {A_LINE, "A Line"},
            {B_LINE, "B Line"},
            {C_LINE, "C Line"},
            {D_LINE, "D Line"},
            {E_LINE, "E Line"},
            {K_LINE, "K Line"}
        };

        std::map<transit_realtime::VehiclePosition_VehicleStopStatus, std::string> status_str = {
            {transit_realtime::VehiclePosition_VehicleStopStatus::VehiclePosition_VehicleStopStatus_IN_TRANSIT_TO, "in transit to"},
            {transit_realtime::VehiclePosition_VehicleStopStatus::VehiclePosition_VehicleStopStatus_INCOMING_AT, "incoming at"},
            {transit_realtime::VehiclePosition_VehicleStopStatus::VehiclePosition_VehicleStopStatus_STOPPED_AT, "stopped at"}
        };
    /* data */

};

