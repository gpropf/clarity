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
    shared_ptr<WebElement> wptr_;
    std::string boundField_;
    bool emitInitialValue_ = true;

   public:
    // virtual void emitOne(SignalObject<S>* sobj, const S& s) {
    //     // cout << "WebElementSignalObject::emitOne() CALLED!" << endl;
    //     sobj->accept(s);
    //     // cout << "WebElementSignalObject::emitOne() CALLED! END" << endl;
    // }

    virtual void emit(const S& s) { SignalObject<S>::emit(s); }

    WebElementSignalObject(const WebElement& wptr, const std::string& boundField,
                           bool emitInitialValue = true)
        : boundField_(boundField),
          emitInitialValue_(emitInitialValue),
          wptr_(make_shared<WebElement>(wptr)) {}

    // WebElementSignalObject(const val domElement, const std::string& boundField, bool
    // emitInitialValue_ = true) {
    //     // wptr_ = make_shared<WebElement>(wptr);
    //     // boundField_ = boundField;
    // }

    /**
     * @brief Everything here seems to concern setting up the output so if there isn't one we just
     * return.
     *
     */
    virtual void finalize() {
        if (this->getOutput() == nullptr) return;
        val elgEmitFn = val::global("elgEmitFn");
        val listenerFn = elgEmitFn(*this);
        wptr_->domElement_.call<void>("addEventListener", val("change"), listenerFn);

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
 * @brief A `SignalObject` wrapper for a JS function object.
 *
 * @tparam S
 */
template <typename S>
class JSFunctionSignalObject : public SignalObject<S> {
    shared_ptr<val> fnptr_;

   public:
    virtual void emitOne(SignalObject<S>* sobj, const S& s) {}

    JSFunctionSignalObject(const val& f) { fnptr_ = make_shared<val>(f); }

    virtual void accept(const S& s) {
        // cout << "JSFunctionSignalObject::accept() CALLED!" << endl;
        val fn = *fnptr_;
        fn(s);
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