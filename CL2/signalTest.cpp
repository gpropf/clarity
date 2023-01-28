
#include "Signal.hpp"
#include "WebElement.hpp"

int main() {
    // std::cout << "HELLO from signalTest.cpp!" << std::endl;
    const auto *web = new cl2::InputElement("input", "webel", "text", 1);
    auto *ssWeb = new cl2::WebElementSignalObject<std::string>(*web);

    const auto *webRecipient = new cl2::InputElement("input", "webRecipient", "text", 2);

    // cl2::BR(100,"br1");
    cl2::BR();

    auto *ssWebR = new cl2::WebElementSignalObject<std::string>(*webRecipient);

    auto *t1 = new cl2::Tee<std::string>();

    const val logFn = val::global("logStuff");
    auto *clso = new cl2::JSFunctionSignalObject<std::string>(logFn);
    // ssWeb->setOutput(clso);

    const auto *numInput = new cl2::InputElement("input", "numInput", "range", 3);
    auto *numInputWSO = new cl2::WebElementSignalObject<std::string>(*numInput);

    auto bigdiv = cl2::WebElement("div", "bigdiv", "foo", 10000);

    cl2::SVG("svg1", 400, 300, 4, bigdiv.domElement_);




    auto str2DblFn = [](std::string s) {
        double d = std::stod(s);
        cout << "Double value of string is " << d << endl;
        return d;
    };

    auto *strToNum = new cl2::Transformer<std::string, double>(str2DblFn);

    numInputWSO->setOutput(strToNum);
    numInputWSO->finalize();

    t1->setOutput(clso);
    t1->setSecondOutput(ssWebR);

    ssWeb->setOutput(t1);

    // std::string testStr = "Foo";
    // ssWeb->emitOne(clso, testStr);
    ssWeb->finalize();
    ssWebR->finalize();
    t1->finalize();
    // web->printStats();

    return 0;
}
