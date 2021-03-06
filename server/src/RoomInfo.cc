#include "RoomInfo.h"
#include "Proto.h"
#include "SqlManager.h"
#include "SqlStm.h"
#include "RoomManager.h"
#include <zdb/zdb.h>
#include <zdb/Exception.h>
#include <zdb/Connection.h>

#define T RESPONSE_TYPE
using namespace redbud::parser::json;

RoomInfo::RoomInfo(Json in){
}

Json
RoomInfo::handle(){
    std::string stm0="select count(*) from rooms";
    Connection_T sqlConn=SqlManager::getInstance().getConn();
    ResultSet_T r0=Connection_executeQuery(sqlConn,stm0.c_str());
    if(ResultSet_next(r0)){
        out["rooms_num"]=ResultSet_getInt(r0,1);
    }
    std::string stm1="select * from rooms order by id";
    ResultSet_T r1=Connection_executeQuery(sqlConn,stm1.c_str());
    Json rlist;
    while(ResultSet_next(r1)){
        rlist.push_back(Json::Object{
                        {"id",ResultSet_getStringByName(r1,"id")},
                        {"name",ResultSet_getStringByName(r1,"name")},
                        {"player1",ResultSet_getStringByName(r1,"player1")},
                        {"player2",ResultSet_getStringByName(r1,"player2")},
                        {"readygo1",ResultSet_getStringByName(r1,"readygo1")},
                        {"readygo2",ResultSet_getStringByName(r1,"readygo2")},
                        {"status",ResultSet_getStringByName(r1,"status")}
                        }
                        );
    }
    if(rlist.size()!=0){
        out["rooms_list"]=rlist;
    }
    out["response_type"]=int(T::FETCH_ROOM_INFO_SUCCESS);
    SqlManager::getInstance().putConn(sqlConn);
    return out;
}
