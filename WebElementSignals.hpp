/**
 * @file WebElementSignals.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief The signal classes that have an HTML element (or JS event listener at least) at their
 * core.
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
/**
 * @brief A signal that originates or ends in a web element. This type may also act as a conduit or
 * passthru due to inheriting both emitter and acceptor capabilities.
 *
 * @tparam S
 */
template <typename S>
class WebElementSignal : public SignalAcceptor<S>, public SignalEmitter<S> {
    shared_ptr<WebElement> wptr_;  //!< The actual WebElement this acts as a signal wrapper for.
    std::string boundField_;  //!< The field in the domElement that stores whatever signal data we
                              //!< are interested in.
    std::string eventListenerName_ = "";

    val eventListenerFn_ = val::null();

   public:
    virtual void emit(const S& s) { SignalEmitter<S>::emit(s); }

    WebElementSignal(const WebElement& wptr, const std::string& boundField,
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

    virtual ~WebElementSignal() {}
};

/**
 * @brief A signal originating in an event listener.
 *
 * @tparam S data type of signal, e.g. string, int, double, etc...
 */
template <typename S>
class EventListenerEmitter : public SignalEmitter<S> {
    // shared_ptr<val> eventListener_ = nullptr;
   protected:
    val domElement_, eventListenerFn_;
    std::string eventListenerName_;
    std::string eventListenerGeneratorName_;


   public:
    // shared_ptr<Signal<S>> getOutput() const { return output_; }
    EventListenerEmitter(val domElement, const std::string& eventListenerName,
                         const std::string& eventListenerGeneratorName,
                         bool emitInitialValue = false)
        : domElement_(domElement),
          eventListenerName_(eventListenerName),
          eventListenerGeneratorName_(eventListenerGeneratorName) {
        SignalEmitter(emitInitialValue);
    }

    virtual void update() {
        val elgEmitFn = val::global(this->eventListenerGeneratorName_.c_str());
        eventListenerFn_ = elgEmitFn(*this);
        domElement_.call<void>("removeEventListener", val(eventListenerName_), eventListenerFn_);
        domElement_.call<void>("addEventListener", val(eventListenerName_), eventListenerFn_);
    }

    virtual void emit(const S& s) {
        cout << "EventListenerEmitter::emit called!" << endl;
        SignalEmitter<S>::emit(s);
    }

    virtual ~EventListenerEmitter() {
        // cout << "Destroying EventListenerEmitter\n";
    }
};

/**
 * @brief A signal originating in an event listener.
 *
 * @tparam S data type of signal, e.g. string, int, double, etc...
 */
template <typename S>
class SelectEmitter : public EventListenerEmitter<S> {
    // shared_ptr<val> eventListener_ = nullptr;

   public:
    // shared_ptr<Signal<S>> getOutput() const { return output_; }
    SelectEmitter(val domElement)
        : EventListenerEmitter<S>(domElement, "change", "elgSelectEmitFn") {
        // this->domElement_ = domElement;
        // this->eventListenerName_ = "change";
        // this->eventListenerGeneratorName_ = "elgSelectEmitFn";
    }

    virtual void update() { EventListenerEmitter<S>::update(); }

    virtual void emit(const S& s) {
        cout << "SelectEmitter::emit called!" << endl;
        EventListenerEmitter<S>::emit(s);
    }

    virtual ~SelectEmitter() {
        // cout << "Destroying SignalObject\n";
    }
};

/**
 * @brief This class is a bit special because it's essentially fixed in its template parameter. It
 * will emit a pair of doubles that represents the mouse position during a mouse event. The specific
 * event can be different. It may be a click, mouseover, mousedown, etc... but the tparam is going
 * to reflect the location where this happened. This is also why we can include the
 * EMSCRIPTEN_BINDINGS block. The specifics of the bindings are not contingent on any particular use
 * of the class.
 *
 * @tparam S
 */
template <typename S>
class MouseSignal : public SignalEmitter<S> {
    shared_ptr<WebElement> wptr_;  //!< The actual WebElement this acts as a signal wrapper for.
    std::string eventListenerName_ = "";  //!< i.e. 'click', 'mousedown', 'mouseover', etc...
    val eventListenerFn_ = val::null();
    // bool coordsUseViewBox_ = false;

   public:
    virtual void emit(const S& s) { SignalEmitter<S>::emit(s); }

    MouseSignal(const WebElement& wptr, const std::string& eventListenerName,
                bool coordsUseViewBox = false, bool emitInitialValue = true)
        : eventListenerName_(eventListenerName) {
        this->emitInitialValue_ = emitInitialValue;
        wptr_ = make_shared<WebElement>(wptr);
    }

    /**
     * @brief I created this so I can call it from JS to make the needed type for the emit method.
     * There's probably a better way to do this but I just needed a quick way to make sure that I
     * was sending out a C++ pair of doubles.
     *
     * @param x
     * @param y
     * @return std::pair<double, double>
     */
    static std::pair<double, double> packageCoordPair(double x, double y) {
        return std::pair(x, y);
    }

    /**
     * @brief Everything here seems to concern setting up the output so if there isn't one we just
     * return.
     *
     */
    virtual void update() {
        if (this->output_ == nullptr) return;
        val elgEmitFn = val::global("elgMouseSignal");
        wptr_->domElement_.call<void>("removeEventListener", val(eventListenerName_),
                                      eventListenerFn_);
        eventListenerFn_ = elgEmitFn(*this, wptr_->domElement_);
        wptr_->domElement_.call<void>("addEventListener", val(eventListenerName_),
                                      eventListenerFn_);
        // if (!this->emitInitialValue_) return;
        // cout << "Getting initial value for id = " << wptr_->getId().as<std::string>() << endl;
        // // const S initialVal = wptr_->domElement_.call<val>("getAttribute",
        // // val(boundField_)).as<S>();
        // const S initialVal = wptr_->domElement_[boundField_].as<S>();
        // cout << "Initial value for id = " << wptr_->getId().as<std::string>() << " is '"
        //      << initialVal << "'" << endl;
        // this->emit(initialVal);
    }

    virtual ~MouseSignal() {}
};

EMSCRIPTEN_BINDINGS(MouseSignal) {
    emscripten::class_<std::pair<double, double>>("pair");

    emscripten::class_<MouseSignal<std::pair<double, double>>>("MouseSignal")
        .function("emit", &MouseSignal<std::pair<double, double>>::emit,
                  emscripten::allow_raw_pointers())
        .class_function("packageCoordPair",
                        &MouseSignal<std::pair<double, double>>::packageCoordPair,
                        emscripten::allow_raw_pointers());
}

}  // namespace cl2

#endif