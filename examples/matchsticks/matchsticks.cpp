#include <map>

#include "clarity.hpp"
#include "PageContent.hpp"
#include "Matchsticks.hpp"
#include "ClarityNode.hpp"
#include "CLNodeFactory.hpp"

#include "embindings.hpp"
#include "globals.hpp"

int main() {
    Matchsticks matchsticks;
    #ifdef USETF
        TestFramework testFramework;
        testFramework.content(matchsticks.content());
    #else
        matchsticks.content();
    #endif
    return 0;
}
