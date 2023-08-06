#include "proto_parser.h"

ProtoParser::ProtoParser(): has_message(false){}


//Todo(nicolo) maybe add message queue to keep track of n latest messages
//this may provide more resiliancy for data consistency.
void ProtoParser::UpdateMessage(transit_realtime::FeedMessage * msg){
    message = msg;
    has_message = true;
    timestamp = (message->header()).timestamp();
    std::cout << "Message updated at time " << timestamp << std::endl;
}

//Gets the Timestamp of the latest Data
int ProtoParser::GetTimestamp(){
    if(!has_message){
        perror("Proto Parser Has not gathered data yet");
        return -1;
    }
    return 0;

}

std::pair<int,int> getTime(int64_t & sec){
    auto val = std::make_pair(sec/60, sec % 60 );
    return val;
}


void ProtoParser::ParseLocations(){

    message -> PrintDebugString();

    int size = message->entity_size();

    for(int i =0; i < size; ++i){
        auto & entity = message -> entity()[i];

        if(entity.trip_update().trip().has_route_id()){
            int route_id = std::stoi(entity.trip_update().trip().route_id());
            int direction_id = entity.trip_update().trip().direction_id();

            if(entity.trip_update().stop_time_update().size() > 0){
                for(int32_t i = 0; i < entity.trip_update().stop_time_update_size(); ++i){
                    if(!entity.trip_update().stop_time_update()[i].has_stop_id()){
                        continue;
                    }
                    int stop_id = std::stoi(entity.trip_update().stop_time_update()[i].stop_id());
                    if(entity.trip_update().stop_time_update()[i].has_arrival()){
                        uint64_t arrival_time = entity.trip_update().stop_time_update()[i].arrival().time();
                        int64_t time_diff = arrival_time -timestamp;
                        if(time_diff< 0){
                            continue;
                        }
                        std::cout << route_names[RouteId(route_id)] << " train with direction " << direction_id << 
                            " next stop is " << stop_id << ". Arriving in " << getTime(time_diff).first << " minutes and "
                            << getTime(time_diff).second << " seconds" << std::endl;
                            break;
                    }
                }
            }
            else{
                std::cout << "Train " << route_id << " has no posted stops " << std::endl;
            }
            
            
        }
    }
}


void ProtoParser::ParseVehiclePositions(){
    int size = message -> entity_size();
    std::cout << size << " vehicles found." << std::endl;
    for(int i =0; i < size; ++i){

        auto & entity =  message -> entity()[i];
        if(!entity.has_vehicle()){
            //throw std::invalid_argument("Invalid request type. Should be Vehicle Position Request");
            //We are trying to parse the wrong type of message, should abort
            continue;
        }
        if(!entity.vehicle().has_trip()){
            //Choo choo is sleeping
            continue;
        }
        if(!entity.vehicle().has_stop_id() || !entity.vehicle().has_current_status()){
            // We don't have all the info we need to add the train
            continue;
        }

        RouteId route_id = RouteId(stoi(entity.vehicle().trip().route_id()));
        int stop_id = std::stoi(entity.vehicle().stop_id());
        transit_realtime::VehiclePosition_VehicleStopStatus  status = entity.vehicle().current_status();
        
        std::cout << route_names[RouteId(route_id)] << " train " << status_str[status] << " "<< stop_id << std::endl;
    }

}


