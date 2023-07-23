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
    cl2::SignalBuilder sb = cl2::SignalBuilder();
    //auto gridControl = make_shared<GridControl<int>>(15, 10, 600, 400, sb, "gc1");
    // svgMouseClickAcceptor_->setObjectPointer(this->shared_from_this());

    // gridControl->addColorToPallete(0, "#000000");
    // gridControl->addColorToPallete(1, "#ff0000");
    // gridControl->addColorToPallete(2, "#00ff00");
    // gridControl->addColorToPallete(3, "#0000ff");
    // gridControl->finalize();
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
