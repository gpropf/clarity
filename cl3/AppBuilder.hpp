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

class IChannel : public std::enable_shared_from_this<IChannel> {
   protected:
    vector<shared_ptr<IChannel>> channels_;
    string name_;

    /**
     * @brief Adapted from this doc:
     * https://en.cppreference.com/w/cpp/memory/enable_shared_from_this
     *
     * @return std::shared_ptr<IChannel>
     */
    std::shared_ptr<IChannel> getptr() { return this->shared_from_this(); }

   public:
    IChannel(string name) : name_(name) {}

    virtual void finalize() {

    }

    void addConnection(shared_ptr<IChannel> c, bool addBack = true) {
        // if (c == getptr()) return;
        channels_.push_back(c);
        if (addBack) c->addConnection(getptr(), false);
    }

    
    void inject(val s, shared_ptr<IChannel> originator = nullptr) {
        string originatorName = "NULL";
        if (originator) originatorName = originator->name_;
        cout << "Channel name: " << name_ << ", Signal: " << s.as<string>() << ", injected from "
             << originatorName << endl;
        for (auto c : this->channels_) {
            if (c != originator) c->inject(s, getptr());
        }
    }
};


class Channel : public IChannel {
    val currentValue_;

   public:
    Channel(string name) : IChannel(name) {}
};

template <class ObjClass>
class ObjectChannel : public Channel {
    shared_ptr<ObjClass> objPtr;
};


class WebElementChannel : public Channel {
    shared_ptr<WebElement> weptr_;

   public:
    WebElementChannel(string name) : Channel(name) {
                    
    }

    virtual void finalize() {
        val elg = val::global("elgCallMethodOnObjByName");
       // val onChangeFn = elg(*this->weptr_, val("forceDeleteDomElementOnExit"));
        //this->weptr_->addEventListener(val("change"), onChangeFn);
    }
};

/**
 * @brief
 *
 */
// template <template <typename> class T>
class AppBuilder {
    vector<const int> currentGroupIds_;
    vector<const int> allIds_;
    map<const int, shared_ptr<IChannel>> channels_;
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

    
    shared_ptr<Channel> addChannel(string name = "") {
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