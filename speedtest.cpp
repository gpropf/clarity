

#include "speedtest.hpp"

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
//#include "embindings.hpp"
#include "globals.hpp"

EMSCRIPTEN_BINDINGS(speedtest) {
    class_<Speedtest>("Speedtest")
        .class_function("cppTestFn", &Speedtest::cppTestFn)
        // .class_function("destroyEverything_this", &Speedtest::destroyEverything_this,
        //                 allow_raw_pointers())
        .class_function("destroyEverything_shell", &Speedtest::destroyEverything_shell,
                        allow_raw_pointers())
        .function("destroyEverything", &Speedtest::destroyEverything, allow_raw_pointers())

        .function("runLambda", &Speedtest::runLambda, allow_raw_pointers())
        .function("runUpdateTotalFields", &Speedtest::runUpdateTotalFields, allow_raw_pointers());
}

Speedtest *Speedtest::singleton = nullptr;

int main() {
    Speedtest speedtest;
    Speedtest::singleton = &speedtest;
    TestFramework testFramework;
    testFramework.content(speedtest.content());
    return 0;
}
