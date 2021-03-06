#include "GameResult.h"
#include "Proto.h"
#include "SqlStm.h"

#define T RESPONSE_TYPE

using namespace redbud::parser::json;
using namespace std;

GameResult::GameResult(Json in){
    account=in["account"].as_string();
    level=in["level"].as_number();
    integral=in["integral"].as_number();
}

Json
GameResult::handle(){
    string stm0="update users set level="+to_string(level)+",integral="+to_string(integral)+" where account=\""+account+"\"";
    if(SqlStm::silence(stm0)){
        out["response_type"]=int(T::UPDATE_GAMERESULT_SUCCESS);
    }else{
        out["response_type"]=int(T::UPDATE_GAMERESULT_FAILED);
    }
    return out;
}
