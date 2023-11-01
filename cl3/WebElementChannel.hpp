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
class WebElementChannel : public Channel {
    shared_ptr<WebElement> weptr_;
    string eventListenerName_ = "change";
    string channelAttributeName_ = "value";

   public:
    WebElementChannel(string name) : Channel(name) {}

    void setChannelAttributeName(const string &s) { channelAttributeName_ = s; }

    // virtual void finalize() {
    //     // val generateInjectFn = val::global("generateInjectFn");
    //     //  cout << "WebElementChannel::finalize is creating the listener for channel with
    //     address:
    //     //  "
    //     //       << int(this) << endl;
    //     //  // val onChangeFn = generateInjectFn(this->getptr());

    //     // val onChangeFn = this->weptr_->generateEventListenerForChannel(this->getptr());

    //     // this->weptr_->addEventListener(val(eventListenerName_), onChangeFn);
    // }

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