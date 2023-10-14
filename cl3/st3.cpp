/**
 * @file signalTest.cpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief A test program to try out and debug the library.
 * @version 0.2
 * @date 2023-10-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <memory>

#include "AppBuilder.hpp"
#include "TestClasses.hpp"
#include "Util3.hpp"

using std::cout;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;

using namespace cl3;

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

// AppBuilder *AppBuilder::singleton__ = nullptr;
// map<const string, vector<const int>> AppBuilder::groups__;
// map<const int, shared_ptr<WebElement>> AppBuilder::webElements__;
// map<const int, shared_ptr<Channel>> AppBuilder::channels__;
// vector<const int> AppBuilder::currentGroupIds__;
// vector<const int> AppBuilder::allIds__;

int main() {
    auto appBldr = make_shared<AppBuilder>();

    // We make a test object and then tell the appBldr about it.
    auto obj = make_shared<TestObj>();
    auto objId = appBldr->addObject(obj);
    cout << "TestObj added to AppBuilder has id: " << objId << endl;

    // Code to create the width field and associate it with a channel.
    auto [widthInput, widthInputId] = appBldr->textField("widthInput");
    auto [widthInputChannel, widthInputChannelId] =
        appBldr->makeWebElementChannel("widthInputChannel");
    widthInputChannel->installWebElement(widthInput);

    // Code to create the height field and associate it with a channel.
    auto [heightInput, heightInputId] = appBldr->textField("heightInput");
    auto [heightInputChannel, heightInputChannelId] =
        appBldr->makeWebElementChannel("heightInputChannel");
    heightInputChannel->installWebElement(heightInput);

    // These are the endpoints that allow the values we input in the text fields to actually reach
    // and update the corresponding values in TestObj.
    auto objHeightChannel = make_shared<ObjectChannel<TestObj, double>>(
        obj, "objHeightChannel", &TestObj::setDblval, &TestObj::getDblval);
    auto objWidthChannel = make_shared<ObjectChannel<TestObj, int>>(
        obj, "objWidthChannel", &TestObj::setWidth, &TestObj::getWidth);

    appBldr->addChannel(objHeightChannel);
    appBldr->addChannel(objWidthChannel);

    heightInputChannel->addConnection(objHeightChannel);
    widthInputChannel->addConnection(objWidthChannel);

    objWidthChannel->finalize();
    objHeightChannel->finalize();
    heightInputChannel->finalize();
    widthInputChannel->finalize();

    val onClickFn = val::global("sayHello");    

    auto [tickBtn, tickBtnId] = appBldr->button("tickButton", "Tick Once!", onClickFn);
    cout << "tickBtnId: " << tickBtnId << endl;

    val getTickFn = val::global("getTickFn");
    val appBldrTickFn = getTickFn(appBldr);

    tickBtn->addEventListener(val("click"), appBldrTickFn);    
    

    // val setDblvalOnObj = val::global("setDblvalOnObj");

    // obj->setDblval(5.0);

    // setDblvalOnObj(*obj, 1.3);
    // ------------------

    // testObjSetDblval(obj, 4.0);

   // cout << "obj->getDblVal(): " << obj->getDblval();

    // cout << "testObjGetDblval(): " << testObjGetDblval(*obj).as<double>();

    vector<const int> groupIds = appBldr->defineCurrentGroup("g1");
    cout << "Printing contents of group g1." << endl;
    appBldr->printGroup("g1", "\t");

    // val pairChannelWithElement = val::global("pairChannelWithElement");
    // pairChannelWithElement(heightInputChannelId, syncTestInputId);

    appBldr->listWebElements();

   // TestObj *newt = new TestObj();
    // val transfer = val::global("Module.TestObj.transfer");
    //  val newtJS = transfer(*newt);
    //   AppBuilder::setSingleton(&*appBldr);

    // val setInterval = val::global("setInterval");
    // val getClosureOnObj = val::global("getTickerFn");

    //TestObj *rawt = obj.get();

    // val badfn = val::global("getTickerFn")(rawt);
    //  val tickfn = getClosureOnObj(newt);
    // setInterval(badfn, val(3000));
    //  tickfn();
  //  cout << "On the C++ side newt says: " << newt->getDblval() << endl;

    // AppBuilder::tick__();

    appBldr->start(3000);
    obj->start(1000);
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
        .smart_ptr<std::shared_ptr<TestObj>>("TestObj")
        .function("signalEmitterTestMethod", &TestObj::signalEmitterTestMethod)
        //.function("tick", &TestObj::tick, emscripten::allow_raw_pointers());
        .function("setDblval", &TestObj::setDblval, emscripten::allow_raw_pointers())
        .function("getDblval", &TestObj::getDblval, emscripten::allow_raw_pointers())
        // .class_function("setDblvalOnObj", &TestObj::setDblvalOnObj,
        //                 emscripten::allow_raw_pointers())
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