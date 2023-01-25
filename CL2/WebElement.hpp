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

#include "Signal.hpp"

using emscripten::val;
using std::cout;
using std::endl;

namespace cl2 {

struct WebElement {
    std::string tag_, name_;
    int id_;
    val webElement_, domElement_;

   public:
    WebElement(const std::string& tag, const std::string& name, int id)
        : tag_(tag), name_(name), id_(id) {
        std::cout << "WebElement CALLED!" << std::endl;
        val webElement_ = val::global("WebElement").new_();
        domElement_ = webElement_.call<val>("initElement", tag_, name_, id_);
    }

    void printStats() const {
        cout << "Tag is: " << tag_ << endl;
        cout << "element name is: " << name_ << endl;
        cout << "id is: " << id_ << endl;
    }
};

template <typename S>
struct WebElementSignalObject : public SignalObject<S> {
    shared_ptr<WebElement> wptr_;

    virtual void emitOne(SignalObject<S>* sobj, const S& s) {
        cout << "WebElementSignalObject::emitOne() CALLED!" << endl;
        sobj->accept(s);
        cout << "WebElementSignalObject::emitOne() CALLED! END" << endl;
    }

    virtual void emit(const S& s) { SignalObject<S>::emit(s); }

    WebElementSignalObject(WebElement& w) { wptr_ = make_shared<WebElement>(w); }

    virtual void finalize() {
        val elgEmitFn = val::global("elgEmitFn");
        val testListenerFn = elgEmitFn(*this);
        // val testListenerFn = val::global("testListenerFn");

        wptr_->domElement_.call<void>("addEventListener", val("change"), testListenerFn);
    }

    virtual void accept(const S& s) {
        wptr_->domElement_.set("value", val(s));
        // shared_ptr<val> fn = std::static_pointer_cast<val> (this->obj_);
        // fn(s);
    }
    
    virtual ~WebElementSignalObject() { cout << "Destructing WebElementSignalObject\n"; }
};

template <typename S>
struct ConsoleLoggerSignalObject : public SignalObject<S> {
    shared_ptr<val> fnptr_;

    virtual void emitOne(SignalObject<S>* sobj, const S& s) {}

    ConsoleLoggerSignalObject(val& f) {
        // obj_ = std::static_pointer_cast<void>(make_shared<ConsoleLoggerSignalObject>(f));
        fnptr_ = make_shared<val>(f);
    }

    virtual void accept(const S& s) {
        cout << "ConsoleLoggerSignalObject::accept() CALLED!" << endl;
        val fn = *fnptr_;
        fn(s);
    }

    virtual ~ConsoleLoggerSignalObject() { cout << "Destructing ConsoleLoggerSignalObject\n"; }
};

EMSCRIPTEN_BINDINGS(WebElementSignalObject) {
    emscripten::class_<WebElementSignalObject<std::string>>("WebElementSignalObject")
        .function("emit", &WebElementSignalObject<std::string>::emit,
                  emscripten::allow_raw_pointers())
        .function("accept", &WebElementSignalObject<std::string>::accept,
                  emscripten::allow_raw_pointers());

    emscripten::class_<ConsoleLoggerSignalObject<std::string>>("ConsoleLoggerSignalObject")
        .function("emit", &ConsoleLoggerSignalObject<std::string>::emit,
                  emscripten::allow_raw_pointers())
        .function("accept", &ConsoleLoggerSignalObject<std::string>::accept,
                  emscripten::allow_raw_pointers());

    emscripten::class_<SignalObject<std::string>>("SignalObject")
        .function("emit", &SignalObject<std::string>::emit, emscripten::allow_raw_pointers())
        .function("accept", &SignalObject<std::string>::accept, emscripten::allow_raw_pointers());
}

}  // namespace cl2

#endif