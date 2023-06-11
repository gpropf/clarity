#include <map>


#include "GridControl.hpp"
#include "Signal.hpp"
#include "SignalBuilder.hpp"
#include "SignalPrimitives.hpp"
#include "Util.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

#include "Pixelreactor.hpp"

using namespace cl2;

int main() {
    PixelReactor pixelreactor;
    cout << "Hello from Pixelreactor! We should probably do some initialization stuff here." << endl;
    #ifdef USETF
        TestFramework testFramework;
        testFramework.content(pixelreactor.content());
    #else
        //pixelreactor.content();
    #endif
    return 0;
}
