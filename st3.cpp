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

// #include "MonolithicSignals.hpp"

#include "GridControl.hpp"
#include "Signal.hpp"
#include "SignalBuilder3.hpp"
#include "SignalPrimitives.hpp"
#include "Util3.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

using namespace cl3;

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

    std::string signalEmitterTestMethod() {
        cout << "TestObj::signalEmitterTestMethod(): " << s_ << endl;
        return s_;
    }

    /**
     * @brief We're also using the TestObj class to test the MouseSignal class.
     *
     * @param mouseLocation
     */
    void mouseClickAcceptorMethod(const std::pair<double, double> &mouseLocation) {
        cout << "TestObj::mouseClickAcceptorMethod(): x = " << mouseLocation.first
             << ", y = " << mouseLocation.second << endl;
    }

    std::string getS() { return s_; }
};

int main() {
    // We make a test object and then a signal wrapper for it.
    TestObj tobj;
    auto tobjSptr = make_shared<TestObj>();

    // Now we're going to create a JS timer to read the value and fire the signal every so often.
    // val testObjEmitter = val::global("elgTestObjEmitter")(val(*testObjCSO));
    // val setInterval = val::global("setInterval");
    // setInterval(testObjEmitter, 500);

   
    return 0;
}

EMSCRIPTEN_BINDINGS(CppObjectSignalObject) {
    // emscripten::class_<cl2::CppObjectSignalObject<std::string, TestObj>>("CppObjectSignalObject")
    //     .function("emit", &cl2::CppObjectSignalObject<std::string, TestObj>::emit,
    //               emscripten::allow_raw_pointers())
    //     .function("getSignal", &cl2::CppObjectSignalObject<std::string, TestObj>::getSignal,
    //               emscripten::allow_raw_pointers())
    //     .function("accept", &cl2::CppObjectSignalObject<std::string, TestObj>::accept,
    //               emscripten::allow_raw_pointers());

    emscripten::class_<cl2::Merge<std::string, std::string, std::string>>("Merge").function(
        "recompute", &cl2::Merge<std::string, std::string, std::string>::recompute);

    emscripten::class_<cl2::EventListenerEmitter<std::string>>("EventListenerEmitter")
        .function("emit", &cl2::EventListenerEmitter<std::string>::emit,
                  emscripten::allow_raw_pointers());

    emscripten::class_<cl2::ObjectEmitter<std::string, TestObj>>("ObjectEmitter")
        .function("emit", &cl2::ObjectEmitter<std::string, TestObj>::emit,
                  emscripten::allow_raw_pointers());
}

EMSCRIPTEN_BINDINGS(WebElementSignalObject) {
    // emscripten::class_<WebElementSignalObject<std::string>>("WebElementSignalObject")
    //     .function("emit", &WebElementSignalObject<std::string>::emit,
    //               emscripten::allow_raw_pointers())
    //     .function("accept", &WebElementSignalObject<std::string>::accept,
    //               emscripten::allow_raw_pointers());

    // emscripten::class_<WebElementSignal<std::string>>("WebElementSignal")
    //     .function("emit", &WebElementSignal<std::string>::emit, emscripten::allow_raw_pointers())
    //     .function("accept", &WebElementSignal<std::string>::accept,
    //               emscripten::allow_raw_pointers());
}
