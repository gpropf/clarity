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

#ifndef Channel3_hpp
#define Channel3_hpp

#include <memory>

#include "Util3.hpp"
#include "WebElements3.hpp"
//#include "WebElementChannel.hpp"

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

class Channel : public std::enable_shared_from_this<Channel>, public Identifiable {
    val currentValue_;

   protected:
    vector<shared_ptr<Channel>> channels_;
    string name_;

    /**
     * @brief Adapted from this doc:
     * https://en.cppreference.com/w/cpp/memory/enable_shared_from_this
     *
     * @return std::shared_ptr<Channel>
     */
    std::shared_ptr<Channel> getptr() { return this->shared_from_this(); }

   public:
    Channel() {}
    Channel(string name) : name_(name) {}
    string getName() { return name_; }

    virtual void finalize() {}

    virtual void testMethod() { cout << "Channel::testMethod()" << endl; }

    /**
     * @brief Added the specified channel as a connection.
     *
     * @param c
     * @param addBack: if true the added connection's addConnection method is called to add the
     * caller back as a connection. This is a dangerous process that can lead to infinite recursion
     * as a signal endlessly echoes back and forth in the network. The default behavior is to set
     * this to false when calling this method on another channel.
     */
    void addConnection(shared_ptr<Channel> c, bool addBack = true) {
        // if (c == getptr()) return;
        channels_.push_back(c);
        if (addBack) c->addConnection(getptr(), false);
    }

    /**
     * @brief Probably the most important method here. This is how signals get into the signal
     * network. Essentially anything that wants to participate in the network will be a type of
     * channel and will inject values into the network which will then propagate to any other
     * channels that have been attached.
     *
     * @param s
     * @param signalGeneration
     */
    virtual void inject(val s, int signalGeneration = 0) {
        val printVal = val::global("printVal");
        cout << "Channel name: " << name_ << ", signal generation = " << signalGeneration << endl;
        cout << "<SIGNAL>";
        printVal(s);
        cout << "</SIGNAL>" << endl;
        for (auto c : this->channels_) {
            if (signalGeneration == 0) c->inject(s, signalGeneration + 1);
        }
    }

    virtual void syncFrom(string tabs = "") {
        cout << tabs << "Channel::syncFrom() for '" << name_ << "', uid: " << getUid()
             << " DOES NOTHING." << endl;
    };
    virtual void syncTo(){};
};

EMSCRIPTEN_BINDINGS(AppBuilder) {
    emscripten::class_<Channel>("Channel")
        .function("inject", &Channel::inject)
        .smart_ptr<std::shared_ptr<cl3::Channel>>("Channel");
}

};  // namespace cl3


#endif