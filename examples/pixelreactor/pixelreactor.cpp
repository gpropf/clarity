#include <map>

#include "clarity.hpp"
#include "PageContent.hpp"
#include "Pixelreactor.hpp"
#include "ClarityNode.hpp"
#include "CLNodeFactory.hpp"

#include "embindings.hpp"
#include "globals.hpp"



int main() {
    PixelReactor pixelreactor;
    #ifdef USETF
        TestFramework testFramework;
        testFramework.content(pixelreactor.content());
    #else
        pixelreactor.content();
    #endif
    return 0;
}
