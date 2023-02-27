#ifndef Signals_hpp
#define Signals_hpp

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "SignalPrimitives.hpp"

using std::cout;
using std::endl;
using std::make_shared;
using std::shared_ptr;

namespace cl2 {

template <typename S>
class MouseSignal : public SignalEmitter<S> {
    shared_ptr<WebElement> wptr_;  //!< The actual WebElement this acts as a signal wrapper for.

    std::string eventListenerName_ = "";

    val eventListenerFn_ = val::null();
    bool coordsUseViewBox_ = false;

   public:
    virtual void emit(const S& s) { SignalEmitter<S>::emit(s); }

    MouseSignal(const WebElement& wptr, const std::string& eventListenerName,
                bool coordsUseViewBox = false, bool emitInitialValue = true)
        : eventListenerName_(eventListenerName), coordsUseViewBox_(coordsUseViewBox) {
        this->emitInitialValue_ = emitInitialValue;
        wptr_ = make_shared<WebElement>(wptr);
    }

    static std::pair<int, int> packagePairInt(int x, int y) { return std::pair(x, y); }

    static std::pair<double, double> packageCoordPair(double x, double y) { return std::pair(x, y); }

    /**
     * @brief Everything here seems to concern setting up the output so if there isn't one we just
     * return.
     *
     */
    virtual void update() {
        if (this->output_ == nullptr) return;
        val elgEmitFn;
        if (coordsUseViewBox_) {
            elgEmitFn = val::global("elgMouseSignalDouble");
        } else {
            elgEmitFn = val::global("elgMouseSignal");
        }

        wptr_->domElement_.call<void>("removeEventListener", val(eventListenerName_),
                                      eventListenerFn_);
        eventListenerFn_ = elgEmitFn(*this, wptr_->domElement_);
        wptr_->domElement_.call<void>("addEventListener", val(eventListenerName_),
                                      eventListenerFn_);
        if (!this->emitInitialValue_) return;
        // cout << "Getting initial value for id = " << wptr_->getId().as<std::string>() << endl;
        // // const S initialVal = wptr_->domElement_.call<val>("getAttribute",
        // // val(boundField_)).as<S>();
        // const S initialVal = wptr_->domElement_[boundField_].as<S>();
        // cout << "Initial value for id = " << wptr_->getId().as<std::string>() << " is '"
        //      << initialVal << "'" << endl;
        // this->emit(initialVal);
    }
};

EMSCRIPTEN_BINDINGS(Signals) {
    emscripten::class_<std::pair<double, double>>("pair");

    emscripten::class_<MouseSignal<std::pair<double, double>>>("MouseSignal")
        .function("emit", &MouseSignal<std::pair<double, double>>::emit, emscripten::allow_raw_pointers())
        .class_function("packageCoordPair", &MouseSignal<std::pair<double, double>>::packageCoordPair,
                        emscripten::allow_raw_pointers());
}

}  // namespace cl2
#endif