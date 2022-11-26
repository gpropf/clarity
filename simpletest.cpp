//#include "testbed.hpp"

#include <map>

#include "PageContent.hpp"
#include "ClarityNode.hpp"
#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "globals.hpp"
#include "SimpleTest.hpp"


int main() {
    SimpleTest simpleTest;
    TestFramework testFramework;
    testFramework.content(simpleTest.content());
    return 0;
}
