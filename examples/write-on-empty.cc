#include <handy/handy.h>

using namespace std;
using namespace handy;

// 20M文件的buf
char buf[20 * 1024 * 1024];

static int times = 0;

int main(int argc, const char *argv[]) {
    setloglevel("INFO");
    
    int sended = 0, total = 1024 * 1024 * 100; //100M

    memset(buf, 'a', sizeof buf);

    EventBase bases;
    Signal::signal(SIGINT, [&] { bases.exit(); });

    TcpServer echo(&bases);
    int r = echo.bind("", 2099);
    exitif(r, "bind failed %d %s", errno, strerror(errno));

    // lambda sendcb
    auto sendcb = [&](const TcpConnPtr &con) {
        info("Callback write_buf...[%d] state[%d]", times++, con->getState());
        //如果输出缓冲区没有数据， 且发送长度小区100M，则发送数据
        while (con->getOutput().size() == 0 && sended < total) {
            con->send(buf, sizeof buf); //发送buf 20M
            sended += sizeof buf;
            info("%d bytes sended output size: %lu", sended, con->getOutput().size());
        }

        //发送超过100M,程序退出
        if (sended >= total) {
          int state = con->getState();
          info("xxx: will exit enter! state:[%d]", state);
          con->close(); // con->close -> state change -> onState -> sendcb ->  here again
          bases.exit();
          info("xxx: will exit leave! state:[%d], conn_state:[%d]", state, con->getState());
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
            
            info("xxx: state:%d", con->getState());
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
            //每1秒钟接收一次数据
            sleep(1);
        });

        //设置连接状态变化回调函数， 当server关闭连接时会调用
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
