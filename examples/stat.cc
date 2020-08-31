#include <handy/handy.h>
#include <handy/stat-svr.h>

using namespace std;
using namespace handy;

int main(int argc, const char *argv[]) {
    Logger::getLogger().setLogLevel("DEBUG");
    EventBase base;
    StatServer sample(&base);
    int r = sample.bind("", 80);
    exitif(r, "bind failed %d %s", errno, strerror(errno));
    
    //get loglevel： curl http://0.0.0.0/loglevel?
    sample.onState("loglevel", "log level for server", [] { return Logger::getLogger().getLogLevelStr(); });

    //get pid ： curl http://0.0.0.0/pid?
    sample.onState("pid", "process id of server", [] { return util::format("%d", getpid()); });
    
    //set lesslog ：设置更多的日志TRACE级别
    //get pid ： curl http://0.0.0.0/lesslog?
    sample.onCmd("lesslog", "set log to less detail", [] {
        Logger::getLogger().adjustLogLevel(-1);
        return "OK";
    });

    //set morelog： 设置更少的日志INFO级别
    sample.onCmd("morelog", "set log to more detail", [] {
        Logger::getLogger().adjustLogLevel(1);
        return "OK";
    });

    //set restart： 重启进程 
    sample.onCmd("restart", "restart program", [&] {
        base.safeCall([&] {
            base.exit();
            Daemon::changeTo(argv);
        });
        return "restarting";
    });

    //set stop： 停止进程
    sample.onCmd("stop", "stop program", [&] {
        base.safeCall([&] { base.exit(); });
        return "stoping";
    });
    
    //上述路由没有命中的话，则进入下面的路由
    //命中onPage("page") ： 获取page
     //get pid ： curl http://0.0.0.0/page?
    sample.onPage("page", "show page content", [] { return "this is a page"; });

    //没有命中onPage("page"), 返回404

    Signal::signal(SIGINT, [&] { base.exit(); });
    base.loop();
    return 0;
}
