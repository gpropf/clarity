
#include "WebElement.hpp"
#include "Signal.hpp"


int main() {
    std::cout << "HELLO from signalTest.cpp!" << std::endl;
    cl2::WebElement web("input", "webel", 1);
    cl2::WebElementSignalObject<std::string> ssWeb(web);
    val logFn = val::global("logStuff");
    cl2::ConsoleLoggerSignalObject<std::string> clso(logFn);
    ssWeb.addOutput(make_shared<void>(clso));
    web.printStats();

    return 0;
}
