
#include "WebElement.hpp"
#include "Signal.hpp"


int main() {
    std::cout << "HELLO from signalTest.cpp!" << std::endl;
    cl2::WebElement web("input", "webel", 1);
    cl2::WebElementSignalObject ssWeb(web);
    val logFn = val::global("logStuff");
    cl2::ConsoleLoggerSignalObject clso(logFn);
    ssWeb.addOutput(clso);
    std::string testStr = "Foo string";
    shared_ptr<std::string>testStrPtr = make_shared<std::string>(testStr);
    shared_ptr<void> testStrPtrVoid = std::static_pointer_cast<void>(testStrPtr);
    //ssWeb.emit(testStrPtrVoid);
    ssWeb.emitOne(&clso,testStrPtrVoid );
    web.printStats();

    return 0;
}
