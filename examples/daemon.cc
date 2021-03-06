#include <handy/handy.h>

using namespace std;
using namespace handy;

/* 
logfile=daemon.log
loglevel=debug
log_rotate_interval=120
*/
int main(int argc, const char *argv[]) {
    if (argc != 2) {
        printf("usage: %s <start|stop|restart>\n", argv[0]);
        return 1;
    }
    string program = argv[0];
    string pidfile = program + ".pid";
    string conffile = program + ".conf";

    Daemon::daemonProcess(argv[1], pidfile.c_str());

    Conf conf;
    int r = conf.parse(conffile.c_str());
    fatalif(r, "config file parse failed %s", conffile.c_str());

    //logfile :  daemon.log, default:program.log
    string logfile = conf.get("", "logfile", program + ".log");

    //loglevel : debug
    string loglevel = conf.get("", "loglevel", "INFO");

    //log_rotate_interval : 120(s)
    long rotateInterval = conf.getInteger("", "log_rotate_interval", 86400); //默认： 24*2600 = 1 day
    fprintf(stderr, "conf: file: %s level: %s interval: %ld\n", logfile.c_str(), loglevel.c_str(), rotateInterval);
    
    //set logger opt
    Logger::getLogger().setFileName(logfile.c_str());
    Logger::getLogger().setLogLevel(loglevel.c_str());
    Logger::getLogger().setRotateInterval(rotateInterval);

    //create tcpserver
    EventBase base;
    Signal::signal(SIGINT, [&] { base.exit(); });
    TcpServerPtr echo = TcpServer::startServer(&base, "", 2099);
    exitif(echo == NULL, "start tcp server failed");
    echo->onConnRead([](const TcpConnPtr &con) { con->send(con->getInput()); });
    
    //start after 1000ms,  exec interval 1000ms
    base.runAfter(1000, [] { info("log"); }, 1000);

    //dispatch loop
    base.loop();
    info("program exited");
}