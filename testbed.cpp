//#include "testbed.hpp"

#include <map>

#include "PageContent.hpp"
#include "ClarityNode.hpp"
#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "globals.hpp"
#include "Showcase.hpp"

template <>
const array<string, 8> CanvasGrid<unsigned char>::colors = {"#F5F5DC", "#00FF00", "#00AA00", "#FF00FF",
                                                            "#AA00AA", "#00AAFF", "#9090AA", "#888888"};

int main() {
    Showcase showcase;
    TestFramework testFramework;
    testFramework.content(showcase.content());
    return 0;
}
