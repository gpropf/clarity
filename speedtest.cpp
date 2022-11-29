

#include "Speedtest.hpp"

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
//#include "embindings.hpp"
#include <emscripten/wasm_worker.h>

#include "globals.hpp"

// template <>
// string HybridNode<Speedtester>::cppValToString() const {
//     // cout << "HybridNode<string>::cppValToString()\n";
//     if (cppVal_ == nullptr) return "HybridNode<Speedtest> NULLPTR";
//     return string("Speedtest object '" + this->name_ + "' currently set up to generate ") +
//            clto_str(*cppVal_->nTotalFields) + string(" fields\n");  //
//            static_cast<string>(cppVal_);
// }

// Speedtest *Speedtest::singleton = nullptr;

std::function<void(val ev)> Speedtest::updateStateStatic_;

void run_in_worker() { cout << "Hello from wasm worker!\n"; }

int main() {
    //emscripten_wasm_worker_t worker = emscripten_malloc_wasm_worker(/*stack size: */ 1024);
    //emscripten_wasm_worker_post_function_v(worker, run_in_worker);
    Speedtest speedtest;
    // Speedtest::singleton = &speedtest;
    TestFramework testFramework;
    testFramework.content(speedtest.content());
    return 0;
}
