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

template <typename S>
struct WebElementSignalObject : public SignalObject<WebElement, S> {
    WebElementSignalObject(WebElement& w) : SignalObject<WebElement, S>(w) {}
};

template <typename S>
struct ConsoleLoggerSignalObject : public SignalObject<val, S> {
    ConsoleLoggerSignalObject(val& f) : SignalObject<val, S>(f) {}

    virtual void accept(S & s) {
        val fn = *this->obj_;
        fn(s);
    }
};

}  // namespace cl2

#endif