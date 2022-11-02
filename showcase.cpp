//#include "testbed.hpp"

#include <map>

#include "PageContent.hpp"
#include "ClarityNode.hpp"
#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "globals.hpp"
#include "Showcase.hpp"


int main() {
    Showcase showcase;
    TestFramework testFramework;
    testFramework.content(showcase.content());
    return 0;
}
