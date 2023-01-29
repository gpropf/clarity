
#include "SignalPrimitives.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

int main() {
    // std::cout << "HELLO from signalTest.cpp!" << std::endl;
    const auto *web = new cl2::InputElement("input", "webel", "text", 1);
    auto *ssWeb = new cl2::WebElementSignalObject<std::string>(*web, "value");

    const auto *webRecipient = new cl2::InputElement("input", "webRecipient", "text", 2);

    // cl2::BR(100,"br1");
    cl2::BR();

    auto *ssWebR = new cl2::WebElementSignalObject<std::string>(*webRecipient, "value");

    auto *t1 = new cl2::Tee<std::string>();

    const val logFn = val::global("logStuff");
    auto *clso = new cl2::JSFunctionSignalObject<std::string>(logFn);
    // ssWeb->setOutput(clso);

    const auto circle1CXRangeInput = cl2::InputElement("input", "circle1CXRangeInput", "range", 3);
    const auto circle1CYRangeInput = cl2::InputElement("input", "circle1CYRangeInput", "range", 4);
    auto *circle1CXRangeInputWSO =
        new cl2::WebElementSignalObject<std::string>(circle1CXRangeInput, "value");
    auto *circle1CYRangeInputWSO =
        new cl2::WebElementSignalObject<std::string>(circle1CYRangeInput, "value");

    auto bigdiv = cl2::WebElement("div", "bigdiv", 10000);

    auto svg = cl2::SVG("svg1", 400, 300, 5, bigdiv.domElement_);
    svg.setAttributes({{"viewBox", val("0 0 100 100")}, {"style", val("border: 1px solid black")}});

    const auto circle1 = cl2::WebElement("circle", "circle1", 6, svg.domElement_);
    circle1.domElement_.call<void>("setAttribute", val("r"), val(45));
    circle1.domElement_.call<void>("setAttribute", val("cx"), val(120));
    circle1.domElement_.call<void>("setAttribute", val("cy"), val(100));
    circle1.domElement_.call<void>("setAttribute", val("fill"), val("#ff0000"));

    auto *circle1CXWSO = new cl2::WebElementSignalObject<std::string>(circle1, "cx");
    auto *circle1CYWSO = new cl2::WebElementSignalObject<std::string>(circle1, "cy");

    auto str2DblFn = [](std::string s) {
        double d = std::stod(s);
        cout << "Value of string as a double is: " << d << endl;
        return d;
    };

    auto *strToNum = new cl2::Transformer<std::string, double>(str2DblFn);

    // circle1CXRangeInputWSO->setOutput(strToNum);
    circle1CXRangeInputWSO->setOutput(circle1CXWSO);
    circle1CYRangeInputWSO->setOutput(circle1CYWSO);
    circle1CXRangeInputWSO->finalize();
    circle1CYRangeInputWSO->finalize();

    t1->setOutput(clso);
    t1->setSecondOutput(ssWebR);

    ssWeb->setOutput(t1);

    ssWeb->finalize();
    ssWebR->finalize();
    t1->finalize();
    // web->printStats();

    return 0;
}
