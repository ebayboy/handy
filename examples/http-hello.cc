#include <handy/handy.h>

using namespace std;
using namespace handy;

// 
// 测试脚本： curl http://0.0.0.0:8081/hello?name=fanpf
// 需要带 ? , 否则不是query_uri
// tryDecode中的解码函数为： if (query_uri[i] == '?') {

int main(int argc, const char *argv[]) {
    int threads = 1;
    if (argc > 1) {
        threads = atoi(argv[1]);
    }
    setloglevel("TRACE");

    //multithread <--> multi eventbase
    MultiBase base(threads);
    
    // Multibase -> EventBases 
    // 继承类转为基类(静态转换和动态转换同样安全)
    // HttpServer : TcpServer
    // base传递路径： multibase -> EventBase -> TcpServer -> bases->allocBase() （此处是multibase的allocBase构造函数)
    // TODO ? httpserver -> tcpserver的关系
    // 1. httpserver -> tcpserver 

    //httpserver constructor do more...
    HttpServer sample(&base);

    //绑定http服务端口
    //sample.bind->tcpserver.bind
    int r = sample.bind("", 8081);
    exitif(r, "bind failed %d %s", errno, strerror(errno));

    //绑定get回调函数
    //httpserver.onGet("uri", lambada func)
    sample.onGet("/hello", [](const HttpConnPtr &con) {
        handy::HttpRequest req = con.getRequest();
        for (auto &&i : req.args)
        {
            info("args: %s:%s", i.first.c_str(), i.second.c_str());
        }
        info("method:%s uri:%s queri_uri:%s", req.method.c_str(), req.uri.c_str(), req.query_uri.c_str());

        //show headers
        for (auto &&h : req.headers)
        {
            info("header: %s:%s", h.first.c_str(), h.second.c_str());
        }

        string v = req.version;
        info("version:%s", v.c_str());
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
