//#include "testbed.hpp"

#include <map>

#include "PageContent.hpp"
#include "ClarityNode.hpp"
#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "globals.hpp"
#include "Unittest.hpp"


int main() {
    Unittest unittest;
    TestFramework testFramework;
    testFramework.content(unittest.content());
    return 0;
}
