/**
 * @file WebElementSignals.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief The signal classes that have an HTML element (or JS event listener at least) at their core.
 * @version 0.1
 * @date 2023-03-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef WebElement_hpp
#define WebElement_hpp

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <stdio.h>
#include <unistd.h>

#include <functional>
#include <iostream>
#include <map>
#include <sstream>

#include "SignalPrimitives.hpp"
#include "WebElements.hpp"

using emscripten::val;
using std::cout;
using std::endl;

namespace cl2 {

template <typename S>
class WebElementSignalObjectSS : public SignalAcceptor<S>, public SignalEmitter<S> {
    shared_ptr<WebElement> wptr_;  //!< The actual WebElement this acts as a signal wrapper for.
    std::string boundField_;  //!< The field in the domElement that stores whatever signal data we
                              //!< are interested in.
    std::string eventListenerName_ = "";

    val eventListenerFn_ = val::null();

   public:
    virtual void emit(const S& s) { SignalEmitter<S>::emit(s); }

    WebElementSignalObjectSS(const WebElement& wptr, const std::string& boundField,
                             bool emitInitialValue = true) {
        boundField_ = boundField;
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
        if (wptr_->domElement_["type"] == val("range")) {
            eventListenerName_ = "input";
        } else {
            eventListenerName_ = "change";
        }
        val elgEmitFn = val::global("elgEmitFn");
        wptr_->domElement_.call<void>("removeEventListener", val(eventListenerName_),
                                      eventListenerFn_);
        eventListenerFn_ = elgEmitFn(*this);
        wptr_->domElement_.call<void>("addEventListener", val(eventListenerName_),
                                      eventListenerFn_);
        if (!this->emitInitialValue_) return;
        cout << "Getting initial value for id = " << wptr_->getId().as<std::string>() << endl;
        // const S initialVal = wptr_->domElement_.call<val>("getAttribute",
        // val(boundField_)).as<S>();
        const S initialVal = wptr_->domElement_[boundField_].as<S>();
        // cout << "Initial value for id = " << wptr_->getId().as<std::string>() << " is '"
        //      << initialVal << "'" << endl;
        this->emit(initialVal);
    }

    /**
     * @brief It seems to be necessary to call `setAttribute()` for some things while using the
     * Emscripten `set()` method is needed for others, notably setting the 'value' property of input
     * fields. I'm not sure if 'value' is the only such exception and calling both methods doesn't
     * seem to hurt anything but this is clearly a kludge.
     *
     * Setting the bound field does not seem to trigger the event listener that in turn calls the
     * emit() method, thus we force it here so that a signal is passed on through to the next
     * link in the chain when a signal comes in.
     *
     * @param s
     */
    virtual bool accept(const S& s) {
        SignalAcceptor<S>::accept(s);
        // bool storedSignalAccepts = StoredSignal<S>::accept(s);
        // if (!storedSignalAccepts) return false;
        wptr_->domElement_.set(boundField_, val(s));
        wptr_->domElement_.call<void>("setAttribute", val(boundField_), val(s));

        this->emit(s);
        return true;
    }

    virtual ~WebElementSignalObjectSS() {}
};

/**
 * @brief A signal originating in an event listener.
 *
 * @tparam S data type of signal, e.g. string, int, double, etc...
 */
template <typename S>
class EventListenerEmitter : public SignalEmitter<S> {
    // shared_ptr<val> eventListener_ = nullptr;
    val domElement_;
    std::string eventListenerName_;

   public:
    // shared_ptr<Signal<S>> getOutput() const { return output_; }
    EventListenerEmitter(val domElement, const std::string& eventListenerName)
        : domElement_(domElement), eventListenerName_(eventListenerName) {}

    virtual void update() {
        val elgEmitFn = val::global("elgEmitFn");
        val eventListenerFn = elgEmitFn(*this);
        domElement_.call<void>("removeEventListener", val(eventListenerName_), eventListenerFn);
        domElement_.call<void>("addEventListener", val(eventListenerName_), eventListenerFn);
    }

    virtual void emit(const S& s) {
        cout << "EventListenerEmitter::emit called!" << endl;
        SignalEmitter<S>::emit(s);
    }

    virtual ~EventListenerEmitter() {
        // cout << "Destroying SignalObject\n";
    }
};



}  // namespace cl2

#endif