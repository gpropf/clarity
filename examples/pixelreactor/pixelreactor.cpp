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
//static Beaker<unsigned char>* singleton__ = nullptr;

int main() {
    PixelReactor pixelreactor;
    cout << "Pixelreactor stub program started..." << endl;    
    return 0;
}


EMSCRIPTEN_BINDINGS(CppObjectSignalObject) {
    emscripten::class_<cl2::Merge<std::string, std::string, std::string>>("Merge").function(
        "recompute", &cl2::Merge<std::string, std::string, std::string>::recompute);
    emscripten::class_<cl2::EventListenerEmitter<std::string>>("EventListenerEmitter")
        .function("emit", &cl2::EventListenerEmitter<std::string>::emit,
                  emscripten::allow_raw_pointers());    
}

EMSCRIPTEN_BINDINGS(WebElementSignalObject) {    
    emscripten::class_<WebElementSignal<std::string>>("WebElementSignal")
        .function("emit", &WebElementSignal<std::string>::emit, emscripten::allow_raw_pointers())
        .function("accept", &WebElementSignal<std::string>::accept,
                  emscripten::allow_raw_pointers());
}
