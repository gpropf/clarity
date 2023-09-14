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

namespace cl3 {

class IChannel {
    vector<IChannel> channels_;
    void addConnection(IChannel c) { channels_.push_back(c); }
};

template <typename S>
class Channel : public IChannel {
    S currentValue_;
};

template <typename S, class ObjClass>
class ObjectChannel : public Channel<S> {
    shared_ptr<ObjClass> objPtr;
};

template <typename S>
class WebElementChannel: public Channel<S> {

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

    const int addObject(shared_ptr<void> obj) {
        const int objid = cl3::TicketMachine::getNextSid();
        pushId(objid);
        return objid;
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

    shared_ptr<TextField> textField(const string& name, val parentElement = val::null()) {
        const int tfid = cl3::TicketMachine::getNextSid();
        pushId(tfid);
        auto tf = make_shared<TextField>(name, to_string(tfid), parentElement);
        // val tfDomEl = tf.getDomElement();
        webElements_.insert({tfid, tf});
        return tf;
    }
};
}  // namespace cl3
#endif