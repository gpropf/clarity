/**
 * @file SignalBuilder.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief This is the factory class that creates and then connects the signals.
 * @version 0.1
 * @date 2023-03-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef AppBuilder3_hpp
#define AppBuilder3_hpp

// #include "SignalPrimitives.hpp"
#include "Util3.hpp"
// #include "WebElementSignals.hpp"
#include <memory>

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

class Channel : public std::enable_shared_from_this<Channel> {
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
        cout << "Channel name: " << name_ << ", Signal: "
             << ", signal generation = " << signalGeneration << endl;
        for (auto c : this->channels_) {
            if (signalGeneration == 0) c->inject(s, signalGeneration + 1);
        }
    }    
};

template<typename S>
S valToCPP(val v) { return std::stoi(v.as<string>()); }

template<>
int valToCPP(val v) { return std::stoi(v.as<string>()); }

template<>
double valToCPP(val v) { return std::stod(v.as<string>()); }

template <class ObjClass, typename S>
class ObjectChannel : public Channel {
    S signalValue_;
    shared_ptr<ObjClass> objPtr_;
    void (ObjClass::*signalAcceptorMethod_)(const S& s);
    S (ObjClass::*signalEmitterMethod_)() const;

    void setSignalEmitterMethod(S (ObjClass::*signalEmitterMethod)()) {
        signalEmitterMethod_ = signalEmitterMethod;
    }

    void setSignalAcceptorMethod(void (ObjClass::*signalAcceptorMethod)(const S& s)) {
        signalAcceptorMethod_ = signalAcceptorMethod;
    }

    //S valToCPP(val v) { return std::stoi(v.as<string>()); }

    val cppToVal(S s) { return val(s); }

   public:
    ObjectChannel(shared_ptr<ObjClass> objPtr,
                  void (ObjClass::*signalAcceptorMethod)(const S& s) = nullptr,
                  S (ObjClass::*signalEmitterMethod)() const = nullptr)
        : objPtr_(objPtr),
          signalAcceptorMethod_(signalAcceptorMethod),
          signalEmitterMethod_(signalEmitterMethod) {}

    virtual void inject(val v, int signalGeneration = 0) {
        Channel::inject(v, signalGeneration);
        S s = valToCPP<S>(v);
        cout << "Injected value treated as '" << typeid(s).name() << "'" << endl;
        (*objPtr_.*signalAcceptorMethod_)(s);
    }
};

/**
 * @brief A type of channel with a WebElement at its core. The WebElement's event listeners will
 * originate signals that are then injected into the network.
 *
 */
class WebElementChannel : public Channel {
    shared_ptr<WebElement> weptr_;
    string eventListenerName_ = "change";

   public:
    WebElementChannel(string name) : Channel(name) {}

    virtual void finalize() {
        val inject = val::global("inject");
        val onChangeFn = inject(*this->getptr());
        this->weptr_->addEventListener(val(eventListenerName_), onChangeFn);
    }

    /**
     * @brief Assigns the provided WebElement to this channel
     *
     * @param weptr
     */
    void installWebElement(shared_ptr<WebElement> weptr) { this->weptr_ = weptr; }
};

/**
 * @brief
 *
 */
// template <template <typename> class T>
class AppBuilder {
    vector<const int> currentGroupIds_;
    vector<const int> allIds_;
    map<const int, shared_ptr<Channel>> channels_;
    map<const int, shared_ptr<void>> objects_;
    map<const int, shared_ptr<WebElement>> webElements_;
    map<const string, vector<const int>> groups_;

    // TicketMachine tm_;
    bool labelAllInputs_ = true;
    bool labelsSwallowTheirReferents_ = true;
    bool addBRAfterAllCalls_ =
        true;  //!< add a '<BR>' tag after each call so everything goes on its own line.
    val parentDOMElement_ = val::null();
    // map<const string, const WebElement*>
    //     elementMap_;  //!< a map of elements we've created so we can re-use them.
    // map<string, val> attrs_;
    bool attrsResetAfterSingleUse_ = true;

    void pushId(const int id) {
        currentGroupIds_.push_back(id);
        allIds_.push_back(id);
    }

   public:
    /**
     * @brief Construct a new App Builder object. This class is the central factory class and also
     * functions as an online registry and database for the various elements of the signal network
     * you create. There are three classes of things we track and manage here:
     * 1. Objects: stored as map<const int, shared_ptr<void>> objects_. These can be of any type and
     * we make no assumptions about them
     * 2. Channels: stored as map<const int, shared_ptr<Channel>> channels_
     * 3. WebElements: stored as map<const int, shared_ptr<WebElement>> webElements_
     * 4. Groups: stored as map<const string, vector<const int>> groups_
     *
     * In addition to creating various GUI elements such as text fields and range controls, the AB
     * also watches existing objects and makes sure that changes in the internal state of objects
     * are reflected by signals in the network that ultimately cause values in GUI elements to
     * update as well.
     *
     * @param startId
     * @param labelAllInputs
     * @param labelsSwallowTheirReferents
     * @param parentDOMElement
     */
    AppBuilder(int startId = 0, bool labelAllInputs = true, bool labelsSwallowTheirReferents = true,
               val parentDOMElement = val::null())
        : labelAllInputs_(labelAllInputs),
          labelsSwallowTheirReferents_(labelsSwallowTheirReferents),
          parentDOMElement_(parentDOMElement) {
        // tm_ = TicketMachine(startId);
    }

    /**
     * @brief Adds a pre-existing object to the database.
     *
     * @param obj
     * @return const int: the assigned id
     */
    const int addObject(shared_ptr<void> obj) {
        const int objid = cl3::TicketMachine::getNextSid();
        objects_.insert({objid, obj});
        pushId(objid);
        return objid;
    }

    /**
     * @brief Creates a basic channel and ads it to the database.
     *
     * @param name
     * @return shared_ptr<Channel>
     */
    shared_ptr<Channel> makeChannel(string name = "") {
        auto c = make_shared<Channel>(name);
        const int objid = cl3::TicketMachine::getNextSid();
        channels_.insert({objid, c});
        pushId(objid);
        return c;
    }

    /**
     * @brief Creates a text field HTML element using the WebElements library
     *
     * @param name
     * @param parentElement
     * @return shared_ptr<TextField>
     */
    shared_ptr<TextField> textField(const string& name, val parentElement = val::null()) {
        const int tfid = cl3::TicketMachine::getNextSid();
        pushId(tfid);
        auto tf = make_shared<TextField>(name, to_string(tfid), parentElement);
        // val tfDomEl = tf.getDomElement();
        webElements_.insert({tfid, tf});
        return tf;
    }

    /**
     * @brief When called, assigns a name, which can then be used as a unique identifier, to the
     * most recent set of objects that have been created or added. This is meant as a way to keep
     * track of signal sub-networks so that they can be moved, copied, or deleted as a unit.
     *
     * @param groupName
     * @return vector<const int>
     */
    vector<const int> defineCurrentGroup(const string groupName) {
        vector<const int> groupIds;
        while (!currentGroupIds_.empty()) {
            groupIds.push_back(currentGroupIds_.back());
            currentGroupIds_.pop_back();
        }
        groups_.insert({groupName, groupIds});
        return groupIds;
    }

    void printGroup(const string& groupName) {
        auto ids = groups_[groupName];
        while (!ids.empty()) {
            int id = ids.back();
            ids.pop_back();
            cout << "ID: " << id << endl;
        }
    }
};

};  // namespace cl3

#endif