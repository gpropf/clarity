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


EMSCRIPTEN_BINDINGS(CppObjectSignalObject) {
    // emscripten::class_<cl2::CppObjectSignalObject<std::string, TestObj>>("CppObjectSignalObject")
    //     .function("emit", &cl2::CppObjectSignalObject<std::string, TestObj>::emit,
    //               emscripten::allow_raw_pointers())
    //     .function("getSignal", &cl2::CppObjectSignalObject<std::string, TestObj>::getSignal,
    //               emscripten::allow_raw_pointers())
    //     .function("accept", &cl2::CppObjectSignalObject<std::string, TestObj>::accept,
    //               emscripten::allow_raw_pointers());

    emscripten::class_<cl2::Merge<std::string, std::string, std::string>>("Merge").function(
        "recompute", &cl2::Merge<std::string, std::string, std::string>::recompute);

    emscripten::class_<cl2::EventListenerEmitter<std::string>>("EventListenerEmitter")
        .function("emit", &cl2::EventListenerEmitter<std::string>::emit,
                  emscripten::allow_raw_pointers());

    // emscripten::class_<cl2::ObjectEmitter<std::string, TestObj>>("ObjectEmitter")
    //     .function("emit", &cl2::ObjectEmitter<std::string, TestObj>::emit,
    //               emscripten::allow_raw_pointers());
}

EMSCRIPTEN_BINDINGS(WebElementSignalObject) {
    // emscripten::class_<WebElementSignalObject<std::string>>("WebElementSignalObject")
    //     .function("emit", &WebElementSignalObject<std::string>::emit,
    //               emscripten::allow_raw_pointers())
    //     .function("accept", &WebElementSignalObject<std::string>::accept,
    //               emscripten::allow_raw_pointers());

    emscripten::class_<WebElementSignal<std::string>>("WebElementSignal")
        .function("emit", &WebElementSignal<std::string>::emit, emscripten::allow_raw_pointers())
        .function("accept", &WebElementSignal<std::string>::accept,
                  emscripten::allow_raw_pointers());
}
