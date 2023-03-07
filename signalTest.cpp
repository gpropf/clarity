/**
 * @file signalTest.cpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief A test program to try out and debug the library.
 * @version 0.1
 * @date 2023-03-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "MonolithicSignals.hpp"
#include "Signal.hpp"
#include "SignalBuilder.hpp"
#include "SignalPrimitives.hpp"
#include "Util.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

#include "GridControl.hpp"

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

    const std::string signalEmitterTestMethod() {
        cout << "TestObj::signalEmitterTestMethod(): " << s_ << endl;
        return s_;
    }

    void mouseAcceptorTestMethod(const std::pair<double, double> &mouseLocation) {
        cout << "TestObj::mouseAcceptorTestMethod(): x = " << mouseLocation.first
             << ", y = " << mouseLocation.second << endl;
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
    // setInterval(testObjEmitter, 500);

    auto objectEmitter = make_shared<ObjectEmitter<std::string, TestObj>>(tobjSptr);
    objectEmitter->setSignalEmitterMethod(&TestObj::signalEmitterTestMethod);
    val jsEmitterFn = val::global("elgObjEmitter")(val(*objectEmitter));
    //setInterval(jsEmitterFn, 500);

    // It's easy to get control of elements that are already present in a static HTML page by
    // creating a WE from the id.
    const auto capturedDiv = cl2::WebElement("555");

    // We can also create de-novo elements.
    auto generatedDiv = cl2::WebElement("div", "generatedDiv", getStrId());

    // We're now using the SignalBuilder factory objects to create our web content.
    cl2::SignalBuilder sb = cl2::SignalBuilder();

    auto objectEmitterTextInput = sb.textInputWSS<std::string>(
        "objectEmitterTextInput", "Enter a new value for the string (s_) stored in the TestObj.");

    sb.connect<std::string>(objectEmitter, objectEmitterTextInput);

    auto objectAcceptor = make_shared<ObjectAcceptor<std::string, TestObj>>(tobjSptr);
    objectAcceptor->setSignalAcceptorMethod(&TestObj::signalAcceptorTestMethod);
    sb.connect<std::string>(objectEmitterTextInput, objectAcceptor);

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

    
    //mfork1->addOutput(objectAcceptor);

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
    svg.setAttributes(
        {{"viewBox", val("10 15 100 100")}, {"style", val("border: 1px solid black")}});
    // svg.setAttributes({{"style", val("border: 1px solid black")}});

    auto mouseSignal = make_shared<MouseSignal<std::pair<double, double>>>(svg, "click");

    auto svgMouseClickAcceptor =
        make_shared<ObjectAcceptor<std::pair<double, double>, TestObj>>(tobjSptr);
    svgMouseClickAcceptor->setSignalAcceptorMethod(&TestObj::mouseAcceptorTestMethod);

    sb.connect<std::pair<double, double>>(mouseSignal, svgMouseClickAcceptor);

    const auto circle1 = cl2::WebElement("circle", "circle1", getStrId(), svg.domElement_);
    circle1.domElement_.call<void>("setAttribute", val("r"), val(45));
    circle1.domElement_.call<void>("setAttribute", val("cx"), val(120));
    circle1.domElement_.call<void>("setAttribute", val("cy"), val(100));
    circle1.domElement_.call<void>("setAttribute", val("fill"), val("#ff0000"));


    auto rect1 = Rect("rect1", 23, 31, 40, 50, "#ffaa99", "blue", true, getStrId(), svg.getDomElement());

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

    // auto dblInputELE =
    //     sb.textInputELE<std::string>("dblInput", "Enter a floating point number", false);

    auto dblInputWSS =
        sb.textInputWSS<std::string>("dblInput", "Enter a floating point number", false);

    sb.connect<std::string>(dblInputWSS, strToNumTransformerSS);

    // Connect the input field to the conversion function
    // sb.connect<std::string>(dblInputWSO, strToNumTransformer);

    // Here we're going back to our TestObj and creating a field that will allow the user to update
    // the string value it contains.
    // const auto testObjValTextInputWSO = sb.textInput<std::string>(
    //     "testObjValTextInput", "Enter a new value for the string stored in the TestObj.");

    // sb.connect<std::string>(testObjValTextInputWSO, testObjCSO);
    // sb.connect<std::string>(testObjCSO, testObjValTextInputWSO);

    auto mergeFn = [](std::string s1, std::string s2) { return s1 + s2; };
    auto mergeSignal = make_shared<MergeSS<std::string, std::string, std::string>>(mergeFn);

    const auto m1InputWSO = sb.textInputWSS<std::string>("m1Input", "Enter the first value", false);
    const auto m2InputWSO = sb.textInputWSS<std::string>("m2Input", "Enter the second value", false);
    const auto mergeOutWSO =
        sb.textInputWSS<std::string>("mergeOut", "Output of merged signals goes here", false);

    sb.connect<std::string, std::string, std::string>(m1InputWSO, m2InputWSO, mergeSignal,
                                                      mergeOutWSO);

    val recomputeMergeFn = val::global("elgMergeRecompute")(val(*mergeSignal));
    const auto mergeRecomputeButton = sb.button("Recompute", recomputeMergeFn);

    auto gridControl = make_shared<GridControl<int>>(60,40, 600,400, sb, "gc1");
    //svgMouseClickAcceptor_->setObjectPointer(this->shared_from_this());
    
    gridControl->addColorToPallete(0, "#000000");
    gridControl->addColorToPallete(1, "#ff0000");
    gridControl->addColorToPallete(2, "#00ff00");
    gridControl->addColorToPallete(3, "#0000ff");
    gridControl->finalize();

    const auto selectOutput = sb.textInputWSS<std::string>("selectOutput", "Output of select box appears here.", false);
    auto select1 = Select("select1", getStrId());
    auto option1 = Option("1", "Foo", select1.getDomElement());
    auto option2 = Option("2", "Bar", select1.getDomElement());
    auto option3 = Option("3", "Baz", select1.getDomElement());

    auto selectEmitter = make_shared<SelectEmitter<std::string>>(select1.getDomElement());
    selectEmitter->setOutput(selectOutput);
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

    emscripten::class_<cl2::MergeSS<std::string, std::string, std::string>>("Merge").function(
        "recompute", &cl2::MergeSS<std::string, std::string, std::string>::recompute);

    emscripten::class_<cl2::EventListenerEmitter<std::string>>("EventListenerEmitter")
        .function("emit", &cl2::EventListenerEmitter<std::string>::emit,
                  emscripten::allow_raw_pointers());

    emscripten::class_<cl2::ObjectEmitter<std::string, TestObj>>("ObjectEmitter")
        .function("emit", &cl2::ObjectEmitter<std::string, TestObj>::emit,
                  emscripten::allow_raw_pointers());
}

EMSCRIPTEN_BINDINGS(WebElementSignalObject) {
    emscripten::class_<WebElementSignalObject<std::string>>("WebElementSignalObject")
        .function("emit", &WebElementSignalObject<std::string>::emit,
                  emscripten::allow_raw_pointers())
        .function("accept", &WebElementSignalObject<std::string>::accept,
                  emscripten::allow_raw_pointers());

    emscripten::class_<WebElementSignalObjectSS<std::string>>("WebElementSignalObjectSS")
        .function("emit", &WebElementSignalObjectSS<std::string>::emit,
                  emscripten::allow_raw_pointers())
        .function("accept", &WebElementSignalObjectSS<std::string>::accept,
                  emscripten::allow_raw_pointers());

    emscripten::class_<JSFunctionSignalObject<std::string>>("ConsoleLoggerSignalObject")
        .function("emit", &JSFunctionSignalObject<std::string>::emit,
                  emscripten::allow_raw_pointers())
        .function("accept", &JSFunctionSignalObject<std::string>::accept,
                  emscripten::allow_raw_pointers());

    emscripten::class_<SignalObject<std::string>>("SignalObject")
        .function("emit", &SignalObject<std::string>::emit, emscripten::allow_raw_pointers())
        .function("accept", &SignalObject<std::string>::accept, emscripten::allow_raw_pointers());
}
