
#include "Signal.hpp"
#include "WebElement.hpp"

int main() {
    std::cout << "HELLO from signalTest.cpp!" << std::endl;
    auto *web = new cl2::InputElement("input", "webel", 1);
    cl2::WebElementSignalObject<std::string> *ssWeb =
        new cl2::WebElementSignalObject<std::string>(*web);

    auto *webRecipient = new cl2::InputElement("input", "webRecipient", 2);
    cl2::WebElementSignalObject<std::string> *ssWebR =
        new cl2::WebElementSignalObject<std::string>(*webRecipient);

    val logFn = val::global("logStuff");
    cl2::ConsoleLoggerSignalObject<std::string> *clso =
        new cl2::ConsoleLoggerSignalObject<std::string>(logFn);
    ssWeb->addOutput(clso);
    ssWeb->addOutput(ssWebR);

    // std::string testStr = "Foo";
    // ssWeb->emitOne(clso, testStr);
    ssWeb->finalize();
    web->printStats();

    return 0;
}
