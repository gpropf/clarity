/**
 * @file WebElementChannel.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief An interface between web elements such as textfields and a channel. This is how you
 * connect your GUI elements to the signal pipeline.
 * @version 0.2
 * @date 2023-10-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef WebElementChannel3_hpp
#define WebElementChannel3_hpp

#include <memory>

#include "Util.hpp"
#include "WebElements.hpp"

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
class WebElementChannel : public Channel {
    shared_ptr<WebElement> weptr_;
    string eventListenerName_ = "change";
    string channelAttributeName_ = "value";

   public:
    WebElementChannel(string name) : Channel(name) {}

    void setChannelAttributeName(const string &s) { channelAttributeName_ = s; }
    
    /**
     * @brief Assigns the provided WebElement to this channel
     *
     * @param weptr
     */
    void installWebElement(shared_ptr<WebElement> weptr) {
        this->weptr_ = weptr;
        cout << "WebElementChannel::installWebElement()" << int(this) << endl;
        val onChangeFn = this->weptr_->generateEventListenerForChannel(this->getptr());
        this->weptr_->setChannelEventListener(onChangeFn);
    }

    virtual void inject(val s, int signalGeneration = 0) {
        cout << "WebElementChannel::inject() called!" << endl;
        Channel::inject(s, signalGeneration);
        auto domEl = weptr_->getDomElement();
        if (signalGeneration > 0) {
            domEl.set(channelAttributeName_, s);
            domEl.call<void>("setAttribute", val(channelAttributeName_), s);
        }
    }

    virtual void syncFrom(string tabs = "") {
        cout << tabs << "WebElementChannel::syncFrom() for '" << name_ << "', uid: " << getUid()
             << " DOES NOTHING." << endl;
    };
};

};  // namespace cl3

EMSCRIPTEN_BINDINGS(WebElementChannel) {}

#endif