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

class OtherClass {
    int foo_ = 7;

   public:
    int getFoo() { return foo_; }

    void setFoo(int f) { foo_ = f; }
};

/**
 * @brief This is a simple do-nothing class meant to test the `CppObjectSignalObject` class' ability
 * to use getter and setter method pointers to act as initiation points or endpoints for signals.
 *
 */
class TestObj : public Ticker {
    int i_ = 3;
    double d_ = 3.5;
    double dblval_ = 3.14159;
    std::string s_ = "foo";
    int width_ = 13;

   public:
    virtual void tick() {
        // Ticker::tick();
        cout << "TestObj ticks!" << endl;
        dblval_ += 0.01;
    }    

    /**
     * @brief This installs this class' `tick()` method to be called from the `setInterval()` JS
     * function.
     *
     */
    virtual void generateTickFunction() { tickJS_ = val::global("TickerTick")(*this); }

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
        cout << "TestObj::getDblval(): " << dblval_ << endl;
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

    // auto ticker = Ticker(700, true);
    // auto ticker = Ticker();
    // ticker.start();

    auto obj = make_shared<TestObj>();

    // obj->generateTickFunction();
    obj->start();

    auto appBldr = make_shared<AppBuilder>();
    // obj->signalEmitterTestMethod();
    auto objIntChannel =
        make_shared<ObjectChannel<TestObj, int>>(obj, "objIntChannel", &TestObj::setWidth, &TestObj::getWidth);
    auto objDblChannel =
        make_shared<ObjectChannel<TestObj, double>>(obj, "objDblChannel", &TestObj::setDblval, &TestObj::getDblval);

    auto widthInput = appBldr->textField("widthInput");
    auto syncTestInput = appBldr->textField("syncTestInput");
    auto dblInput = appBldr->rangeInput("dblInput");
    // auto textField2 = appBldr->textField("TF2");
    // auto textField3 = appBldr->textField("TF3");

    appBldr->addObject(obj);
    appBldr->addChannel(objDblChannel);

    auto c3 = appBldr->makeChannel("c3");
    auto c4 = appBldr->makeChannel("c4");
    // c3->addConnection(c4);

    std::function<int()> getWidthFromTestObjFn = [&]() { return obj->getWidth(); };
    auto fnid = appBldr->addIntFunction(getWidthFromTestObjFn);

    cout << "getWidthFromTestObjFn id is " << fnid << endl;
    // c4->addConnection(objIntChannel);

    // appBldr->printGroup("g1");

    auto widthChannel = make_shared<WebElementChannel>("widthChannel");
    auto dblChannel = make_shared<WebElementChannel>("dblChannel");
    widthChannel->installWebElement(widthInput);
    dblChannel->installWebElement(dblInput);
    auto syncTestInputChannel = make_shared<WebElementChannel>("syncTestInputChannel");
    syncTestInputChannel->installWebElement(syncTestInput);
    // val elg = val::global("elgCallMethodOnObjByName");
    // val onChangeFn = elg(*widthChannel, val("inject"));
    // widthInput->addEventListener(val("change"), onChangeFn);

    widthChannel->addConnection(objIntChannel);

    widthChannel->finalize();

    dblChannel->addConnection(objDblChannel);

    dblChannel->finalize();

    objDblChannel->addConnection(syncTestInputChannel);

    objDblChannel->finalize();
    syncTestInputChannel->finalize();

    // c3->inject(val("BOO"));
    c4->inject(val(5));
    // widthInput->addConnection(textField2);
    vector<const int> groupIds = appBldr->defineCurrentGroup("g1");
    appBldr->printGroup("g1");
    // auto range1 = make_shared<RangeInput>("R1", "R1-id");

    // cout << "Calling the int function I installed earlier: " << appBldr->intFunctions_[6]() <<
    // endl;

    // shared_ptr<void> getDoubleFromTestObjFnVptr =
    //     make_shared<std::function<double()>>([&]() { return obj->getDblval(); });
    // // shared_ptr<void> getDoubleFromTestObjFnVptr = getDoubleFromTestObjFn;
    // auto dblFnId = appBldr->addObject(getDoubleFromTestObjFnVptr);

    // std::function<double()> fn = appBldr->objects_[dblFnId];

    // fn();

    auto fooObj = make_shared<OtherClass>();

    cout << "We have added our object channel!" << endl;
    appBldr->initTick();
    // std::function<int()> getFoo = [&]() { return fooObj->getFoo(); };
    // auto fnidFoo = appBldr->addIntFunction(getFoo);

    // cout << "Calling the OTHER int function I installed earlier: "
    //      << appBldr->intFunctions_[fnidFoo]() << endl;
    // return 0;

    // std::function<void(int)> setFoo = [&](int i) { fooObj->setFoo(i); };
    // auto fnidSetFoo = appBldr->addSetIntFunction(setFoo);

    // appBldr->setIntFunctions_[fnidSetFoo](5);

    // cout << "Calling the OTHER int function I installed earlier (again): "
    //      << appBldr->intFunctions_[fnidFoo]() << endl;

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

    emscripten::class_<TestObj>("TestObj")
        .function("signalEmitterTestMethod", &TestObj::signalEmitterTestMethod)
        //.function("tick", &TestObj::tick, emscripten::allow_raw_pointers());
        .function("tick", &TestObj::tick);
}

EMSCRIPTEN_BINDINGS(Channel) {
    emscripten::class_<Channel>("Channel")
        .function("inject", &Channel::inject, emscripten::allow_raw_pointers())
        //.function("injectCppval", &Channel::injectCppval, emscripten::allow_raw_pointers())
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