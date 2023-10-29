/**
 * @file AppBuilder.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief This is the factory class that creates and then connects the signals.
 * @version 0.2
 * @date 2023-10-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef WebElementChannel3_hpp
#define WebElementChannel3_hpp

#include <memory>

#include "Channel.hpp"
#include "Util3.hpp"
#include "WebElements3.hpp"

using std::cout;
using std::map;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

namespace cl3 {

/**
 * @brief A type of channel with a WebElement at its core. The WebElement's event listeners will
 * originate signals that are then injected into the network.
 *
 */
class WebElementChannel : public Channel, public WebElement {
    // string eventListenerName_ = "change";

   protected:
    shared_ptr<WebElement> weptr_;
    string channelEventListenerName_ = "change";

   public:
    WebElementChannel(string name) : Channel(name) {}

    virtual val generateEventListenerForChannel2(shared_ptr<Channel> wec) { return val(wec); }

    virtual void setChannelEventListener2(val evListener) {
        weptr_->addEventListener(val(channelEventListenerName_), evListener);
    }

    /**
     * @brief Assigns the provided WebElement to this channel
     *
     * @param weptr
     */
    // void installWebElement(shared_ptr<WebElement> weptr) {
    //     this->weptr_ = weptr;
    //     cout << "WebElementChannel::installWebElement()" << int(this) << endl;
    //     val onChangeFn = this->weptr_->generateEventListenerForChannel(this->getptr());
    //     this->weptr_->setChannelEventListener(onChangeFn);
    // }

    virtual void inject(val s, int signalGeneration = 0) {
        cout << "WebElementChannel::inject() called!" << endl;
        Channel::inject(s, signalGeneration);
        auto domEl = weptr_->getDomElement();
        if (signalGeneration > 0) {
            domEl.set("value", s);
        }
    }

    virtual void syncFrom(string tabs = "") {
        cout << tabs << "WebElementChannel::syncFrom() for '" << name_
             << "', uid: " << Channel::getUid() << " DOES NOTHING." << endl;
    };
};

class TextFieldChannel : public WebElementChannel, public TextField {
    // virtual val generateEventListenerForChannel(shared_ptr<Channel> wec) {
    //     val generateEventListenerForChannel_TextField =
    //         val::global("generateEventListenerForChannel_TextField");
    //     val evListenerFn = generateEventListenerForChannel_TextField(wec);
    //     return evListenerFn;
    // }

    virtual void syncFrom(string tabs = "") {
        cout << tabs << "TextFieldChannel::syncFrom() for '" << name_
             << "', uid: " << Channel::getUid() << " DOES NOTHING." << endl;
    };

   public:
    // std::shared_ptr<TextFieldChannel> getptr() { return this->shared_from_this(); }

    TextFieldChannel(const std::string& name, const std::string& id = "",
                     val parentElement = val::null())
        : TextField(name, id, parentElement), WebElementChannel(name) {
        WebElementChannel::channelEventListenerName_ = "change";
    }

    virtual void finalize() {
        WebElement* thisAsWE = dynamic_cast<TextField*>(this);
        std::shared_ptr<WebElement> newWeptr(thisAsWE);
        weptr_ = newWeptr;
        val generateEventListenerForChannel =
            val::global("generateEventListenerForChannel_TextField");
        val onChangeFn = generateEventListenerForChannel(this->getptr());
        setChannelEventListener2(onChangeFn);
    }
};

class ButtonChannel : public WebElementChannel, public Button {
    string clickCommand_ = "";

   public:
    void setClickCommand(string clickCommand) { clickCommand_ = clickCommand; }

    ButtonChannel(const std::string& name, const std::string& displayedText, val onClickFn,
                  const std::string& id = "", const std::string& clickCommand = "", val parentElement = val::null())
        : Button(name, displayedText, onClickFn, id, parentElement), WebElementChannel(name) {
        WebElementChannel::channelEventListenerName_ = "click";
        ButtonChannel::clickCommand_ = clickCommand;
    }

    virtual void finalize() {
        WebElement* thisAsWE = dynamic_cast<Button*>(this);
        std::shared_ptr<WebElement> newWeptr(thisAsWE);
        weptr_ = newWeptr;

        val generateEventListenerForChannel = val::global("generateEventListenerForChannel_Button");

        val listenerFn = generateEventListenerForChannel(this->getptr(), val(ButtonChannel::clickCommand_));
        setChannelEventListener2(listenerFn);
    }
};

};  // namespace cl3

EMSCRIPTEN_BINDINGS(WebElementChannel) {
    emscripten::class_<cl3::WebElementChannel>("WebElementChannel")
        .smart_ptr<std::shared_ptr<cl3::WebElementChannel>>("WebElementChannel");
    emscripten::class_<cl3::TextFieldChannel>("TextFieldChannel")
        .smart_ptr<std::shared_ptr<cl3::TextFieldChannel>>("TextFieldChannel");
}

#endif