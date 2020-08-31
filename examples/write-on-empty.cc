#include <handy/handy.h>

using namespace std;
using namespace handy;

// 20M文件的buf
char buf[20 * 1024 * 1024];

static int times = 0;

int main(int argc, const char *argv[]) {
    setloglevel("TRACE");
    int sended = 0, total = 1054768 * 100;

    memset(buf, 'a', sizeof buf);

    EventBase bases;
    Signal::signal(SIGINT, [&] { bases.exit(); });

    TcpServer echo(&bases);
    int r = echo.bind("", 2099);
    exitif(r, "bind failed %d %s", errno, strerror(errno));

    // lambda sendcb
    auto sendcb = [&](const TcpConnPtr &con) {
        info("Callback write_buf...[%d]", times++);
        while (con->getOutput().size() == 0 && sended < total) {
            con->send(buf, sizeof buf);
            sended += sizeof buf;
            info("%d bytes sended output size: %lu", sended, con->getOutput().size());
        }
        if (sended >= total) {
            con->close();
            bases.exit();
        }
    };

    // call lambda sendcb
    echo.onConnCreate([sendcb]() {
        //连接创建时回调
        TcpConnPtr con(new TcpConn);

        //连接状态变化回调
        con->onState([sendcb](const TcpConnPtr &con) {
            if (con->getState() == TcpConn::Connected) {
                //当连接状态变化为  TcpConn::Connected， 设置可写回调函数为sendcb
                con->onWritable(sendcb);
            }

            info("Direct write_buf...[%d]", times++);
            sendcb(con);
        });
        return con;
    });

    //模拟了一个客户端，连接服务器后，接收服务器发送过来的数据
    thread th([] {
        EventBase base2;
        TcpConnPtr con = TcpConn::createConnection(&base2, "127.0.0.1", 2099);
        con->onRead([](const TcpConnPtr &con) {
            info("recv %lu bytes", con->getInput().size());
            con->getInput().clear();
            sleep(1);
        });
        con->onState([&](const TcpConnPtr &con) {
            if (con->getState() == TcpConn::Closed || con->getState() == TcpConn::Failed) {
                base2.exit();
            }
        });
        base2.loop();
    });

    bases.loop();
    th.join();
    info("program exited");
}