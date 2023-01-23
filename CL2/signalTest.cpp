
#include "Signal.hpp"
#include "WebElement.hpp"

int main() {
    std::cout << "HELLO from signalTest.cpp!" << std::endl;
    cl2::WebElement *web = new cl2::WebElement("input", "webel", 1);
    cl2::WebElementSignalObject<std::string> *ssWeb =
        new cl2::WebElementSignalObject<std::string>(*web);
    val logFn = val::global("logStuff");
    cl2::ConsoleLoggerSignalObject<std::string> *clso =
        new cl2::ConsoleLoggerSignalObject<std::string>(logFn);
    ssWeb->addOutput(clso);
    std::string testStr = "Foo string";
    // shared_ptr<std::string> testStrPtr = make_shared<std::string>(testStr);
    // shared_ptr<void> testStrPtrVoid = std::static_pointer_cast<void>(testStrPtr);
    // ssWeb.emit(testStrPtrVoid);
    ssWeb->emitOne(clso, testStr);
    ssWeb->finalize();
    web->printStats();

    return 0;
}
