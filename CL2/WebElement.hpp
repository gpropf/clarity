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

    virtual void emitOne(SignalObject<S>* sobj, S& s) {
        cout << "WebElementSignalObject::emitOne() CALLED!" << endl;
        sobj->accept(s);
        cout << "WebElementSignalObject::emitOne() CALLED! END" << endl;
    }

    WebElementSignalObject(WebElement& w) {
        wptr_ = make_shared<WebElement>(w);
        val testListenerFn = val::global("testListenerFn");
        wptr_->domElement_.call<void>("addEventListener", val("change"), testListenerFn);
    }

    virtual void accept(S& s) {
        // shared_ptr<val> fn = std::static_pointer_cast<val> (this->obj_);
        // fn(s);
    }
};

template <typename S>
struct ConsoleLoggerSignalObject : public SignalObject<S> {
    shared_ptr<val> fnptr_;

    virtual void emitOne(SignalObject<S>* sobj, S& s) {}

    ConsoleLoggerSignalObject(val& f) {
        // obj_ = std::static_pointer_cast<void>(make_shared<ConsoleLoggerSignalObject>(f));
        fnptr_ = make_shared<val>(f);
    }

    virtual void accept(S& s) {
        cout << "ConsoleLoggerSignalObject::accept() CALLED!" << endl;
        val fn = *fnptr_;
        fn(s);
    }
};

}  // namespace cl2

#endif