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
 * @brief A `SignalObject` wrapper for a `WebElement`.
 *
 * @tparam S
 */
template <typename S>
class WebElementSignalObject : public SignalObject<S> {
    shared_ptr<WebElement> wptr_;  //!< The actual WebElement this acts as a signal wrapper for.
    std::string boundField_;  //!< The field in the domElement that stores whatever signal data we
                              //!< are interested in.
    bool emitInitialValue_ = true;  //!< In some cases we don't want to emit the initial value but
                                    //!< the default is to do so.

    val changeListenerFn_ = val::null();

   public:
    virtual void emit(const S& s) { SignalObject<S>::emit(s); }

    WebElementSignalObject(const WebElement& wptr, const std::string& boundField,
                           bool emitInitialValue = true)
        : boundField_(boundField),
          emitInitialValue_(emitInitialValue),
          wptr_(make_shared<WebElement>(wptr)) {}

    /**
     * @brief Everything here seems to concern setting up the output so if there isn't one we just
     * return.
     *
     */
    virtual void finalize() {
        if (this->getOutput() == nullptr) return;
        val elgEmitFn = val::global("elgEmitFn");
        wptr_->domElement_.call<void>("removeEventListener", val("change"), changeListenerFn_);
        changeListenerFn_ = elgEmitFn(*this);
        wptr_->domElement_.call<void>("addEventListener", val("change"), changeListenerFn_);

        if (!emitInitialValue_) return;
        cout << "Getting initial value for id = " << wptr_->getId().as<std::string>() << endl;
        // const S initialVal = wptr_->domElement_.call<val>("getAttribute",
        // val(boundField_)).as<S>();
        const S initialVal = wptr_->domElement_[boundField_].as<S>();
        cout << "Initial value for id = " << wptr_->getId().as<std::string>() << " is '"
             << initialVal << "'" << endl;
        emit(initialVal);
    }

    /**
     * @brief It seems to be necessary to call `setAttribute()` for some things while using the
     * Emscripten `set()` method is needed for others, notably setting the 'value' property of input
     * fields. I'm not sure if 'value' is the only such exception and calling both methods doesn't
     * seem to hurt anything but this is clearly a kludge.
     *
     * @param s
     */
    virtual void accept(const S& s) {
        wptr_->domElement_.set(boundField_, val(s));
        wptr_->domElement_.call<void>("setAttribute", val(boundField_), val(s));
    }

    virtual ~WebElementSignalObject() {  // cout << "Destroying WebElementSignalObject\n";
    }
};

/**
 * @brief A `SignalObject` wrapper for a JS function object. Note that this is not exactly the
 * analog of the CppLambda. There is no second output that can have a different type from the normal
 * one.
 *
 * @tparam S
 */
template <typename S>
class JSFunctionSignalObject : public SignalObject<S> {
    shared_ptr<val> jsFn_;

   public:
    JSFunctionSignalObject(const val& jsFn) { jsFn_ = make_shared<val>(jsFn); }

    /**
     * @brief In this class we simply call the stored JS function on the value.
     *
     * @param s
     */
    virtual void accept(const S& s) {
        val fn = *jsFn_;
        // S sOut = fn(s).template as<S>();
        val fnOut = fn(s);
        if (fnOut == val::null()) return;
        S sOut = fnOut.as<S>();
        if (this->getOutput() != nullptr) this->getOutput()->accept(sOut);
    }

    virtual void finalize() {}

    virtual ~JSFunctionSignalObject() {  // cout << "Destroying JSFunctionSignalObject\n";
    }
};

EMSCRIPTEN_BINDINGS(WebElementSignalObject) {
    emscripten::class_<WebElementSignalObject<std::string>>("WebElementSignalObject")
        .function("emit", &WebElementSignalObject<std::string>::emit,
                  emscripten::allow_raw_pointers())
        .function("accept", &WebElementSignalObject<std::string>::accept,
                  emscripten::allow_raw_pointers());

    emscripten::class_<JSFunctionSignalObject<std::string>>("ConsoleLoggerSignalObject")
        .function("emit", &JSFunctionSignalObject<std::string>::emit,
                  emscripten::allow_raw_pointers())
        .function("accept", &JSFunctionSignalObject<std::string>::accept,
                  emscripten::allow_raw_pointers());

    emscripten::class_<SignalObject<std::string>>("SignalObject")
        .function("emit", &SignalObject<std::string>::emit, emscripten::allow_raw_pointers())
        .function("accept", &SignalObject<std::string>::accept, emscripten::allow_raw_pointers());
}

}  // namespace cl2

#endif