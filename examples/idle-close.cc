#include <handy/handy.h>

using namespace handy;
using namespace std;

int main(int argc, const char *argv[]) 
{
  setloglevel("TRACE");
  EventBase base;
  Signal::signal(SIGINT, [&] { base.exit(); });

  //std::shared_ptr<TcpServer> svr;
  TcpServerPtr svr = TcpServer::startServer(&base, "", 2099);
  exitif(svr == NULL, "start tcp server failed");
  svr->onConnState([](const TcpConnPtr &con) {
      if (con->getState() == TcpConn::Connected) {
      con->addIdleCB(2, [](const TcpConnPtr &con) {
          info("idle for 2 seconds, close connection");
          con->close();
          });
      }
      });

  //connect for for test
  auto con = TcpConn::createConnection(&base, "localhost", 2099);

  //register timer
  base.runAfter(3000, [&]() { base.exit(); });

  //dispatch loop
  base.loop();
}
