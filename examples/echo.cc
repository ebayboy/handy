
#include <iostream>
#include <handy/handy.h>

using namespace handy;
using namespace std;

int main(int argc, const char *argv[]) {
    EventBase base;
    Signal::signal(SIGINT, [&] { base.exit(); });
    TcpServerPtr svr = TcpServer::startServer(&base, "", 2099);
    exitif(svr == NULL, "start tcp server failed");
    svr->onConnRead(
        [](const TcpConnPtr &con) { 

        Slice s = con->getInput();

        std::cout << "con->getInput:" << s.toString() << std::endl;
        con->send(con->getInput()); 
        }
        );
    base.loop();
}
