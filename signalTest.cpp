
#include "ElementBuilder.hpp"
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
    auto *testObjCSO = new cl2::CppObjectSignalObject<std::string, TestObj>(tobj, true);
    // The signal wrapper needs to know how to set and get the stored value.
    testObjCSO->setter = &TestObj::setS;
    testObjCSO->getter = &TestObj::getS;

    // Now we're going to create a JS timer to read the value and fire the signal every so often.
    val testObjEmitter = val::global("elgTestObjEmitter")(val(testObjCSO));
    val setInterval = val::global("setInterval");
    //setInterval(testObjEmitter, val(500));

    // It's easy to get control of elements that are already present in a static HTML page by
    // creating a WE from the id.
    const auto capturedDiv = cl2::WebElement("555");

    // We can also create de-novo elements.
    auto generatedDiv = cl2::WebElement("div", "generatedDiv", getStrId());

    // We're now using the ElementBuilder factory objects to create our web content.
    cl2::ElementBuilder eb = cl2::ElementBuilder();

    // Creating two text fields.
    const auto srcTextInput = eb.textInput(
        "srcTextInput", "Source field: type something here and it will appear in the next field.");
    const auto dstTextInput = eb.textInput(
        "dstTextInput",
        "Destination field: Type something in the field above and it will be copied here.");

    // Signal wrappers for the text fields.
    auto *srcTextInputWSO = new cl2::WebElementSignalObject<std::string>(srcTextInput, "value");
    auto *dstTextInputWSO = new cl2::WebElementSignalObject<std::string>(dstTextInput, "value");

    // We create a JS function to use as an endpoint for a JSFunctionSignalObject.
    const val logFn = val::global("logStuff");
    auto *consoleLogFSO = new cl2::JSFunctionSignalObject<std::string>(logFn);

    // A Tee allow us to send an output to two inputs.
    auto *t1 = new cl2::Tee<std::string>();

    t1->setOutput(consoleLogFSO);
    t1->setSecondOutput(dstTextInputWSO);

    // Our srcTextInputWSO message will go through the Tee to 2 places.
    srcTextInputWSO->setOutput(t1);

    // Now we're going to create an SVG area and a circle within it. We will create range controls
    // to adjust the size and position of the circle.
    const auto circle1CXRangeInput = eb.rangeInput("circle1CXRangeInput", "Circle center X value");
    const auto circle1CYRangeInput = eb.rangeInput("circle1CYRangeInput", "Circle center Y value");

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
    circle1CYRangeInputWSO->setOutput(circle1CYWSO);

    // This is the 'functional' part of FRP. We have a pure function here defined as a C++ lambda.
    // We will set this up as the core of a CppLambda object that takes a string, runs the lambda
    // on it, and outputs a double.
    auto str2DblFn = [](std::string s) {
        double d = std::stod(s);
        cout << "Value of string as a double is: " << d << endl;
        return d;
    };

    // We now place our lambda in the core of the CppLambda signal wrapper.
    auto *strToNumTransformer = new cl2::CppLambda<std::string, double>(str2DblFn);

    // String to convert to a number.
    const auto dblInput = eb.textInput("dblInput", "Enter a floating point number");

    // We now create a signal wrapper for the input field and connect it to the conversion function.
    auto *dblInputWSO = new cl2::WebElementSignalObject<std::string>(dblInput, "value", false);
    dblInputWSO->setOutput(strToNumTransformer);

    // Here we're going back to our TestObj and creating a field that will allow the user to update
    // the string value it contains.
    const auto testObjValTextInput = eb.textInput(
        "testObjValTextInput", "Enter a new value for the string stored in the TestObj.");

    auto *testObjValTextInputWSO =
        new cl2::WebElementSignalObject<std::string>(testObjValTextInput, "value", false);
    testObjValTextInputWSO->setOutput(testObjCSO);
    testObjCSO->setOutput(testObjValTextInputWSO);

    auto mergeFn = [](std::string s1, std::string s2) { return s1 + s2; };
    auto* mergeSignal = new cl2::Merge<std::string, std::string, std::string>(mergeFn);

    const auto m1Input = eb.textInput("m1Input", "Enter the first value");
    const auto m2Input = eb.textInput("m2Input", "Enter the second value");
    const auto mergeOut = eb.textInput("mergeOut", "Output of Merge signal goes here");
    auto *m1InputWSO = new cl2::WebElementSignalObject<std::string>(m1Input, "value", false);
    auto *m2InputWSO = new cl2::WebElementSignalObject<std::string>(m2Input, "value", false);
    auto *mergeOutWSO = new cl2::WebElementSignalObject<std::string>(mergeOut, "value", false);
    
    m1InputWSO->setOutput(mergeSignal->getInput1());
    m2InputWSO->setOutput(mergeSignal->getInput2());
    mergeSignal->setOutput(mergeOutWSO);
    mergeOutWSO->setOutput(mergeSignal);

    //m1InputWSO->update();
    //mergeSignal->setOutput(consoleLogFSO);

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
