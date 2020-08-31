#include <handy/handy.h>

using namespace std;
using namespace handy;

int main(int argc, const char *argv[]) {
    setloglevel("TRACE");

    EventBase base;
    Signal::signal(SIGINT, [&] { base.exit(); });

    // connect to port 2099, and timeout 3000ms
    TcpConnPtr con = TcpConn::createConnection(&base, "127.0.0.1", 2099, 3000);

    // set reconnect interval 3000ms
    // 保证server在先启动， cli后启动也能执行到connect代码
    con->setReconnectInterval(3000);

    //接收到消息后打印到日志
    con->onMsg(new LengthCodec, [](const TcpConnPtr &con, Slice msg) { info("recv msg: %.*s", (int) msg.size(), msg.data()); });

    //set callback onState
    con->onState([=](const TcpConnPtr &con) {
        info("onState called state: %d", con->getState());
        if (con->getState() == TcpConn::Connected) {
            //成功连接后发送hello消息
            con->sendMsg("hello");
        }
    });

    //dispatch loop, connect to 
    base.loop();
    
    info("program exited");
}
