#include "Handshake.h"
#include "Proto.h"
#include "SqlManager.h"
#include "SqlStm.h"
#include "RoomManager.h"
#include <muduo/base/Logging.h>


#define T RESPONSE_TYPE
using namespace redbud::parser::json;
using namespace std;

Handshake::Handshake(Json in){
    id=in["id"].as_number();
    nickname=in["nickname"].as_string();
    order=ORDER(int(in["order"].as_number()));
}

Json
Handshake::handle(){
    string stm0;
    if(order==ORDER::LEFT){
        stm0="update rooms set readygo1=\"yes\" where id="+to_string(id);
    }else if(order==ORDER::RIGHT){
        stm0="update rooms set readygo2=\"yes\" where id="+to_string(id);
    }
    LOG_INFO<<"Handshake: stm0: "<<stm0;
    if(SqlStm::silence(stm0)){
        string stm1="select * from rooms where id="+to_string(id);
        string stm2="select * from rooms where id="+to_string(id);
        if(SqlStm::getField(stm1,"readygo1")=="yes"&&SqlStm::getField(stm2,"readygo2")=="yes"){
            string stm3="update rooms set status=\"onbattle\" where id"+to_string(id);
            if(SqlStm::silence(stm3)){
                Json toall;
                toall["id"]=id;
                toall["nickname"]=nickname;
                toall["status"]="onbattle";
                toall["response_type"]=int(T::BROADCAST_READYGO);
            }
        }
        out["response_type"]=int(T::READYGO_SUCCESS);
        return out;
    }
    out["response_type"]=int(T::READYGO_FAILED);
    out["reason"]="inner server error";
    return out;
}
