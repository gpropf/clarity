
#include "SignalPrimitives.hpp"
#include "Util.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

/**
 * @brief This is a simple do-nothing class meant to test the `CppObjectSignalObject` class' ability
 * to use getter and setter method pointers to act as initiation points or endpoints for signals.
 *
 */
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
    // We make a test object and then a signal wrapper for it.
    TestObj tobj;
    auto *testObjCSO = new cl2::CppObjectSignalObject<std::string, TestObj>(tobj);
    // The signal wrapper needs to know how to set and get the stored value.
    testObjCSO->setter = &TestObj::setS;
    testObjCSO->getter = &TestObj::getS;

    // Now we're going to create a JS timer to read the value and fire the signal every so often.
    val testObjEmitter = val::global("elgTestObjEmitter")(val(testObjCSO));
    val setInterval = val::global("setInterval");
    setInterval(testObjEmitter, val(500));

    // It's easy to get control of elements that are already present in a static HTML page by
    // creating a WE from the id.
    const auto capturedDiv = cl2::WebElement("555");

    // We can also create de-novo elements.
    auto generatedDiv = cl2::WebElement("div", "generatedDiv", getStrId());

    // Creating two text fields.
    const auto srcTextInput = cl2::InputElement("input", "srcTextInput", "text", getStrId());
    const auto dstTextInput = cl2::InputElement("input", "dstTextInput", "text", getStrId());
    cl2::BR();

    // Signal wrappers for the text fields.
    auto *srcTextInputWSO = new cl2::WebElementSignalObject<std::string>(srcTextInput, "value");
    auto *dstTextInputWSO = new cl2::WebElementSignalObject<std::string>(dstTextInput, "value");

    // srcTextInputWSO->finalize();


    // We create a JS function to use as an endpoint for a JSFunctionSignalObject.
    const val logFn = val::global("logStuff");
    auto *consoleLogFSO = new cl2::JSFunctionSignalObject<std::string>(logFn);

    // A Tee allow us to send an output to two inputs.
    auto *t1 = new cl2::Tee<std::string>();

    t1->setOutput(consoleLogFSO);
    t1->setSecondOutput(dstTextInputWSO);

    // Our srcTextInputWSO message will go through the Tee to 2 places.
    srcTextInputWSO->setOutput(t1);

    
    srcTextInputWSO->finalize();
    dstTextInputWSO->finalize();
    t1->finalize();
    // srcTextInputWSO->setOutput(consoleLogFSO);
    //  testObjCSO->setOutput(consoleLogFSO);

    // Now we're going to create an SVG area and a circle within it. We will create range controls
    // to adjust the size and position of the circle.
    const auto circle1CXRangeInput =
        cl2::InputElement("input", "circle1CXRangeInput", "range", getStrId());
    const auto circle1CYRangeInput =
        cl2::InputElement("input", "circle1CYRangeInput", "range", getStrId());

    // Label the controls.
    cl2::Label("Circle center X value", circle1CXRangeInput, true, getStrId());
    cl2::BR();
    cl2::Label("Circle center Y value", circle1CYRangeInput, true, getStrId());
    cl2::BR();

    // Signal wrappers for the controls.
    auto *circle1CXRangeInputWSO =
        new cl2::WebElementSignalObject<std::string>(circle1CXRangeInput, "value");
    auto *circle1CYRangeInputWSO =
        new cl2::WebElementSignalObject<std::string>(circle1CYRangeInput, "value");

    auto svg = cl2::SVG("svg1", 400, 300, getStrId(), generatedDiv.domElement_);
    svg.setAttributes({{"viewBox", val("0 0 100 100")}, {"style", val("border: 1px solid black")}});

    const auto circle1 = cl2::WebElement("circle", "circle1", getStrId(), svg.domElement_);
    circle1.domElement_.call<void>("setAttribute", val("r"), val(45));
    circle1.domElement_.call<void>("setAttribute", val("cx"), val(120));
    circle1.domElement_.call<void>("setAttribute", val("cy"), val(100));
    circle1.domElement_.call<void>("setAttribute", val("fill"), val("#ff0000"));

    // Now we need signal wrappers to connect to various attributes of the circle.
    auto *circle1CXWSO = new cl2::WebElementSignalObject<std::string>(circle1, "cx");
    auto *circle1CYWSO = new cl2::WebElementSignalObject<std::string>(circle1, "cy");

    // Now the range controls are connected to the circle attributes.
    circle1CXRangeInputWSO->setOutput(circle1CXWSO);
    // circle1CXRangeInputWSO->setOutput(testObjCSO);
    circle1CYRangeInputWSO->setOutput(circle1CYWSO);
    circle1CXRangeInputWSO->finalize();
    circle1CYRangeInputWSO->finalize();

    // This is the 'functional' part of FRP. We have a pure function here defined as a C++ lambda.
    // We will set this up as the core of a CppLambda object that takes a string, runs the lambda
    // on it, and outputs a double.
    auto str2DblFn = [](std::string s) {
        double d = std::stod(s);
        cout << "Value of string as a double is: " << d << endl;
        return d;
    };

    auto *strToNumTransformer = new cl2::CppLambda<std::string, double>(str2DblFn);
    const auto dblInput = cl2::InputElement("input", "dblInput", "text", getStrId());
    cl2::Label("Enter a floating point number", dblInput, true, getStrId());

    // We now create a signal wrapper for the input field and connect it to the conversion function.
    auto *dblInputWSO = new cl2::WebElementSignalObject<std::string>(dblInput, "value", false);
    dblInputWSO->setOutput(strToNumTransformer);
    dblInputWSO->finalize();
    // circle1CXRangeInputWSO->setOutput(strToNumTransformer);

    

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
