#include <handy/handy.h>

using namespace std;
using namespace handy;

int main(int argc, const char *argv[]) {
    setloglevel("TRACE");
    EventBase base;

    //hsha模拟服务器端
    HSHAPtr hsha = HSHA::startServer(&base, "", 2099, 4);
    exitif(!hsha, "bind failed");
    Signal::signal(SIGINT, [&, hsha] {
        base.exit();
        hsha->exit();
        signal(SIGINT, SIG_DFL);
    });

    hsha->onMsg(new LineCodec, [](const TcpConnPtr &con, const string &input) {
        // get number [0-1000)
        int ms = rand() % 1000;
        info("processing a msg");
        usleep(ms * 1000); // sleep 0-999 ms
        return util::format("%s used %d ms", input.c_str(), ms);
    });

    //for 模拟客户端
    for (int i = 0; i < 5; i++) {
        TcpConnPtr con = TcpConn::createConnection(&base, "localhost", 2099);
        
        // LineCodec -- \r\n结尾的消息
        // 接收\r\n类型的到消息， 打印到日志
        con->onMsg(new LineCodec, [](const TcpConnPtr &con, Slice msg) {
            info("%.*s recved", (int) msg.size(), msg.data());
            con->close();
        });

        // tcp连接成功后， 发送消息'hello'
        con->onState([](const TcpConnPtr &con) {
            if (con->getState() == TcpConn::Connected) {
                con->sendMsg("hello");
            }
     
        });
    }


    base.runAfter(1000, [&, hsha] {
        base.exit();
        hsha->exit();
    });
    
    base.loop();
    info("program exited");
}
