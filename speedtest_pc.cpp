

#include "Speedtest_pc.hpp"

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
//#include "embindings.hpp"
#include "globals.hpp"

EMSCRIPTEN_BINDINGS(my_class_example) {
    class_<MyClass>("MyClass")
        .constructor<int, std::string>()
        .function("incrementX", &MyClass::incrementX)
        .property("x", &MyClass::getX, &MyClass::setX)
        .class_function("getStringFromInstance", &MyClass::getStringFromInstance);
}

EMSCRIPTEN_BINDINGS(Speedtest) {
    class_<Speedtest>("Speedtest")
        // .constructor<string>(allow_raw_pointers())
        // .constructor<>(allow_raw_pointers())
        .class_function("cppTestFn", &Speedtest::cppTestFn)
        // .class_function("destroyEverything_this", &Speedtest::destroyEverything_this,
        //                 allow_raw_pointers())
        .class_function("destroyEverything_shell", &Speedtest::destroyEverything_shell,
                        allow_raw_pointers())
        .function("destroyEverything", &Speedtest::destroyEverything, allow_raw_pointers())
        // .function("showname", &Speedtest::showname, allow_raw_pointers())

        .function("runLambda", &Speedtest::runLambda, allow_raw_pointers())
        .function("doNothing", &Speedtest::doNothing, allow_raw_pointers())
        .class_function("runUpdateTotalFields", &Speedtest::runUpdateTotalFields,
                        allow_raw_pointers());

    class_<HybridNode<Speedtest>>("HybridNode_st")
        // The type code that comes back from typeid.name() is literally
        // 'NSt3__212basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE' so we're just going to
        // use _str as the suffix for this one.
        .function("doNothing", &HybridNode<Speedtest>::doNothing, allow_raw_pointers());
}

template <>
string HybridNode<Speedtest>::cppValToString() const {
    // cout << "HybridNode<string>::cppValToString()\n";
    if (cppVal_ == nullptr) return "HybridNode<Speedtest> NULLPTR";
    return string("Speedtest object '" + this->name_ + "' currently set up to generate ") +
           clto_str(*cppVal_->nTotalFields) +
           string(" fields\n");  // static_cast<string>(cppVal_);
}

Speedtest *Speedtest::singleton = nullptr;

int main() {
    Speedtest speedtest;
    Speedtest::singleton = &speedtest;
    TestFramework testFramework;
    testFramework.content(speedtest.content());
    return 0;
}
