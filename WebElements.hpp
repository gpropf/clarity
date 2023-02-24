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
    val domElement_;

    val getId() const { return domElement_["id"]; }

    val getDomElement() const { return domElement_; }

    std::string getName() const {
        return domElement_.call<val>("getAttribute", val("name")).as<std::string>();
    }

    WebElement(const std::string& tag, const std::string& name, const std::string& id = "",
               val parentElement = val::null()) {
        val initElement = val::global("WebElement")["initElement"];

        domElement_ = initElement(tag, name, id);

        if (parentElement != val::null()) {
            parentElement.call<void>("appendChild", domElement_);
        }
    }

    /**
     * @brief This constructor is specifically designed to make use of a pre-existing element such
     * as from a static HTML page that someone has already laid out.
     *
     * @param domElement
     */
    WebElement(val domElement) { domElement_ = domElement; }

    WebElement(const std::string& id) {
        val document = val::global("document");
        domElement_ = document.call<val>("getElementById", val(id));
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
    BR(const std::string& name = "", const std::string& id = "", val parentElement = val::null())
        : WebElement("br", name, id, parentElement) {}

    BR(val parentElement, const std::string& name = "", const std::string& id = "")
        : WebElement("br", name, id, parentElement) {}
};

struct SVG : public WebElement {
    SVG(const std::string& name, int width, int height, const std::string& id = "",
        val parentElement = val::null())
        : WebElement("svg", name, id, parentElement) {
        domElement_.call<void>("setAttribute", val("width"), val(width));
        domElement_.call<void>("setAttribute", val("height"), val(height));
    }
};

struct Label : public WebElement {
    Label(const std::string& text, const WebElement& wel, bool swallowForElement = true,
          const std::string& id = "", val parentElement = val::null())
        : WebElement("label", "lbl_" + wel.getName(), id, parentElement) {
        setAttribute("for", wel.getId());
        domElement_.set("innerHTML", val(text));
        domElement_.call<void>("appendChild", wel.domElement_);
    }
};

struct Button : public WebElement {
    Button(const std::string& name, const std::string& displayedText, val onClickFn,
           const std::string& id = "", val parentElement = val::null())
        : WebElement("button", name, id, parentElement) {
        this->domElement_.call<void>("addEventListener", val("click"), onClickFn);
        this->domElement_.set("textContent", val(displayedText));
    }
};

/**
 * @brief Specializing the `WebElement` for input fields. Also a struct because this is intended to
 * be instantiated const and used as immutable.
 *
 */
struct InputElement : public WebElement {
   public:
    InputElement(const std::string& tag, const std::string& name, const std::string& type,
                 const std::string& id = "", val parentElement = val::null())
        : WebElement(tag, name, id, parentElement) {
        domElement_.set("type", val(type));
    }
};

}  // namespace cl2

#endif