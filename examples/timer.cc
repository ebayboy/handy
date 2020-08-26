#include <handy/handy.h>


using namespace handy;
using namespace std;

int main(int argc, const char *argv[]) {
    //create base
    EventBase base;

    //bind base + signal
    Signal::signal(SIGINT, [&] { base.exit(); });

    info("program begin");

    //case1
    base.runAfter(200, []() { info("a task in runAfter 200ms"); });

    //case2
    base.runAfter(100, []() { info("a task in runAfter 100ms interval 1000ms"); }, 1000);

    //case3
    TimerId id = base.runAt(time(NULL) * 1000 + 300, []() { info("a task in runAt now+300 interval 500ms"); }, 500);
    base.runAfter(2000, [&]() {
        info("cancel task of interval 500ms");
        base.cancel(id);
    });

    //case4
    //runafter 3000ms, then base.exit()
    base.runAfter(3000, [&]() { base.exit(); });

    //dispatch loop
    cout << "dispatch loop..." << endl;
    base.loop();
}
