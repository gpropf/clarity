/**
 * @file WebElements.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief Basic HTML classes. Essentially, these are a quick way to create HTML tags.
 * @version 0.1
 * @date 2023-03-01
 *
 * @copyright Copyright (c) 2023
 *
 */

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

#include "Util3.hpp"

using emscripten::val;
using std::cout;
using std::endl;

namespace cl3 {

class Channel;
class WebElementChannel;

/**
 * @brief Represents a single web element. Defined as a struct because this is meant to defined
 * usually as const.
 *
 */
struct WebElement : public Identifiable {
    val domElement_;

    string channelEventListenerName_ = "change";

    bool deleteDomElementOnExit_ = false;
    // bool deleteDomElementOnExit_ = true;

    void forceDeleteDomElementOnExit() { deleteDomElementOnExit_ = true; }

    void deleteDomElement() { domElement_.call<void>("remove"); }

    virtual val generateEventListenerForChannel(shared_ptr<Channel> wec) { return val(wec); }

    virtual void setChannelEventListener(val evListener) {
        addEventListener(val(channelEventListenerName_), evListener);
    }

    val getId() const { return domElement_["id"]; }

    virtual int setUid(int id) {
        int oldId = Identifiable::setUid(id);
        domElement_.set("id", id);
        return oldId;
    }

    val getDomElement() const { return domElement_; }

    std::string getName() const {
        return domElement_.call<val>("getAttribute", val("name")).as<std::string>();
    }

    virtual ~WebElement() {
        if (deleteDomElementOnExit_) domElement_.call<void>("remove");
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

    void addEventListener(val ev, val listenerFn) {
        domElement_.call<void>("addEventListener", ev, listenerFn);
    }
};

/**
 * @brief Specializing the `WebElement` for input fields. Also a struct because this is intended to
 * be instantiated const and used as immutable.
 *
 */
struct InputElement : public WebElement {
   public:
    InputElement(const std::string& name, const std::string& type, const std::string& id = "",
                 val parentElement = val::null())
        : WebElement("input", name, id, parentElement) {
        domElement_.set("type", val(type));
        // domElement_.call<void>("addEventListener", val("change"), onChangeFn);
    }
};

struct TextField : public InputElement {
    TextField(const std::string& name, const std::string& id = "", val parentElement = val::null())
        : InputElement(name, "text", id, parentElement) {
        channelEventListenerName_ = "change";
    }

    virtual val generateEventListenerForChannel(shared_ptr<Channel> wec) {
        val generateEventListenerForChannel_TextField =
            val::global("generateEventListenerForChannel_TextField");
        val evListenerFn = generateEventListenerForChannel_TextField(wec);
        return evListenerFn;
    }
};

struct RangeInput : public InputElement {
    RangeInput(const std::string& name, const std::string& id = "", val parentElement = val::null())
        : InputElement(name, "range", id, parentElement) {
        channelEventListenerName_ = "input";
    }

    virtual val generateEventListenerForChannel(shared_ptr<Channel> wec) {
        val generateEventListenerForChannel_RangeInput =
            val::global("generateEventListenerForChannel_RangeInput");
        val evListenerFn = generateEventListenerForChannel_RangeInput(wec);
        return evListenerFn;
    }
};

// OLD STUFF BELOW
// OLD STUFF BELOW
// OLD STUFF BELOW

struct Div : public WebElement {
    Div(const std::string& name = "", const std::string& id = "", val parentElement = val::null())
        : WebElement("div", name, id, parentElement) {}
};

struct Select : public WebElement {
    Select(const std::string& name, const std::string& id = "", val parentElement = val::null())
        : WebElement("select", name, id, parentElement) {}
};

struct Option : public WebElement {
    Option(const std::string& value, const std::string& label, val parentElement = val::null())
        : WebElement("option", "", "", parentElement) {
        domElement_.set("value", val(value));
        domElement_.set("label", val(label));
    }
};

struct Textarea : public WebElement {
    Textarea(const std::string& name, int rows, int cols, const std::string& id = "",
             val parentElement = val::null())
        : WebElement("textarea", name, id, parentElement) {
        domElement_.call<void>("setAttribute", val("rows"), val(rows));
        domElement_.call<void>("setAttribute", val("cols"), val(cols));
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

struct Ellipse : public WebElement {
    Ellipse(const std::string& name, double cx, double cy, double rx, double ry,
            const std::string& id = "", val parentElement = val::null())
        : WebElement("ellipse", name, id, parentElement) {
        domElement_.call<void>("setAttribute", val("cx"), val(cx));
        domElement_.call<void>("setAttribute", val("cy"), val(cy));
        domElement_.call<void>("setAttribute", val("rx"), val(rx));
        domElement_.call<void>("setAttribute", val("ry"), val(ry));
    }
};

struct Rect : public WebElement {
    Rect(const std::string& name, double x, double y, double width, double height,
         const std::string& fill, const std::string& stroke = "", double strokeWidth = 0.5,
         bool preventDuplicateIds = true, const std::string& id = "",
         val parentElement = val::null())
        : WebElement("rect", name, id, parentElement) {
        if (preventDuplicateIds) {
            val document = val::global("document");
            val oldElement = document.call<val>("getElementById", val(id));
            oldElement.call<void>("remove");
        }
        domElement_.call<void>("setAttribute", val("x"), val(x));
        domElement_.call<void>("setAttribute", val("y"), val(y));
        domElement_.call<void>("setAttribute", val("width"), val(width));
        domElement_.call<void>("setAttribute", val("height"), val(height));
        domElement_.call<void>("setAttribute", val("fill"), val(fill));
        if (stroke != "") {
            domElement_.call<void>("setAttribute", val("stroke"), val(stroke));
            domElement_.call<void>("setAttribute", val("stroke-width"), val(strokeWidth));
        }
        parentElement.call<void>("appendChild", domElement_);
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
    string clickCommand_ = "CLICK";

    void setClickCommand(string clickCommand) { clickCommand_ = clickCommand; }

    Button(const std::string& name, const std::string& displayedText, val onClickFn,
           const std::string& id = "", val parentElement = val::null())
        : WebElement("button", name, id, parentElement) {
        this->domElement_.call<void>("addEventListener", val("click"), onClickFn);
        this->domElement_.set("textContent", val(displayedText));

        channelEventListenerName_ = "click";
    }

    virtual val generateEventListenerForChannel(shared_ptr<Channel> wec) {
        val generateEventListenerForChannel_Button =
            val::global("generateEventListenerForChannel_Button");
        val evListenerFn = generateEventListenerForChannel_Button(wec, val(clickCommand_));
        return evListenerFn;
    }
};

}  // namespace cl3

EMSCRIPTEN_BINDINGS(WebElement) {
    emscripten::class_<cl3::WebElement>("WebElement")
        .function("getName", &cl3::WebElement::getName, emscripten::allow_raw_pointers())
        .function("getDomElement", &cl3::WebElement::getDomElement,
                  emscripten::allow_raw_pointers())
        .smart_ptr<std::shared_ptr<cl3::WebElement>>("WebElement");
    emscripten::class_<cl3::TextField>("TextField")
        .smart_ptr<std::shared_ptr<cl3::TextField>>("TextField");
}

#endif