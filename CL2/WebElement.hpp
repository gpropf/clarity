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

/**
 * @brief Represents a single web element. Defined as a struct because this is meant to defined
 * using const.
 *
 */
struct WebElement {
    std::string tag_, name_, boundField_;
    int id_;
    val webElement_;
    val domElement_;

    // public:
    WebElement(const std::string& tag, const std::string& name, const std::string& boundField,
               int id, val parentElement = val::null())
        : tag_(tag), name_(name), boundField_(boundField), id_(id) {
        val webElement_ = val::global("WebElement").new_();
        domElement_ = webElement_.call<val>("initElement", tag_, name_, id_);
        if (parentElement != val::null()) {
            parentElement.call<void>("appendChild", domElement_);
        }
    }
};

/**
 * @brief Simple br tag, probably doesn't need a name or id. The logic in the JS code is that
 * negative ids are not created so you can set the id to a negative to create a tag with no id.
 *
 */
struct BR : public WebElement {
    BR(const std::string& name = "", int id = -1, val parentElement = val::null())
        : WebElement("br", name, "", id, parentElement) {}
};

struct SVG : public WebElement {
    SVG(const std::string& name, int width, int height, int id, val parentElement = val::null())
        : WebElement("svg", name, "", id, parentElement) {
        // domElement_.set("width", val(width));
        // domElement_.set("height", val(height));
        domElement_.call<void>("setAttribute", val("width"), val(width));
        domElement_.call<void>("setAttribute", val("height"), val(height));
    }
};

/**
 * @brief Specializing the `WebElement` for input fields. Also a struct because this is intended to
 * be instantiated const and used as immutable.
 *
 */
struct InputElement : public WebElement {
   public:
    InputElement(const std::string& tag, const std::string& name, const std::string& type, int id, val parentElement = val::null())
        : WebElement(tag, name, "value", id, parentElement) {
        domElement_.set("type", val(type));
    }
};

/**
 * @brief A `SignalObject` wrapper for a `WebElement`.
 *
 * @tparam S
 */
template <typename S>
class WebElementSignalObject : public SignalObject<S> {
    shared_ptr<WebElement> wptr_;

   public:
    virtual void emitOne(SignalObject<S>* sobj, const S& s) {
        // cout << "WebElementSignalObject::emitOne() CALLED!" << endl;
        sobj->accept(s);
        // cout << "WebElementSignalObject::emitOne() CALLED! END" << endl;
    }

    virtual void emit(const S& s) { SignalObject<S>::emit(s); }

    WebElementSignalObject(const WebElement& w) { wptr_ = make_shared<WebElement>(w); }

    virtual void finalize() {
        val elgEmitFn = val::global("elgEmitFn");
        val testListenerFn = elgEmitFn(*this);
        wptr_->domElement_.call<void>("addEventListener", val("change"), testListenerFn);
    }

    virtual void accept(const S& s) { wptr_->domElement_.set(wptr_->boundField_, val(s)); }

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