//#include "testbed.hpp"

#include "SimpleTest.hpp"

#include <emscripten/wasm_worker.h>
#include <stdio.h>

#include <map>

#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"
#include "PageContent.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "globals.hpp"

void run_in_worker() { printf("Hello from wasm worker!\n"); }

int main() {
    emscripten_wasm_worker_t worker = emscripten_malloc_wasm_worker(/*stack size: */ 1024);
    emscripten_wasm_worker_post_function_v(worker, run_in_worker);
    // SimpleTest simpleTest;
    // TestFramework testFramework;
    // testFramework.content(simpleTest.content());
    return 0;
}
