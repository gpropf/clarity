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

#ifndef AppBuilder3_hpp
#define AppBuilder3_hpp

#include <memory>

#include "Channel.hpp"
#include "ObjectChannel.hpp"
#include "Util3.hpp"
#include "WebElementChannel.hpp"
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
        if (running_) start();
    }

    virtual ~Ticker() {}

    /**
     * @brief We overload this in subclasses to create the tick callback for each class.
     *
     */
    virtual void generateTickFunction() = 0;

    /**
     * @brief Must be overloaded to define tick behavior.
     *
     */
    virtual void tick() = 0;

    bool isRunning() { return running_; }

    virtual void start(int tickInterval = 250) {
        cout << "DBG AppBuilder::start()" << endl;
        if (tickInterval != -1) tickInterval_ = tickInterval;
        if (tickJS_ == val::null()) generateTickFunction();
        timerId_ = setInterval_(tickJS_, val(tickInterval_));
        running_ = true;
    }

    void stop() {
        cout << "DBG AppBuilder::stop()" << endl;
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
};

template <typename S>
using getterSetterPair = std::pair<std::function<S()>, std::function<void(S)>>;

class AppBuilder : public std::enable_shared_from_this<AppBuilder>, public Ticker {
    vector<const int> currentGroupIds_;
    vector<const int> allIds_;
    map<const int, shared_ptr<Channel>> channels_;
    map<const int, shared_ptr<void>> objects_;
    map<const int, shared_ptr<WebElement>> webElements_;
    map<const string, vector<const int>> groups_;

    string state_ = "";

    bool labelAllInputs_ = true;
    bool labelsSwallowTheirReferents_ = true;
    bool addBRAfterAllCalls_ =
        true;  //!< add a '<BR>' tag after each call so everything goes on its own line.
    val parentDOMElement_ = val::null();

    // map<string, val> webElementAttrs_;
    bool attrsResetAfterSingleUse_ = true;

   public:
    std::shared_ptr<AppBuilder> getptr() { return this->shared_from_this(); }

    void pushId(const int id) {
        currentGroupIds_.push_back(id);
        allIds_.push_back(id);
    }

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
          parentDOMElement_(parentDOMElement) {}

    void syncFrom(string prefix = "") {
        cout << "AppBuilder::syncFrom()" << endl;
        for (auto [id, c] : channels_) {
            cout << "AppBuilder::syncFrom(), Channel: '" << c->getName() << "'"
                 << ", id: " << id << endl;
            c->syncFrom("\tAB: " + prefix);
        }
    }

    virtual void start(int tickInterval = 250) { Ticker::start(tickInterval); }

    void tick() {
        cout << "AppBuilder says TICK!" << endl;
        syncFrom("tick: ");
    }

    void threadTestFn() {
        cout << "AppBuilder says hello from the button click!" << endl;
        syncFrom("threadTestFn: ");
        printGroup("g1", "button callback\t:");
    }

    void doNothing() {
        // syncFrom();
        cout << "AppBuilder says it's doing nothing right now!" << endl;
    }

    /**
     * @brief This installs this class' `tick()` method to be called from the `setInterval()` JS
     * function.
     *
     */
    virtual void generateTickFunction() {
        val TickerTick = val::global("TickerTick");
        tickJS_ = TickerTick(this->getptr());
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
     * @brief Adds an existing channel to the database.
     *
     * @param c
     * @return const int
     */
    const int addChannel(shared_ptr<Channel> c) {
        const int cid = cl3::TicketMachine::getNextSid();
        c->setUid(cid);
        c->finalize();
        channels_.insert({cid, c});
        pushId(cid);
        return cid;
    }

    /**
     * @brief Adds an existing web element to the database.
     *
     * @param wel
     * @return const int
     */
    const int addWebElement(shared_ptr<WebElement> wel) {
        const int id = cl3::TicketMachine::getNextSid();
        wel->setUid(id);
        webElements_.insert({id, wel});
        pushId(id);
        return id;
    }

    /**
     * @brief Creates a basic channel and ads it to the database.
     *
     * @param name
     * @return shared_ptr<Channel>
     */
    auto makeChannel(string name = "") {
        auto c = make_shared<Channel>(name);
        const int cid = addChannel(c);
        return std::make_pair(c, cid);
    }

    /**
     * @brief Same kind of factory method as above but for a WEC.
     *
     * @param name
     * @return auto
     */
    auto makeWebElementChannel(string name = "") {
        auto c = make_shared<WebElementChannel>(name);
        const int cid = addChannel(c);
        return std::make_pair(c, cid);
    }

    /**
     * @brief Creates a text field HTML element using the WebElements library
     *
     * @param name
     * @param parentElement
     * @return auto
     */
    auto textField(const string& name, val parentElement = val::null()) {
        auto tf = make_shared<TextField>(name, to_string(-2), parentElement);
        const int tfid = addWebElement(tf);
        return std::make_pair(tf, tfid);
    }

    auto textFieldChannel(const string& name, val parentElement = val::null()) {
        auto c = make_shared<TextFieldChannel>(name, to_string(-2), parentElement);
        const int cid = addChannel(c);
        return std::make_pair(c, cid);
    }

    /**
     * @brief Get's a (hopefully) unique id from the TicketMachine. Also adds that id to the stack
     * of ids that will become the next group when you create one.
     *
     * @return const int
     */
    const int getNewId() {
        const int id = cl3::TicketMachine::getNextSid();
        pushId(id);
        return id;
    }

    /**
     * @brief Uses the WebElements library to make a button element.
     *
     * @param name
     * @param displayedText
     * @param onClickFn
     * @param parentElement
     * @return auto
     */
    auto button(const string& name, const std::string& displayedText, val onClickFn,
                val parentElement = val::null()) {
        auto btn =
            make_shared<Button>(name, displayedText, onClickFn, to_string(-3), parentElement);
        const int id = addWebElement(btn);
        return std::make_pair(btn, id);
    }

    auto buttonChannel(const string& name, const std::string& displayedText, val onClickFn,
                       const std::string& clickCommand, val parentElement = val::null()) {
        auto c = make_shared<ButtonChannel>(name, displayedText, onClickFn, to_string(-3),
                                            clickCommand, parentElement);
        const int cid = addChannel(c);
        return std::make_pair(c, cid);

        // auto btn =
        //     make_shared<Button>(name, displayedText, onClickFn, to_string(-3), parentElement);
        // const int id = addWebElement(btn);
        // return std::make_pair(btn, id);
    }

    /**
     * @brief Creates a range control using the WebElements library
     *
     * @param name
     * @param parentElement
     * @return auto
     */
    auto rangeInput(const string& name, val parentElement = val::null()) {
        auto tf = make_shared<RangeInput>(name, to_string(-4), parentElement);
        const int tfid = addWebElement(tf);
        return std::make_pair(tf, tfid);
    }

    shared_ptr<WebElement> getWebElement(int id) const { return webElements_.at(id); }

    shared_ptr<Channel> getChannel(const int id) { return channels_.at(id); }

    int getNumWebElements() { return webElements_.size(); }

    string getState() const { return state_; }

    void setState(const string& newState) {
        if (newState == "CLICK") {
            toggle();
            if (running_) {
                state_ = "RUNNING";
            } else {
                state_ = "NOT_RUNNING";
            }
        }
    }

    void listWebElements() {
        for (auto [id, el] : webElements_) {
            cout << "ID: " << id << ", " << el->getName() << endl;
        }
    }

    void listChannels() {
        cout << "Listing channels..." << endl;
        for (auto [id, el] : channels_) {
            cout << "ID: " << id << ", " << el->getName() << endl;
        }
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
            auto id = currentGroupIds_.back();
            cout << "ADDING " << id << " to group " << groupName << endl;
            groupIds.push_back(id);
            currentGroupIds_.pop_back();
        }
        groups_.insert({groupName, groupIds});
        // groups__.insert({groupName, groupIds});
        return groupIds;
    }

    void printGroup(const string& groupName, string prefixTabs = "") {
        auto ids = groups_[groupName];
        if (ids.empty()) {
            cout << "Group " << groupName << " not found or is empty." << endl;
            return;
        }
        cout << "Group " << groupName << " contents:" << endl;
        while (!ids.empty()) {
            int id = ids.back();
            ids.pop_back();
            cout << prefixTabs << "ID: " << id << endl;
        }
    }
};

};  // namespace cl3

EMSCRIPTEN_BINDINGS(AppBuilder) {
    emscripten::class_<cl3::AppBuilder>("AppBuilder")
        .smart_ptr<std::shared_ptr<cl3::AppBuilder>>("AppBuilder")
        .function("tick", &cl3::AppBuilder::tick, emscripten::allow_raw_pointers())
        .function("threadTestFn", &cl3::AppBuilder::threadTestFn, emscripten::allow_raw_pointers())

        .function("start", &cl3::AppBuilder::start)
        .function("printGroup", &cl3::AppBuilder::printGroup)
        .function("getChannel", &cl3::AppBuilder::getChannel, emscripten::allow_raw_pointers())
        .function("getWebElement", &cl3::AppBuilder::getWebElement,
                  emscripten::allow_raw_pointers())
        .function("listWebElements", &cl3::AppBuilder::listWebElements,
                  emscripten::allow_raw_pointers())
        .function("listChannels", &cl3::AppBuilder::listChannels, emscripten::allow_raw_pointers())
        .function("getNumWebElements", &cl3::AppBuilder::getNumWebElements,
                  emscripten::allow_raw_pointers());
    // .class_function("tick__", &cl3::AppBuilder::tick__, emscripten::allow_raw_pointers())
    // .class_function("pushId__", &cl3::AppBuilder::pushId__, emscripten::allow_raw_pointers())
    // .class_function("textField__", &cl3::AppBuilder::textField__,
    //                 emscripten::allow_raw_pointers())
    // .class_function("getSingleton", &cl3::AppBuilder::getSingleton,
    //                 emscripten::allow_raw_pointers());

    emscripten::class_<cl3::Ticker>("Ticker").function("start", &cl3::Ticker::start);
}

#endif