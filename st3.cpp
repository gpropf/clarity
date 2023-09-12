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

#include "AppBuilder.hpp"
#include "GridControl.hpp"
#include "Signal.hpp"
#include "SignalPrimitives.hpp"
#include "Util3.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

using std::cout;
using std::string;
using std::to_string;
using std::vector;

using namespace cl3;

namespace cl3 {

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
}  // namespace cl3

int main() {
    // We make a test object and then a signal wrapper for it.
    // TestObj tobj;
    auto tobjSptr = make_shared<TestObj>();

    auto appBldr = make_shared<AppBuilder>();
    // tobjSptr->signalEmitterTestMethod();

    val elg = val::global("elgCallMethodOnObjByName");
    val onChangeFn = elg(*tobjSptr, val("signalEmitterTestMethod"));

    val textField1 = appBldr->textField("TF1");
    val textField2 = appBldr->textField("TF2");
    val textField3 = appBldr->textField("TF3");

    vector<const int> groupIds = appBldr->defineCurrentGroup("g1");
    appBldr->printGroup("g1");
    appBldr->printGroup("g1");
    textField1.call<void>("addEventListener", val("change"), onChangeFn);

    auto range1 = make_shared<RangeInput>("R1", "R1-id");

    return 0;
}

EMSCRIPTEN_BINDINGS(TestObj) {
    // emscripten::class_<cl2::CppObjectSignalObject<std::string, TestObj>>("CppObjectSignalObject")
    //     .function("emit", &cl2::CppObjectSignalObject<std::string, TestObj>::emit,
    //               emscripten::allow_raw_pointers())
    //     .function("getSignal", &cl2::CppObjectSignalObject<std::string, TestObj>::getSignal,
    //               emscripten::allow_raw_pointers())
    //     .function("accept", &cl2::CppObjectSignalObject<std::string, TestObj>::accept,
    //               emscripten::allow_raw_pointers());

    emscripten::class_<TestObj>("TestObj").function("signalEmitterTestMethod",
                                                    &TestObj::signalEmitterTestMethod);
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
// namespace cl3