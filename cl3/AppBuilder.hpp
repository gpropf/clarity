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

    void addConnection(shared_ptr<Channel> c, bool addBack = true) {
        // if (c == getptr()) return;
        channels_.push_back(c);
        if (addBack) c->addConnection(getptr(), false);
    }

    virtual void inject(val s, int signalGeneration = 0) {
        val printVal = val::global("printVal");
        cout << "Channel name: " << name_ << ", Signal: "
             << ", signal generation = " << signalGeneration << endl;
        for (auto c : this->channels_) {
            if (signalGeneration == 0) c->inject(s, signalGeneration + 1);
        }
    }

    // virtual void injectCppval(int i, shared_ptr<Channel> originator = nullptr) {
    //     string originatorName = "NULL";
    //     if (originator) originatorName = originator->name_;
    //     cout << "Channel name: " << name_ << ", Signal: " << i << ", injected from "
    //          << originatorName << endl;
    //     for (auto c : this->channels_) {
    //         if (c != originator) c->injectCppval(i, getptr());
    //     }
    // }

    // void injectForWEC(val s) {

    //     cout << "Channel name: " << name_ << ", Signal: " << s.as<string>() << ", injected from
    //     OUTER SPACE" << endl; for (auto c : this->channels_) {
    //         c->inject(s);
    //     }
    // }
};

template <class ObjClass, typename S>
class ObjectChannel : public Channel {
    shared_ptr<ObjClass> objPtr_;
    void (ObjClass::*signalAcceptorMethod_)(const S& s);
    S (ObjClass::*signalEmitterMethod_)();

    S valToCPP(val v) {
        return std::stoi(v.as<string>());
    }

    val cppToVal(S s) {
        return val(s);
    }

    public:
    ObjectChannel(shared_ptr<ObjClass> objPtr) : objPtr_(objPtr) {}
     virtual void inject(val v, int signalGeneration = 0) {
        Channel::inject(v,signalGeneration);
        S s = valToCPP(v);
        cout << "Injected value treated as ??? " << s << endl;
     }
    
};

class WebElementChannel : public Channel {
    shared_ptr<WebElement> weptr_;

   public:
    WebElementChannel(string name) : Channel(name) {}

    // virtual void finalize() {
    //     shared_ptr<WebElementChannel> dummy;
    //     val injectForWEC = val::global("injectForWEC");
    //     val onChangeFn = injectForWEC(*this->getptr());
    //     this->weptr_->addEventListener(val("change"), onChangeFn);
    // }

    virtual void finalize() {
        val inject = val::global("inject");
        val onChangeFn = inject(*this->getptr());
        this->weptr_->addEventListener(val("change"), onChangeFn);
    }

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
    //  map<const int, val> domElements_;
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
    AppBuilder(int startId = 0, bool labelAllInputs = true, bool labelsSwallowTheirReferents = true,
               val parentDOMElement = val::null())
        : labelAllInputs_(labelAllInputs),
          labelsSwallowTheirReferents_(labelsSwallowTheirReferents),
          parentDOMElement_(parentDOMElement) {
        // tm_ = TicketMachine(startId);
    }

    // template <typename S>
    // void connect(shared_ptr<Channel<S>> c, shared_ptr<WebElementChannel<S>> wc) {
    //     c->addConnection(wc);
    // }

    const int addObject(shared_ptr<void> obj) {
        const int objid = cl3::TicketMachine::getNextSid();
        objects_.insert({objid, obj});
        pushId(objid);
        return objid;
    }

    shared_ptr<Channel> makeChannel(string name = "") {
        auto c = make_shared<Channel>(name);
        const int objid = cl3::TicketMachine::getNextSid();
        channels_.insert({objid, c});
        pushId(objid);
        return c;
    }

    shared_ptr<TextField> textField(const string& name, val parentElement = val::null()) {
        const int tfid = cl3::TicketMachine::getNextSid();
        pushId(tfid);
        auto tf = make_shared<TextField>(name, to_string(tfid), parentElement);
        // val tfDomEl = tf.getDomElement();
        webElements_.insert({tfid, tf});
        return tf;
    }

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