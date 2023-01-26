
#include "Signal.hpp"
#include "WebElement.hpp"

int main() {
    // std::cout << "HELLO from signalTest.cpp!" << std::endl;
    const auto *web = new cl2::InputElement("input", "webel", 1);
    auto *ssWeb = new cl2::WebElementSignalObject<std::string>(*web);

    const auto *webRecipient = new cl2::InputElement("input", "webRecipient", 2);
    auto *ssWebR = new cl2::WebElementSignalObject<std::string>(*webRecipient);

    const val logFn = val::global("logStuff");
    auto *clso = new cl2::ConsoleLoggerSignalObject<std::string>(logFn);
    ssWeb->addOutput(clso);
    ssWeb->addOutput(ssWebR);

    // std::string testStr = "Foo";
    // ssWeb->emitOne(clso, testStr);
    ssWeb->finalize();
    ssWebR->finalize();
    // web->printStats();

    return 0;
}
