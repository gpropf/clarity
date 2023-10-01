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

class Ticker {
   protected:
    val setInterval_ = val::global("setInterval");
    val clearInterval_ = val::global("clearInterval");
    val tickJS_ = val::null();
    val timerId_;
    int tickInterval_ = 500;
    bool running_ = false;

   public:
    Ticker(int tickInterval = 500, bool running_ = false)
        : tickInterval_(tickInterval), running_(running_) {
        // tickJS_ = val::global("TickerTick")(*this);
        if (running_) start();
    }

    virtual ~Ticker() {}

    /**
     * @brief Ideally we would have this method simply have this code to install the tick method
     * `tickJS_ = val::global("TickerTick")(*this);`
     *
     * In practice this does not compile and produces errors about how `tick()` is a pure virtual
     * method. Thus you need to overload this method in your subclass with the above line of code.
     * The result will be that it will call your overloaded `tick()` method.
     *
     */
    virtual void generateTickFunction() = 0;

    bool isRunning() { return running_; }

    void start(int tickInterval = -1) {
        if (tickInterval != -1) tickInterval_ = tickInterval;
        if (tickJS_ == val::null()) generateTickFunction();
        timerId_ = setInterval_(tickJS_, val(tickInterval_));
        running_ = true;
    }

    void stop() {
        clearInterval_(timerId_);
        running_ = false;
    }

    bool toggle() {
        if (running_) {
            stop();
            return false;
        } else {
            start();
            return true;
        }
    }

    // virtual void tick() { cout << "TICKER TICKS!" << endl; }
    virtual void tick() = 0;
};

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

    virtual void syncFrom() { cout << "Channel::syncFrom() for '" << name_ << "'" << endl; };
    virtual void syncTo(){};
};

template <typename S>
S valToCPP(val v) {
    return std::stoi(v.as<string>());
}

template <>
int valToCPP(val v) {
    return std::stoi(v.as<string>());
}

template <>
double valToCPP(val v) {
    return std::stod(v.as<string>());
}

template <class ObjClass, typename S>
class ObjectChannel : public Channel {
    S signalValue_;
    shared_ptr<ObjClass> objPtr_;
    void (ObjClass::*setter_)(const S& s);
    S (ObjClass::*getter_)() const;

    void setGetterMethod(S (ObjClass::*getter)()) { getter_ = getter; }

    void setSetterMethod(void (ObjClass::*setter)(const S& s)) { setter_ = setter; }

    // S valToCPP(val v) { return std::stoi(v.as<string>()); }

    val cppToVal(S s) { return val(s); }

   public:
    ObjectChannel(shared_ptr<ObjClass> objPtr, string name = "",
                  void (ObjClass::*setter)(const S& s) = nullptr,
                  S (ObjClass::*getter)() const = nullptr)
        : objPtr_(objPtr), setter_(setter), getter_(getter) {
        name_ = name;
    }

    virtual void inject(val v, int signalGeneration = 0) {
        Channel::inject(v, signalGeneration);
        S s = valToCPP<S>(v);
        cout << "Injected value treated as '" << typeid(s).name() << "'" << endl;
        (*objPtr_.*setter_)(s);
    }

    virtual void syncFrom() {
        cout << "ObjectChannel::syncFrom() for '" << name_ << "'" << endl;
        val v = val(to_string((*objPtr_.*getter_)()));
        inject(v);
    };
    virtual void syncTo(){};
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
    void installWebElement(shared_ptr<WebElement> weptr) {
        this->weptr_ = weptr;
        if (typeid(*weptr) == typeid(RangeInput)) {
            eventListenerName_ = "input";
            cout << "This is a range input, so we use the onInput listener!" << endl;
        }
    }

    virtual void inject(val s, int signalGeneration = 0) {
        auto domEl = weptr_->getDomElement();
        if (signalGeneration > 0) {
            domEl.set("value", s);
        }
    }

    virtual void syncFrom() {
        cout << "WebElementChannel::syncFrom() for '" << name_ << "'" << endl;
    };
};

template <typename S>
using getterSetterPair = std::pair<std::function<S()>, std::function<void(S)>>;

/**
 * @brief
 *
 */
// template <template <typename> class T>
class AppBuilder : public Ticker {
    vector<const int> currentGroupIds_;
    vector<const int> allIds_;
    map<const int, shared_ptr<Channel>> channels_;
    map<const int, std::function<int()>> intFunctions_;
    map<const int, std::function<void()>> hookFunctions_;
    map<const int, std::function<void(int)>> setIntFunctions_;
    map<const int, shared_ptr<void>> objects_;

    map<const int, shared_ptr<WebElement>> webElements_;
    map<const string, vector<const int>> groups_;

    // val setInterval_ = val::global("setInterval");
    // val tickJS_;
    // int tickInterval_ = 500;
    // val timerId_;

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

        //  iterateOnceJS();
        //  cout << "Called elgCallMethodOnObjByName..." << endl;

        // val setInterval = val::global("setInterval");

        // tickJS_ = val::global("appBuilderTick");
        // val tickJSFn = tickJS_(*this);
        //
        // val timerId_ = setInterval_(tickJSFn, val(tickInterval_));
    }

    // void initTick() {
    //     val jsMethodCallerFn = val::global("elgCallMethodOnObjByName");
    //     val iterateOnceJS = jsMethodCallerFn(*this, val("tick"));
    //     timerId_ = setInterval_(iterateOnceJS, val(tickInterval_));
    // }

    void syncFrom() {
        cout << "AppBuilder::syncFrom()" << endl;
        for (auto [id, c] : channels_) {
            cout << "Channel: '" << c->getName() << "'" << endl;
            c->syncFrom();
        }
    }

    void runHookFns() {
        for (auto [id, fn] : hookFunctions_) {
            cout << "Calling Hook Function with id = " << id << endl;
            fn();
        }
    }

    void tick() {
        cout << "AppBuilder says TICK!" << endl;
        syncFrom();
        runHookFns();
    }

    /**
     * @brief This installs this class' `tick()` method to be called from the `setInterval()` JS
     * function.
     *
     */
    virtual void generateTickFunction() { tickJS_ = val::global("TickerTick")(*this); }

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

    const int addIntFunction(std::function<int()> intfn) {
        const int objid = cl3::TicketMachine::getNextSid();
        intFunctions_.insert({objid, intfn});
        pushId(objid);
        return objid;
    }

    const int addHookFunction(std::function<void()> hookfn) {
        const int objid = cl3::TicketMachine::getNextSid();
        hookFunctions_.insert({objid, hookfn});
        pushId(objid);
        return objid;
    }

    const int addSetIntFunction(std::function<void(int)> intfn) {
        const int objid = cl3::TicketMachine::getNextSid();
        setIntFunctions_.insert({objid, intfn});
        pushId(objid);
        return objid;
    }

    /**
     * @brief Adds an existing channel to the database.
     *
     * @param c
     * @return const int
     */
    const int addChannel(shared_ptr<Channel> c) {
        const int objid = cl3::TicketMachine::getNextSid();
        channels_.insert({objid, c});
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
     * @brief Creates a range control using the WebElements library
     *
     * @param name
     * @param parentElement
     * @return shared_ptr<TextField>
     */
    shared_ptr<RangeInput> rangeInput(const string& name, val parentElement = val::null()) {
        const int tfid = cl3::TicketMachine::getNextSid();
        pushId(tfid);
        auto tf = make_shared<RangeInput>(name, to_string(tfid), parentElement);
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

EMSCRIPTEN_BINDINGS(AppBuilder) {
    emscripten::class_<cl3::AppBuilder>("AppBuilder")
        .function("tick", &cl3::AppBuilder::tick);

    // emscripten::class_<cl3::Ticker>("Ticker").function("tick", &cl3::Ticker::tick,
    //                                                    emscripten::allow_raw_pointers());
}

#endif