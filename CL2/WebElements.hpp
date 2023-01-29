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
    //std::string tag_;  //, name_;
                       // int id_;
    //val webElement_;
    val domElement_;

    // public:

    val getId() const { return domElement_["id"]; }

    std::string getName() const {
        return domElement_.call<val>("getAttribute", val("name")).as<std::string>();
    }

    WebElement(const std::string& tag, const std::string& name, int id,
               val parentElement = val::null())
         {
        //val webElement_ = val::global("WebElement");
        val initElement = val::global("WebElement")["initElement"];
        //domElement_ = webElement_.call<val>("initElement", tag, name, id);
        domElement_ = initElement(tag, name, id);
        if (parentElement != val::null()) {
            parentElement.call<void>("appendChild", domElement_);
        }
    }

    WebElement(val domElement) {
        std::string tag_ = domElement["tagName"].as<std::string>();
        std::transform(tag_.begin(), tag_.end(), tag_.begin(), ::tolower);
        // name_ = domElement.call<val>("getAttribute", val("name")).as<std::string>();
        // cout << "valname: " << tag_ << endl;
        // id_ = stoi(domElement["id"].as<std::string>());
        domElement_ = domElement;
    }

    void setAttribute(const std::string& attr, const val& value) const {
        domElement_.call<void>("setAttribute", attr, value);
    }

    void setAttributes(const std::map<std::string, val>& attrs) const {
        for (auto [attrName, value] : attrs) {
            setAttribute(attrName, value);
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
        : WebElement("br", name, id, parentElement) {}
};

struct SVG : public WebElement {
    SVG(const std::string& name, int width, int height, int id, val parentElement = val::null())
        : WebElement("svg", name, id, parentElement) {
        // domElement_.set("width", val(width));
        // domElement_.set("height", val(height));
        domElement_.call<void>("setAttribute", val("width"), val(width));
        domElement_.call<void>("setAttribute", val("height"), val(height));
    }
};

struct Label : public WebElement {
    Label(const std::string& text, int id, const WebElement& wel, bool swallowForElement = true,
          val parentElement = val::null())
        : WebElement("label", "lbl_" + wel.getName(), id, parentElement) {
        setAttribute("for", wel.getId());
        // setAttribute("innerHTML", val(text));
        domElement_.set("innerHTML", val(text));
        domElement_.call<void>("appendChild", wel.domElement_);
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
        : WebElement(tag, name, id, parentElement) {
        domElement_.set("type", val(type));
    }
};

}  // namespace cl2

#endif