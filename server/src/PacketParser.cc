#include "PacketParser.h"
#include "Proto.h"
#include "Account.h"
#include "Nickname.h"
#include "Login.h"
#include "Logout.h"
#include "Regist.h"
#include "RoomManager.h"
#include "PairManager.h"
#include "LobbyInfo.h"
#include "RoomInfo.h"
#include "PlayerInfo.h"
#include "PlayersInfo.h"
#include "GroupChat.h"
#include "NewRoom.h"
#include "NewRival.h"
#include  "Handshake.h"
#include "ChessInfo.h"
#include "SinglecastMsg.h"
#include "GameResult.h"
#include <redbud/parser/json_parser.h>
#define T REQUEST_TYPE

using namespace redbud::parser::json;
using namespace std;

void
PacketParser::encode(){
    auto str=out.dumps();
    sendBack(conn,str);
}

void
PacketParser::decode(){
    in=Json::parse(msg);
}

void
PacketParser::dispatch(){
    decode();
    switch(T(int(in["request_type"].as_number()))){
        case T::ACCOUNT_CHECK:
        {
            Account account(in);
            out=account.handle();
        }
        break;
        case T::NICKNAME_CHECK:
        {
            Nickname nickname(in);
            out=nickname.handle();
        }
        break;
        case T::LOGIN:
        {
            Login login(in);
            out=login.handle();
            RoomManager::getInstance().add(conn,0);
        }
        break;
        case T::REGIST:
        {
            Regist regist(in);
            out=regist.handle();
        }
        break;
        case T::LOGOUT:
        {
            Logout logout(in);
            out=logout.handle();
            RoomManager::getInstance().remove(conn,0);
        }
        break;
        case T::FETCH_PLAYER_INFO:
        {
            PlayerInfo playerInfo(in);
            out=playerInfo.handle();
        }
        break;
        case T::FETCH_PLAYERS_INFO:
        {
            PlayersInfo playersInfo(in);
            out=playersInfo.handle();
        }
        break;
        case T::FETCH_LOBBY_INFO:
        {
            LobbyInfo lobbyInfo(in);
            out=lobbyInfo.handle();
        }
        break;
        case T::FETCH_ROOM_INFO:
        {
            RoomInfo roomInfo(in);
            out=roomInfo.handle();
        }
        break;
        case T::GROUP_CHAT:
        {
            GroupChat groupChat(in);
            out=groupChat.handle();
        }
        break;
        case T::SITDOWN:
        {
            ORDER order=PairManager::getInstance().add(conn,in["id"].as_number());
            in["order"]=int(order);
            if(int(in["id"].as_number())==0){
                in["id"]=conn->localAddress().toPort();
                NewRoom newRoom(in);
                out=newRoom.handle();
            }else{
                NewRival newRival(in);
                out=newRival.enter();
            }
            PairManager::getInstance().add(conn,in["id"].as_number());
        }
        break;
        case T::LEAVE:
        {
            ORDER order=PairManager::getInstance().pos(conn,in["id"].as_number());
            in["order"]=int(order);
            NewRival newRival(in);
            out=newRival.leave();
            PairManager::getInstance().remove(conn,in["id"].as_number());
        }
        break;
        case T::READYGO:
        {
            ORDER order=PairManager::getInstance().pos(conn,in["id"].as_number());
            in["order"]=int(order);
            Handshake handShake(in);
            out=handShake.handle();
            Json torival;
            torival["response_type"]=int(RESPONSE_TYPE::SINGLECAST_READYGO);
            torival["id"]=in["id"];
            torival["nickname"]=in["nickname"];
            PairManager::getInstance().singlecast(conn,in["id"].as_number(),torival.dumps());
        }
        break;
        case T::PLACECHESS:
        {
            in.erase("request_type");
            Json torival=in;
            PairManager::getInstance().singlecast(conn,in["id"].as_number(),torival.dumps());
            ChessInfo chessInfo(in);
            out=chessInfo.handle();
        }
        break;
        case T::UPDATE_GAMERESULT:
        {
            //originally the protos are GAMEOVER_WINNER/GAMEOVER_LOSER/GIVEUP
            GameResult gameResult(in);
            out=gameResult.handle();
            Json toall;
            in.erase("request_type");
            toall=in;
            in["response_type"]=int(RESPONSE_TYPE::BROADCAST_GAMERESULT_UPDATE);
            RoomManager::getInstance().broadcast(0,toall.dumps());
        }
        break;
        case T::SEND_MSG:
        {
            SinglecastMsg singlecastMsg(in);
            out=singlecastMsg.handle();
            Json torival;
            torival["nickname"]=out["nickname"];
            torival["msg"]=out["msg"];
            torival["response_type"]=int(RESPONSE_TYPE::SINGLECAST_CHAT);
            PairManager::getInstance().singlecast(conn,in["id"].as_number(),torival.dumps());
        }
        break;
    }
    encode();
}
