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

#include <memory>

#include "AppBuilder.hpp"
#include "Util3.hpp"

using std::cout;
using std::make_shared;
using std::shared_ptr;
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
    double dblval_ = 0;
    std::string s_ = "foo";
    int width_ = 1;

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

    void setWidth(const int &w) {
        cout << "TestObj::setWidth(): " << w << endl;
        width_ = w;
    }

    int getWidth() const {
        cout << "TestObj::getWidth(): " << width_ << endl;
        return width_;
    }

    void setDblval(const double &w) {
        cout << "TestObj::setDblval(): " << w << endl;
        dblval_ = w;
    }

    double getDblval() const {
        cout << "TestObj::getDblval(): " << width_ << endl;
        return dblval_;
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
    auto obj = make_shared<TestObj>();

    auto appBldr = make_shared<AppBuilder>();
    // obj->signalEmitterTestMethod();
    auto objIntChannel = make_shared<ObjectChannel<TestObj, int>>(obj, &TestObj::setWidth, &TestObj::getWidth);
     auto objDblChannel = make_shared<ObjectChannel<TestObj, double>>(obj, &TestObj::setDblval, &TestObj::getDblval);

    auto widthInput = appBldr->textField("widthInput");
    auto dblInput = appBldr->textField("dblInput");
    // auto textField2 = appBldr->textField("TF2");
    // auto textField3 = appBldr->textField("TF3");

    appBldr->addObject(obj);

    auto c3 = appBldr->makeChannel("c3");
    auto c4 = appBldr->makeChannel("c4");
    // c3->addConnection(c4);

    // c4->addConnection(objIntChannel);

    // appBldr->printGroup("g1");

    auto widthChannel = make_shared<WebElementChannel>("widthChannel");
    auto dblChannel = make_shared<WebElementChannel>("dblChannel");
    widthChannel->installWebElement(widthInput);
    dblChannel->installWebElement(dblInput);
    // val elg = val::global("elgCallMethodOnObjByName");
    // val onChangeFn = elg(*widthChannel, val("inject"));
    // widthInput->addEventListener(val("change"), onChangeFn);

    widthChannel->addConnection(objIntChannel);

    widthChannel->finalize();

    dblChannel->addConnection(objDblChannel);

    dblChannel->finalize();

    // c3->inject(val("BOO"));
    c4->inject(val(5));
    // widthInput->addConnection(textField2);
    vector<const int> groupIds = appBldr->defineCurrentGroup("g1");
    appBldr->printGroup("g1");
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

EMSCRIPTEN_BINDINGS(Channel) {
    emscripten::class_<Channel>("Channel")
        .function("inject", &Channel::inject, emscripten::allow_raw_pointers())
        //.function("injectCppval", &Channel::injectCppval, emscripten::allow_raw_pointers())
        //.function("injectForWEC", &Channel::injectForWEC, emscripten::allow_raw_pointers())
        .function("testMethod", &Channel::testMethod, emscripten::allow_raw_pointers());
    //     .function("accept", &WebElementSignalObject<std::string>::accept,
    //               emscripten::allow_raw_pointers());

    emscripten::class_<WebElementChannel>("WebElementChannel")
        .function("inject", &WebElementChannel::inject, emscripten::allow_raw_pointers());
    //  .function("injectCppval", &WebElementChannel::injectCppval,
    //  emscripten::allow_raw_pointers());
    // emscripten::class_<WebElementSignal<std::string>>("WebElementSignal")
    //     .function("emit", &WebElementSignal<std::string>::emit, emscripten::allow_raw_pointers())
    //     .function("accept", &WebElementSignal<std::string>::accept,
    //               emscripten::allow_raw_pointers());
}
// namespace cl3