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

   public:
    virtual void emit(const S& s) { SignalEmitter<S>::emit(s); }

    MouseSignal(const WebElement& wptr, const std::string& eventListenerName,
                bool emitInitialValue = true)
        : eventListenerName_(eventListenerName) {
        this->emitInitialValue_ = emitInitialValue;
        wptr_ = make_shared<WebElement>(wptr);
    }

    /**
     * @brief Everything here seems to concern setting up the output so if there isn't one we just
     * return.
     *
     */
    virtual void update() {
        if (this->output_ == nullptr) return;
        
        val elgEmitFn = val::global("elgEmitFn");
        wptr_->domElement_.call<void>("removeEventListener", val(eventListenerName_),
                                      eventListenerFn_);
        eventListenerFn_ = elgEmitFn(*this);
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

}  // namespace cl2
#endif