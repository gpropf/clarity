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

class WebElement {
    std::string tag_, name_;
    int id_;
    val webElement_;

   public:
    WebElement(const std::string& tag, const std::string& name, int id)
        : tag_(tag), name_(name), id_(id) {
        std::cout << "WebElement CALLED!" << std::endl;
        val webElement_ = val::global("WebElement").new_();
        webElement_.call<void>("initElement", tag_, name_, id_);
    }

    void printStats() const {
        cout << "Tag is: " << tag_ << endl;
        cout << "element name is: " << name_ << endl;
        cout << "id is: " << id_ << endl;
    }
};

struct WebElementSignalObject : public SignalObject {

    

    WebElementSignalObject(WebElement& w) {
        //obj_ = std::static_pointer_cast<void>(make_shared<WebElement>(w));
    }

    virtual void accept(shared_ptr<void> s) {
        // shared_ptr<val> fn = std::static_pointer_cast<val> (this->obj_);
        // fn(s);
    }
};

struct ConsoleLoggerSignalObject : public SignalObject {

    shared_ptr<val> fnptr_;

    ConsoleLoggerSignalObject(val& f) {
        //obj_ = std::static_pointer_cast<void>(make_shared<ConsoleLoggerSignalObject>(f));
        fnptr_ = make_shared<val>(f);
    }

    virtual void accept(shared_ptr<void> s) {
        
        val fn = *fnptr_;
        fn(s);
    }
};

}  // namespace cl2

#endif