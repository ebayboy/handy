#include <handy/handy.h>

using namespace std;
using namespace handy;


/* 处理流程：
1. server 监听 2099
2. client 连接 2099
3. server accept 连接socket，创建TcpConn, 绑定TcpConn到scoket上，设置onMsg回调函数
4. 客户端连接成功后使用mBdT：发送hello
5. 服务端接收到msg后，解析mbdt， 打印消息， 并将消息返回给客户端
*/

//mBdT格式： 前4字节报文长度， 后面是消息内容;   length(4byte)data;

int main(int argc, const char *argv[]) {
    Logger::getLogger().setLogLevel(Logger::LTRACE);
    EventBase base;
    Signal::signal(SIGINT, [&] { base.exit(); });

    TcpServerPtr echo = TcpServer::startServer(&base, "", 2099);
    exitif(echo == NULL, "start tcp server failed");

    // onConnState -- 连接状态发生变更时回调statecb_
    // onConnCreate -- accept socket后，调用回调函数createcb_创建TcpConn， 之后conn.cc处理函数会将TcpConn绑定到socket上
    echo->onConnCreate([] {
        //客户端连接创建时， 创建新的tcp， 设定回调函数
        TcpConnPtr con(new TcpConn);

        // LengthCodec:  给出了消息长度的消息
        con->onMsg(new LengthCodec, [](const TcpConnPtr &con, Slice msg) {
            info("recv msg: %.*s", (int) msg.size(), msg.data());
            con->sendMsg(msg);
        });

        //返回的con会attach到base. accpted socket（真正的tcp底层连接）上
        return con;
    });

    base.loop();

    info("program exited");
}
