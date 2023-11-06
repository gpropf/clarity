/**
 * @file AppBuilder.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief This is the factory class that creates and then connects the signals.
 * @version 0.3
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
#include "Util.hpp"
#include "WebElementChannel.hpp"
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

class Ticker {
   protected:
    val setInterval_ = val::global("setInterval");
    val clearInterval_ = val::global("clearInterval");
    val tickJS_ = val::null();
    val timerId_;
    int tickInterval_ = 500;
    bool running_ = false;
    string state_ = "";

   public:
    Ticker(int tickInterval = 500, bool running_ = false)
        : tickInterval_(tickInterval), running_(running_) {
        if (running_) start();
    }

    virtual ~Ticker() {}

    /**
     * @brief Manipulates the Ticker's state machine. By defining an object channel that can set
     * or get the string state value and then connecting that channel to a button channel, you can
     * create a nice toggle control for the Ticker. This is illustrated in the demo app.
     *
     * @param newState
     */
    virtual void setState(const string& newState) {
        if (newState == "CLICK") {
            toggle();
            if (running_) {
                state_ = "RUNNING";
            } else {
                state_ = "NOT_RUNNING";
            }
        }
    }

    string getState() const { return state_; }

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

    virtual void start(int tickInterval = -1) {
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

// template <typename S>
// using getterSetterPair = std::pair<std::function<S()>, std::function<void(S)>>;

class AppBuilder : public std::enable_shared_from_this<AppBuilder>, public Ticker {
    vector<const int> currentGroupIds_;
    vector<const int> allIds_;
    map<const int, shared_ptr<Channel>> channels_;
    map<const int, shared_ptr<void>> objects_;
    map<const int, shared_ptr<WebElement>> webElements_;
    map<const string, vector<const int>> groups_;

    // string state_ = "";

    bool labelAllInputs_ = true;
    bool labelsSwallowTheirReferents_ = true;
    bool addBRAfterAllCalls_ =
        true;  //!< add a '<BR>' tag after each call so everything goes on its own line.
    val parentDOMElement_ = val::null();

    // map<string, val> webElementAttrs_;
    bool attrsResetAfterSingleUse_ = true;

   public:
    std::shared_ptr<AppBuilder> getptr() { return this->shared_from_this(); }

    /**
     * @brief Push an id onto the "stack" for later grouping with other ids.
     *
     * @param id
     */
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

    /**
     * @brief Could probably also have been called "pull". Basically, this method calls the method
     * of the same name in the channels. Some of these are going to be pulling values ib from
     * objects and this ultimately how those values are pulled into the signal network.
     *
     * @param prefix during testing we sometimes call this method from methods besides `tick()`.
     * This allows us to see what called the method in the debug output.
     */
    void syncFrom(string prefix = "") {
        cout << "AppBuilder::syncFrom()" << endl;
        for (auto [id, c] : channels_) {
            cout << "AppBuilder::syncFrom(), Channel: '" << c->getName() << "'"
                 << ", id: " << id << endl;
            c->syncFrom("\tAB: " + prefix);
        }
    }

    // virtual void start(int tickInterval = 250) { Ticker::start(tickInterval); }

    void tick() {
        cout << "AppBuilder says TICK!" << endl;
        syncFrom("tick: ");
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
     * @brief Adds a <br> tag contingent on the setting of the `addBRAfterAllCalls_` value.
     *
     */
    void maybeBR() {
        if (addBRAfterAllCalls_) BR();
    }

    /**
     * @brief Convenience method to allow you to wrap an element with a label tag.
     *
     * @param descriptionText
     * @param wel
     * @param parentElement
     * @return auto
     */
    auto label(const std::string& descriptionText, const WebElement& wel,
               val parentElement = val::null()) {
        auto label = make_shared<Label>(descriptionText, wel, labelsSwallowTheirReferents_,
                                        to_string(-10), parentElement);
        const int labelId = addWebElement(label);
        return std::make_pair(label, labelId);
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
        maybeBR();
        return std::make_pair(tf, tfid);
    }

    /**
     * @brief Creates a labelled text field HTML element using the WebElements library. Note that,
     * while the AB records the id of the label element, it does not return the label element or its
     * id.
     *
     * @param name
     * @param parentElement
     * @return auto
     */
    auto textField(const string& name, const string& descriptionText,
                   val parentElement = val::null()) {
        auto tf = make_shared<TextField>(name, to_string(-2), parentElement);
        const int tfid = addWebElement(tf);

        auto label = make_shared<Label>(descriptionText, *tf, labelsSwallowTheirReferents_,
                                        to_string(-10), parentElement);
        const int labelId = addWebElement(label);
        maybeBR();
        return std::make_pair(tf, tfid);
    }

    auto textFieldChannel(const string& name, const string& descriptionText,
                          val parentElement = val::null()) {
        auto [tf, tfid] = textField(name, descriptionText, parentElement);
        auto [c, cid] = makeWebElementChannel(name);
        c->installWebElement(tf);
        return c;
    }

    /**
     * @brief Create an SVG area using the WebElements library.
     *
     * @param name
     * @param width
     * @param height
     * @param id
     * @param parentElement
     * @return auto
     */
    auto svg(const std::string& name, int width, int height, const std::string& id = "",
             val parentElement = val::null()) {
        auto svg = make_shared<SVG>(name, width, height, to_string(-3), parentElement);
        const int svgid = addWebElement(svg);
        maybeBR();
        return std::make_pair(svg, svgid);
    }

    /**
     * @brief Create an ellipse area using the WebElements library.
     *
     * @param name
     * @param cx
     * @param cy
     * @param rx
     * @param ry
     * @param parentElement
     * @return auto
     */
    auto ellipse(const std::string& name, double cx, double cy, double rx, double ry,
                 val parentElement = val::null()) {
        auto ellipse = make_shared<Ellipse>(name, cx, cy, rx, ry, to_string(-4), parentElement);
        const int ellipseId = addWebElement(ellipse);
        return std::make_pair(ellipse, ellipseId);
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
    auto button(const string& name, const std::string& displayedText, val onClickFn = val::null(),
                val parentElement = val::null()) {
        auto btn =
            make_shared<Button>(name, displayedText, onClickFn, to_string(-3), parentElement);
        const int id = addWebElement(btn);
        maybeBR();
        return std::make_pair(btn, id);
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
        maybeBR();
        return std::make_pair(tf, tfid);
    }

    /**
     * @brief This is the labelled form of rangeInput.
     *
     * @param name
     * @param descriptionText
     * @param parentElement
     * @return auto
     */
    auto rangeInput(const string& name, const string& descriptionText,
                    val parentElement = val::null()) {
        auto [range, rangeId] = rangeInput(name, parentElement);
        auto [lbl, lblId] = label(descriptionText, *range, parentElement);
        maybeBR();
        return std::make_pair(range, rangeId);
    }

    shared_ptr<WebElement> getWebElement(int id) const { return webElements_.at(id); }

    shared_ptr<Channel> getChannel(const int id) { return channels_.at(id); }

    int getNumWebElements() { return webElements_.size(); }

    /**
     * @brief Basically for debugging.
     *
     */
    void listWebElements() {
        for (auto [id, el] : webElements_) {
            cout << "ID: " << id << ", " << el->getName() << endl;
        }
    }

    /**
     * @brief Basically for debugging.
     *
     */
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

    /**
     * @brief Print details of the named group to the console.
     *
     * @param groupName
     * @param prefixTabs
     */
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

    emscripten::class_<cl3::Ticker>("Ticker").function("start", &cl3::Ticker::start);
}

#endif