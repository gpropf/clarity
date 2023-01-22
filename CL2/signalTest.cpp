
#include "WebElement.hpp"
#include "Signal.hpp"


int main() {
    std::cout << "HELLO from signalTest.cpp!" << std::endl;
    cl2::WebElement web("input", "webel", 1);
    cl2::SignalSource<cl2::WebElement, std::string> ssWeb(web);
    web.printStats();

    return 0;
}
