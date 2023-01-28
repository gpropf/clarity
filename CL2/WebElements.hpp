#ifndef WebElements_hpp
#define WebElements_hpp

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
    InputElement(const std::string& tag, const std::string& name, const std::string& type, int id,
                 val parentElement = val::null())
        : WebElement(tag, name, "value", id, parentElement) {
        domElement_.set("type", val(type));
    }
};

}  // namespace cl2

#endif