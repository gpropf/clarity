//#include "testbed.hpp"

#include "Showcase-pthread.hpp"

//#include <emscripten/wasm_worker.h>

#include <map>

#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"
#include "PageContent.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "globals.hpp"



int main() {
  

    Showcase showcase;

#ifdef USETF
    TestFramework testFramework;
    testFramework.content(showcase.content());
#else
    showcase.content();
#endif

    return 0;
}
