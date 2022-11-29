#include "proto_parser.h"

ProtoParser::ProtoParser(): has_message(false){}


//Todo(nicolo) maybe add message queue to keep track of n latest messages
//this may provide more resiliancy for data consistency.
void ProtoParser::UpdateMessage(transit_realtime::FeedMessage * msg){
    message = msg;
    has_message = true;
}

//Gets the Timestamp of the latest Data
int ProtoParser::GetTimestamp(){
    if(!has_message){
        perror("Proto Parser Has not gathered data yet");
        return -1;
    }
    return 0;

}


