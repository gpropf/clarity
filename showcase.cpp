//#include "testbed.hpp"

#include "Showcase.hpp"

#include <map>

#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"
#include "PageContent.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "globals.hpp"

//#define USETF

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
