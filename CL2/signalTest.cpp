
#include "SignalPrimitives.hpp"
#include "Util.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

class TestObj {
    int i_ = 0;
    double d_ = 0;
    std::string s_ = "foo";

   public:
    void setS(const std::string &s) {
        s_ = s;
        cout << "TestObj.s_ has been set to: " << s_ << endl;
    }
    std::string getS() { return s_; }
};

int main() {
    TestObj tobj;

    auto *testObjCSO = new cl2::CppObjectSignalObject<std::string, TestObj>(tobj);

    testObjCSO->setter = &TestObj::setS;
    testObjCSO->getter = &TestObj::getS;

    val testObjEmitter = val::global("elgTestObjEmitter")(val(testObjCSO));
    

    val setInterval = val::global("setInterval");
    setInterval(testObjEmitter, val(500));

    val domEl555 = val::global("document").call<val>("getElementById", val("555"));
    // const auto capturedDiv = cl2::WebElement(domEl555);
    const auto capturedDiv = cl2::WebElement("555");
    const auto *sourceTextInput =
        new cl2::InputElement("input", "sourceTextInput", "text", getStrId());
    auto *sourceTextInputWSO =
        new cl2::WebElementSignalObject<std::string>(*sourceTextInput, "value");

    const auto *destTextInput = new cl2::InputElement("input", "destTextInput", "text", getStrId());

    // cl2::BR(100,"br1");
    cl2::BR();

    auto *destTextInputWSO = new cl2::WebElementSignalObject<std::string>(*destTextInput, "value");

    auto *t1 = new cl2::Tee<std::string>();

    const val logFn = val::global("logStuff");
    auto *consoleLogFSO = new cl2::JSFunctionSignalObject<std::string>(logFn);
    sourceTextInputWSO->setOutput(consoleLogFSO);
    //testObjCSO->setOutput(consoleLogFSO);
    
    const auto circle1CXRangeInput =
        cl2::InputElement("input", "circle1CXRangeInput", "range", getStrId());

    const auto circle1CYRangeInput =
        cl2::InputElement("input", "circle1CYRangeInput", "range", getStrId());

    cl2::Label("Circle center X value", circle1CXRangeInput, true, getStrId());
    cl2::BR();
    cl2::Label("Circle center Y value", circle1CYRangeInput, true, getStrId());

    auto *circle1CXRangeInputWSO =
        new cl2::WebElementSignalObject<std::string>(circle1CXRangeInput, "value");
    auto *circle1CYRangeInputWSO =
        new cl2::WebElementSignalObject<std::string>(circle1CYRangeInput, "value");

    // auto bigdiv = cl2::WebElement("div", "bigdiv");

    auto svg = cl2::SVG("svg1", 400, 300, getStrId(), capturedDiv.domElement_);
    svg.setAttributes({{"viewBox", val("0 0 100 100")}, {"style", val("border: 1px solid black")}});

    const auto circle1 = cl2::WebElement("circle", "circle1", getStrId(), svg.domElement_);
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
    // circle1CXRangeInputWSO->setOutput(testObjCSO);

    circle1CYRangeInputWSO->setOutput(circle1CYWSO);
    circle1CXRangeInputWSO->finalize();
    circle1CYRangeInputWSO->finalize();

    t1->setOutput(consoleLogFSO);
    t1->setSecondOutput(destTextInputWSO);

    sourceTextInputWSO->setOutput(t1);

    sourceTextInputWSO->finalize();
    destTextInputWSO->finalize();
    t1->finalize();
    // sourceTextInput->printStats();

    return 0;
}

EMSCRIPTEN_BINDINGS(CppObjectSignalObject) {
    emscripten::class_<cl2::CppObjectSignalObject<std::string, TestObj>>("CppObjectSignalObject")
        .function("emit", &cl2::CppObjectSignalObject<std::string, TestObj>::emit,
                  emscripten::allow_raw_pointers())
        .function("getSignal", &cl2::CppObjectSignalObject<std::string, TestObj>::getSignal,
                  emscripten::allow_raw_pointers())
        .function("accept", &cl2::CppObjectSignalObject<std::string, TestObj>::accept,
                  emscripten::allow_raw_pointers());
}
