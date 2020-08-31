#include <handy/handy.h>

using namespace std;
using namespace handy;

int main(int argc, const char *argv[]) {
    int threads = 1;
    if (argc > 1) {
        threads = atoi(argv[1]);
    }
    setloglevel("TRACE");

    MultiBase base(threads);
    
    HttpServer sample(&base);

    //绑定http服务端口
    int r = sample.bind("", 8081);
    exitif(r, "bind failed %d %s", errno, strerror(errno));

    //绑定get回调函数
    sample.onGet("/hello", [](const HttpConnPtr &con) {
        string v = con.getRequest().version;
        HttpResponse resp;
        resp.body = Slice("hello world");
        con.sendResponse(resp);

        //process http 1.0 短连接
        if (v == "HTTP/1.0") {
            con->close();
        }
    });

    Signal::signal(SIGINT, [&] { base.exit(); });
    
    base.loop();
    
    return 0;
}
