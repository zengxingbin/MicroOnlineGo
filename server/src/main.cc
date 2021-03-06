#include "GoServer.h"
#include "SqlManager.h"
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <pthread.h>

using namespace muduo;
using namespace muduo::net;

int main(){
    LOG_INFO<<"pid = "<<getpid();
    EventLoop loop;
    InetAddress listenAddr(60000);
    GoServer server(&loop,listenAddr);
    server.start();
    if(SqlManager::getInstance().start()==false){
        LOG_ERROR<<"failed to create mysql connection pool";
        exit(1);
    }
    loop.loop();
}
