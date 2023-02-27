
#include "SignalBuilder.hpp"
#include "SignalPrimitives.hpp"
#include "Signals.hpp"
#include "Util.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

using namespace cl2;

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

    void signalAcceptorTestMethod(const std::string &s) {
        s_ = s;
        cout << "TestObj::signalAcceptorTestMethod(): " << s_ << endl;
    }

    void mouseAcceptorTestMethod(const std::pair<int, int> &mouseLocation) {
        
        cout << "TestObj::mouseAcceptorTestMethod(): x = " << mouseLocation.first << ", y = " << mouseLocation.second << endl;
    }

    std::string getS() { return s_; }
};

int main() {
    // We make a test object and then a signal wrapper for it.
    TestObj tobj;
    auto tobjSptr = make_shared<TestObj>();
    auto testObjCSO = make_shared<cl2::CppObjectSignalObject<std::string, TestObj>>(tobj, true);
    // The signal wrapper needs to know how to set and get the stored value.
    testObjCSO->setter = &TestObj::setS;
    testObjCSO->getter = &TestObj::getS;

    // Now we're going to create a JS timer to read the value and fire the signal every so often.
    val testObjEmitter = val::global("elgTestObjEmitter")(val(*testObjCSO));
    // val setInterval = val::global("setInterval");
    setInterval(testObjEmitter, 500);

    // It's easy to get control of elements that are already present in a static HTML page by
    // creating a WE from the id.
    const auto capturedDiv = cl2::WebElement("555");

    // We can also create de-novo elements.
    auto generatedDiv = cl2::WebElement("div", "generatedDiv", getStrId());

    // We're now using the SignalBuilder factory objects to create our web content.
    cl2::SignalBuilder sb = cl2::SignalBuilder();  //

    // auto srcTextInputWSO = sb.textInput<std::string>(
    //     "srcTextInput", "Source field: type something here and it will appear in the next
    //     field.");
    // auto dstTextInputWSO = sb.textInput<std::string>(
    //     "dstTextInput",
    //     "Dest field: type something in the field above and it will be copied here.");

    auto srcTextInputWSS = sb.textInputWSS<std::string>(
        "srcTextInput",
        "WSS: Source field: type something here and it will appear in the next field.");
    auto dstTextInputWSS = sb.textInputWSS<std::string>(
        "dstTextInput",
        "WSS: Dest field: type something in the field above and it will be copied here.");

    auto dstTextInputWSS2 =
        sb.textInputWSS<std::string>("dstTextInputWSS2", "WSS: MFork Dest field 2.");
    auto dstTextInputWSS3 =
        sb.textInputWSS<std::string>("dstTextInputWSS3", "WSS: MFork Dest field 3.");
    auto dstTextInputWSS4 =
        sb.textInputWSS<std::string>("dstTextInputWSS4", "WSS: MFork Dest field 4.");

    auto mfork1 = make_shared<MultiFork<std::string>>();
    sb.connect<std::string>(srcTextInputWSS, dstTextInputWSS);
    sb.connect<std::string>(dstTextInputWSS, mfork1);
    mfork1->addOutput(dstTextInputWSS2);
    mfork1->addOutput(dstTextInputWSS3);
    mfork1->addOutput(dstTextInputWSS4);
    // mfork1->addOutput(dstTextInputWSS);

    auto objectAcceptor = make_shared<ObjectAcceptor<std::string, TestObj>>(tobjSptr);
    objectAcceptor->setSignalAcceptorMethod(&TestObj::signalAcceptorTestMethod);
    mfork1->addOutput(objectAcceptor);

    // We create a JS function to use as an endpoint for a JSFunctionSignalObject.
    const val logFn = val::global("logStuff");
    auto consoleLogFSO = make_shared<cl2::JSFunctionSignalObject<std::string>>(logFn);

    // A Tee allow us to send an output to two inputs.
    auto t1 = make_shared<cl2::Tee<std::string>>();

    // Our srcTextInputWSO message will go through the Tee to 2 places.
    // sb.connect<std::string>(t1, consoleLogFSO, dstTextInputWSO);
    // sb.connect<std::string>(srcTextInputWSO, t1);

    // Now we're going to create an SVG area and a circle within it. We will create range controls
    // to adjust the size and position of the circle.

    // Signal wrappers for the controls.
    auto circle1CXRangeInputWSO =
        sb.rangeInputWSS<std::string>("circle1CXRangeInput", "Circle center X value");
    auto circle1CYRangeInputWSO =
        sb.rangeInputWSS<std::string>("circle1CYRangeInput", "Circle center Y value");

    auto svg = cl2::SVG("svg1", 400, 300, getStrId(), generatedDiv.domElement_);
    svg.setAttributes({{"viewBox", val("20 20 60 60")}, {"style", val("border: 1px solid black")}});

    auto mouseSignal = make_shared<MouseSignal<std::pair<int, int>>>(svg, "click");

auto svgMouseClickAcceptor = make_shared<ObjectAcceptor<std::pair<int, int>, TestObj>>(tobjSptr);
    svgMouseClickAcceptor->setSignalAcceptorMethod(&TestObj::mouseAcceptorTestMethod);


    // auto svgMouseClickOutput =
    //     sb.textInputWSS<std::pair<int, int>>("svgMouseClickOutput", "SVG Mouse click location.");

    sb.connect<std::pair<int, int>>(mouseSignal, svgMouseClickAcceptor);

    const auto circle1 = cl2::WebElement("circle", "circle1", getStrId(), svg.domElement_);
    circle1.domElement_.call<void>("setAttribute", val("r"), val(45));
    circle1.domElement_.call<void>("setAttribute", val("cx"), val(120));
    circle1.domElement_.call<void>("setAttribute", val("cy"), val(100));
    circle1.domElement_.call<void>("setAttribute", val("fill"), val("#ff0000"));

    // Now we need signal wrappers to connect to various attributes of the circle.
    auto circle1CXWSO = make_shared<cl2::WebElementSignalObjectSS<std::string>>(circle1, "cx");
    auto circle1CYWSO = make_shared<cl2::WebElementSignalObjectSS<std::string>>(circle1, "cy");

    // Now the range controls are connected to the circle attributes.
    sb.connect<std::string>(circle1CXRangeInputWSO, circle1CXWSO);
    sb.connect<std::string>(circle1CYRangeInputWSO, circle1CYWSO);

    // This is the 'functional' part of FRP. We have a pure function here defined as a C++ lambda.
    // We will set this up as the core of a CppLambda object that takes a string, runs the lambda
    // on it, and outputs a double.
    auto str2DblFn = [](std::string s) {
        double d = std::stod(s);
        cout << "Value of string as a double is: " << d << endl;
        return d;
    };

    // We now place our lambda in the core of the CppLambda signal wrapper.
    // auto strToNumTransformer = make_shared<cl2::CppLambda<std::string, double>>(str2DblFn);
    auto strToNumTransformerSS = make_shared<cl2::CppLambdaSS<std::string, double>>(str2DblFn);

    // String to convert to a number.
    // const auto dblInputWSO =
    //     sb.textInput<std::string>("dblInput", "Enter a floating point number", false);

    // InputElement dblInput = InputElement("input", "dblInput", "text", "ss1");
    // val dblInputDE = dblInput.getDomElement();
    // auto dblInputELE = make_shared<EventListenerEmitter<std::string>>(dblInputDE, "change");

    auto dblInputELE =
        sb.textInputELE<std::string>("dblInput", "Enter a floating point number", false);

    // dblInputELE->setOutput(strToNumTransformerSS);
    // dblInputELE->update();
    sb.connect<std::string>(dblInputELE, strToNumTransformerSS);

    // Connect the input field to the conversion function
    // sb.connect<std::string>(dblInputWSO, strToNumTransformer);

    // Here we're going back to our TestObj and creating a field that will allow the user to update
    // the string value it contains.
    const auto testObjValTextInputWSO = sb.textInput<std::string>(
        "testObjValTextInput", "Enter a new value for the string stored in the TestObj.");

    sb.connect<std::string>(testObjValTextInputWSO, testObjCSO);
    sb.connect<std::string>(testObjCSO, testObjValTextInputWSO);

    auto mergeFn = [](std::string s1, std::string s2) { return s1 + s2; };
    auto mergeSignal = make_shared<cl2::Merge<std::string, std::string, std::string>>(mergeFn);

    const auto m1InputWSO = sb.textInput<std::string>("m1Input", "Enter the first value", false);
    const auto m2InputWSO = sb.textInput<std::string>("m2Input", "Enter the second value", false);
    const auto mergeOutWSO =
        sb.textInput<std::string>("mergeOut", "Output of merged signals goes here", false);

    sb.connect<std::string, std::string, std::string>(m1InputWSO, m2InputWSO, mergeSignal,
                                                      mergeOutWSO);

    val recomputeMergeFn = val::global("elgMergeRecompute")(val(*mergeSignal));
    const auto mergeRecomputeButton = sb.button("Recompute", recomputeMergeFn);

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

    emscripten::class_<cl2::Merge<std::string, std::string, std::string>>("Merge").function(
        "recompute", &cl2::Merge<std::string, std::string, std::string>::recompute);

    emscripten::class_<cl2::EventListenerEmitter<std::string>>("EventListenerEmitter")
        .function("emit", &cl2::EventListenerEmitter<std::string>::emit,
                  emscripten::allow_raw_pointers());

    

    
}
