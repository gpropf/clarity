/**
 * @file WebElementChannel.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief An interface between web elements such as textfields and a channel. This is how you
 * connect your GUI elements to the signal pipeline.
 * @version 0.3
 * @date 2023-10-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef WebElementChannel3_hpp
#define WebElementChannel3_hpp

#include <memory>

#include "Metaval.hpp"
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

    virtual void injectMetaval(shared_ptr<Metaval> mv, int signalGeneration = 0) {
        cout << "WebElementChannel::injectMetaval() called!" << endl;
        Channel::injectMetaval(mv, signalGeneration);
        auto domEl = weptr_->getDomElement();
        if (signalGeneration > 0) {
            domEl.set(channelAttributeName_, val(mv->toString()));
            domEl.call<void>("setAttribute", val(channelAttributeName_), val(mv->toString()));
        }
    }

    virtual void syncFrom(string tabs = "") {
        cout << tabs << "WebElementChannel::syncFrom() for '" << name_ << "', uid: " << getUid()
             << " DOES NOTHING." << endl;
    }

    static shared_ptr<cl3::Metaval> makeMetaval(int i) {
        shared_ptr<MetavalInt> mvp = std::make_shared<MetavalInt>(i);
        return std::dynamic_pointer_cast<Metaval>(mvp);
    };

    static shared_ptr<Metaval> makeMetaval2D(double x, double y) {
        shared_ptr<Metaval2D> mvp = std::make_shared<Metaval2D>(x, y);
        return std::dynamic_pointer_cast<Metaval>(mvp);
    };

    /**
     * @brief Intended to be called from JS event handlers that return a location value.
     *
     * @param x
     * @param y
     * @return std::pair<double, double>
     */
    static std::pair<double, double> makeDoublePair(double x, double y) { return std::pair(x, y); }

    /**
     * @brief Intended to be called from JS event handlers that return an integer location value.
     *
     * @param x
     * @param y
     * @return std::pair<double, double>
     */
    static std::pair<int, int> makeIntPair(int x, int y) { return std::pair(x, y); }
};

EMSCRIPTEN_BINDINGS(WebElementChannel) {
    emscripten::class_<std::pair<double, double>>("dblpair");
    emscripten::class_<std::pair<int, int>>("intpair");

    emscripten::class_<WebElementChannel>("WebElementChannel")
        .function("inject", &WebElementChannel::inject, emscripten::allow_raw_pointers())
        .class_function("makeDoublePair", &WebElementChannel::makeDoublePair)
        .class_function("makeIntPair", &WebElementChannel::makeIntPair)
        .class_function("makeMetaval", &WebElementChannel::makeMetaval)
        .class_function("makeMetaval2D", &WebElementChannel::makeMetaval2D);
    // emscripten::class_<cl3::WebElementChannel>("WebElementChannel")
    //     .class_function("makeDoublePair", &cl3::WebElementChannel::makeDoublePair);
}

};  // namespace cl3

#endif