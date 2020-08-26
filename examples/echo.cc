
#include <iostream>
#include <handy/handy.h>

using namespace handy;
using namespace std;

int main(int argc, const char *argv[]) {
    //create event_base
    EventBase base;

    //bind base SIGINT to base.exit
    Signal::signal(SIGINT, [&] { base.exit(); });

    //create tcpserver, bind server to base
    TcpServerPtr svr = TcpServer::startServer(&base, "", 2099);

    //exit if svr == NULL
    exitif(svr == NULL, "start tcp server failed");
    
    //callback conn_read
    svr->onConnRead(
        //conn callback lamba function
        [](const TcpConnPtr &con) { 
        Slice s = con->getInput();
        std::cout << "con->getInput:" << s.toString() << std::endl;

        //conn send buffer
        con->send(con->getInput()); 
        }
        );

    //dispatch loop
    base.loop();
}
