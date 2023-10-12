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
   public:
    int foo_ = 7;
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
    static TestObj *transfer(TestObj *t) { return t; }

    virtual void tick() {
        // dblval_ += 0.1;
        auto d = getDblval();
        d += 0.1;
        setDblval(d);
        cout << "TestObj ticks! " << dblval_ << endl;
    }

    /**
     * @brief Does the same thing as tick() but calling it from a hook function doesn't seem to
     * produce errors.
     *
     */
    void update() {
        auto d = getDblval();
        d += 0.1;
        setDblval(d);
        cout << "TestObj updates! " << dblval_ << endl;
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

    static void setDblvalOnObj(TestObj *tptr, double v) { tptr->setDblval(v); }

    double getDblval() const {
        // if (dblval_ > 4) {
        //     cout << "This is one thread..." << endl;
        // } else {
        //     cout << "This is the other thread..." << endl;
        // }
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

void *thread_callback(void *arg) {
    // while (true) {
    //     sleep(1);
    //     // printf("Inside the thread: %d\n", *(int *)arg);
    //     AppBuilder *abptr = static_cast<AppBuilder *>(arg);
    //     abptr->threadTestFn();
    // }

    sleep(1);
    printf("Inside the thread: %d\n", *(int *)arg);
    return NULL;
}

void *thread_callback2(void *arg) {
    // sleep(1);
    //  printf("Inside the thread: %d\n", *(int *)arg);
    OtherClass *ocptr = static_cast<OtherClass *>(arg);

    while (1) {
        auto foo = ocptr->getFoo();
        cout << "ocptr->getFoo(): " << foo << endl;
        ocptr->foo_ += 3;
        sleep(1);
    }
    return NULL;
}

void *thread_callback3(void *arg) {
    // sleep(1);
    //  printf("Inside the thread: %d\n", *(int *)arg);
    // OtherClass *ocptr = static_cast<OtherClass *>(arg);
    while (1) {
        AppBuilder *abptr = static_cast<AppBuilder *>(arg);
        abptr->threadTestFn();
        sleep(1);
    }
    // while (1) {
    //     auto foo = ocptr->getFoo();
    //     cout << "ocptr->getFoo(): " << foo << endl;
    //     ocptr->foo_ += 3;
    //     sleep(1);
    // }
    return NULL;
}

void *thread_callback4(void *arg) {
    // sleep(1);
    //  printf("Inside the thread: %d\n", *(int *)arg);
    // OtherClass *ocptr = static_cast<OtherClass *>(arg);
    while (1) {
        AppBuilder *abptr = static_cast<AppBuilder *>(arg);
        abptr->doNothing();
        sleep(1);
    }
    // while (1) {
    //     auto foo = ocptr->getFoo();
    //     cout << "ocptr->getFoo(): " << foo << endl;
    //     ocptr->foo_ += 3;
    //     sleep(1);
    // }
    return NULL;
}

AppBuilder *AppBuilder::singleton__ = nullptr;
map<const string, vector<const int>> AppBuilder::groups__;
map<const int, shared_ptr<WebElement>> AppBuilder::webElements__;
map<const int, shared_ptr<Channel>> AppBuilder::channels__;
vector<const int> AppBuilder::currentGroupIds__;
vector<const int> AppBuilder::allIds__;

int main() {
    // We make a test object and then a signal wrapper for it.

    auto obj = make_shared<TestObj>();

    auto appBldr = make_shared<AppBuilder>();
    cout << "Our AppBuilder is now the singleton!" << endl;
    AppBuilder::setSingleton(&*appBldr);

    auto [widthInput, widthInputId] = AppBuilder::textField__("widthInput");
    cout << "widthInputId: " << widthInputId << endl;

    auto [widthInputChannel, widthInputChannelId] =
        appBldr->makeWebElementChannel("widthInputChannel");
    widthInputChannel->installWebElement(widthInput);

    auto [syncTestInput, syncTestInputId] = AppBuilder::textField__("syncTestInput");
    cout << "syncTestInputId: " << syncTestInputId << endl;

    auto objDblChannel = make_shared<ObjectChannel<TestObj, double>>(
        obj, "objDblChannel", &TestObj::setDblval, &TestObj::getDblval);

    auto objIntChannel = make_shared<ObjectChannel<TestObj, int>>(
        obj, "objIntChannel", &TestObj::setWidth, &TestObj::getWidth);

    auto objId = appBldr->addObject(obj);
    cout << "TestObj added to AppBuilder has id: " << objId << endl;
    appBldr->addChannel(objDblChannel);
    appBldr->addChannel(objIntChannel);

    auto [pairingChannel, pairingChannelId] = appBldr->makeWebElementChannel("pairingChannel");
    cout << "pairingChannelId: " << pairingChannelId << endl;
    pairingChannel->installWebElement(syncTestInput);

    pairingChannel->addConnection(objDblChannel);
    widthInputChannel->addConnection(objIntChannel);

    objDblChannel->finalize();
    pairingChannel->finalize();
    widthInputChannel->finalize();

    val onClickFn = val::global("sayHello");

    val appBuilderSingletonTick = val::global("appBuilderSingletonTick");
    // appBuilderSingletonTick();

    auto [tickBtn, tickBtnId] = appBldr->button("tickButton", "Tick Once!", onClickFn);
    cout << "tickBtnId: " << tickBtnId << endl;

    val installEventListenerById = val::global("installEventListenerById");
    installEventListenerById(tickBtnId);

    auto otherClassObj = make_shared<OtherClass>();

    val appBuilderSingletonStart = val::global("appBuilderSingletonStart");
    // appBuilderSingletonStart(1000);

    val testObjSetDblval = val::global("testObjSetDblval");
    val testObjGetDblval = val::global("testObjGetDblval");

    val setDblvalOnObj = val::global("setDblvalOnObj");

    obj->setDblval(5.0);

    setDblvalOnObj(*obj, 1.3);

    // testObjSetDblval(obj, 4.0);

    cout << "obj->getDblVal(): " << obj->getDblval();

    cout << "testObjGetDblval(): " << testObjGetDblval(*obj).as<double>();

    vector<const int> groupIds = appBldr->defineCurrentGroup("g1");
    cout << "Printing contents of group g1." << endl;
    appBldr->printGroup("g1", "\t");

    // val pairChannelWithElement = val::global("pairChannelWithElement");
    // pairChannelWithElement(pairingChannelId, syncTestInputId);

    appBldr->listWebElements();

    TestObj *newt = new TestObj();
    // val transfer = val::global("Module.TestObj.transfer");
    //  val newtJS = transfer(*newt);
    //   AppBuilder::setSingleton(&*appBldr);

    val setInterval = val::global("setInterval");
    val getClosureOnObj = val::global("getTickerFn");

    TestObj *rawt = obj.get();

    val badfn = val::global("getTickerFn")(rawt);
    // val tickfn = getClosureOnObj(newt);
    setInterval(badfn, val(3000));
    // tickfn();
    cout << "On the C++ side newt says: " << newt->getDblval() << endl;

    AppBuilder::tick__();

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
        .function("setDblval", &TestObj::setDblval, emscripten::allow_raw_pointers())
        .function("getDblval", &TestObj::getDblval, emscripten::allow_raw_pointers())
        .class_function("setDblvalOnObj", &TestObj::setDblvalOnObj,
                        emscripten::allow_raw_pointers())
        .class_function("transfer", &TestObj::transfer, emscripten::allow_raw_pointers())
        .function("tick", &TestObj::tick);
}

EMSCRIPTEN_BINDINGS(Channel) {
    // emscripten::class_<Channel>("Channel")
    //     .function("inject", &Channel::inject, emscripten::allow_raw_pointers())
    //     //.function("injectCppval", &Channel::injectCppval, emscripten::allow_raw_pointers())
    //     .function("testMethod", &Channel::testMethod, emscripten::allow_raw_pointers());
    // //     .function("accept", &WebElementSignalObject<std::string>::accept,
    // //               emscripten::allow_raw_pointers());

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