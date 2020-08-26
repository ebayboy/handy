#include <handy/handy.h>
using namespace handy;

int main(int argc, const char *argv[]) {

  //listen
  setloglevel("TRACE");

  EventBase base;
  info("====EventBase over!");

  Signal::signal(SIGINT, [&] { base.exit(); });
  info("====signal over!");

  TcpServerPtr svr = TcpServer::startServer(&base, "", 2099);
  exitif(svr == NULL, "start tcp server failed");
  info("====TcpServer::startServer over!");

  //on_connected
  svr->onConnState([&](const TcpConnPtr &con) {  // 200ms后关闭连接
      if (con->getState() == TcpConn::Connected)
      info("new client connected..., will close conn after 200ms...");
      base.runAfter(200, [con]() {
          info("close con with 200ms arrived!");
          con->close();
          });
      });
  info("====onConnState over");

  //connect to srv
  TcpConnPtr con1 = TcpConn::createConnection(&base, "localhost", 2099);
  info("====createConnection over");
  //set cli conn reconnect interval
  //
  info("set cli conn reconnect interval 300ms...");
  con1->setReconnectInterval(300);
  info("====setReconnectInterval over");

  //    TcpConnPtr con2 = TcpConn::createConnection(&base, "localhost", 1, 100);

  //以下将时间点发生的事件
  //1. 200ms : close(cliconn) -1 
  //2. 300ms: 重连第1次
  //3. 500ms: close(cliconn) -2
  //4. 600ms 以后程序退出
  //5. 
  
  //exit after 600ms
  base.runAfter(600, [&]() { base.exit(); });
  info("====base.runAfter over");

  //dispatch loop
  info("dispatch loop...");
  base.loop();
  info("====base.loop over");
}
