
#include "WebElementBuilder.hpp"



int main() {
    std::cout << "HELLO from signalTest.cpp!" << std::endl;
    cl2::WebElementBuilder web("input", "webel", 1);

    web.printStats();
    return 0;
}
