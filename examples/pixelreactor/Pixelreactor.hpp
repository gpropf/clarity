#ifndef Pixelreactor_hpp
#define Pixelreactor_hpp

#include <set>
#include <utility>

#include "GridControl.hpp"
#include "Signal.hpp"
#include "SignalBuilder.hpp"
#include "SignalPrimitives.hpp"
#include "Util.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

using namespace cl2;
using namespace std::chrono;

/**
 * @brief My own stringification function once I saw that to_string() wasn't always working.
 *
 * @tparam T
 * @param v
 * @return std::string
 */
template <typename T>
std::string clto_str(const T &v) {
    std::ostringstream os;
    os << v;
    return os.str();
}

template <typename V>
struct Vec2 {
    V x;
    V y;
};

template <typename V>
struct RotationMatrix2D {
    const V r1c1_, r1c2_, r2c1_, r2c2_;
    const int angle_;

    RotationMatrix2D(V r1c1, V r1c2, V r2c1, V r2c2, int angle)
        : r1c1_(r1c1), r1c2_(r1c2), r2c1_(r2c1), r2c2_(r2c2), angle_(angle) {}

    std::pair<V, V> rotateCoordinates(const std::pair<V, V> &coords) const {
        auto [x, y] = coords;
        V xprime = r1c1_ * x + r1c2_ * y;
        V yprime = r2c1_ * x + r2c2_ * y;
        return std::pair(xprime, yprime);
    }

    // void rotateCoordinates(std::pair<V, V> &coords) const {
    //     auto [x, y] = coords;
    //     V xprime = r1c1_ * x + r1c2_ * y;
    //     V yprime = r2c1_ * x + r2c2_ * y;
    //     //x = xprime;
    //     //y = yprime;
    //     coords = std::make_pair(xprime,yprime);
    // }

    // static const RotationMatrix2D<V> rr0; // = RotationMatrix2D<V>(1, 0, 0, 1, 0);
};

const RotationMatrix2D r0 = RotationMatrix2D(1, 0, 0, 1, 0);
const RotationMatrix2D r90 = RotationMatrix2D(0, -1, 1, 0, 90);
const RotationMatrix2D r180 = RotationMatrix2D(-1, 0, 0, -1, 180);
const RotationMatrix2D r270 = RotationMatrix2D(0, 1, -1, 0, 270);

// template <typename U>
// class Beaker;

/**
 * @brief Represents a single "reaction vessel" in which our experiments can take place. The
 * reaction rules that determine how patterns in the grid transform will use the same GridControl
 * control the beaker itself does.
 *
 * @tparam V This is the type we are using for the grid elements. The original app I wrote in
 * ClojureScript used small positive integers for the colors so the expected type here is `unsigned
 * char`. Theoretically, it's possible to use other types though.
 */
template <typename V>
class Beaker : public std::enable_shared_from_this<Beaker<V>> {
   public:
    typedef short int
        priorityT;  // I'm using an int type for the text input field for priorities because the
                    // values weren't "sticking" otherwise. This is a type related bug.
    typedef std::pair<V, priorityT> valuePriorityPairT;
    typedef int gridCoordinateT;
    typedef std::pair<gridCoordinateT, gridCoordinateT> gridCoordinatePairT;
    typedef std::pair<gridCoordinatePairT, V> gridCoordinatesValueTripletT;
    static const RotationMatrix2D<gridCoordinateT> rotationMatrices[4];  // = {r0, r90, r180, r270};

   protected:
    shared_ptr<cl2::SignalBuilder> signalBuilder_;
    val signalBuilderDomElement_ = val::null();
    shared_ptr<Div> beakerDiv_;

    shared_ptr<GridControl<V>> gridControl_;
    shared_ptr<WebElementSignal<std::string>> nameInput_;
    shared_ptr<WebElementSignal<std::string>> successorNameInput_;
    shared_ptr<WebElementSignal<std::string>> successorPriorityInput_;
    shared_ptr<WebElementSignal<std::string>> successorOffsetXInput_;
    shared_ptr<WebElementSignal<std::string>> successorOffsetYInput_;
    shared_ptr<ObjectSignalLoop<std::string, Beaker<unsigned char>>> nameInputLoop_;
    shared_ptr<ObjectSignalLoop<std::string, Beaker<unsigned char>>> successorNameInputLoop_;
    shared_ptr<ObjectSignalLoop<std::string, Beaker<unsigned char>>> successorPriorityInputLoop_;
    shared_ptr<ObjectSignalLoop<std::string, Beaker<unsigned char>>> successorOffsetXInputLoop_;
    shared_ptr<ObjectSignalLoop<std::string, Beaker<unsigned char>>> successorOffsetYInputLoop_;
    // shared_ptr<WebElementSignal<std::string>> newRuleButtonOutput_;
    shared_ptr<WebElementSignal<std::string>> newRuleButton_;
    shared_ptr<WebElementSignal<std::string>> debugRulesButton_;
    shared_ptr<WebElementSignal<std::string>> iterateOnceButton_;
    shared_ptr<WebElementSignal<std::string>> runButton_;
    shared_ptr<WebElementSignal<std::string>> deleteRuleButton_;
    shared_ptr<WebElementSignal<std::string>> clearButton_;
    shared_ptr<WebElementSignal<std::string>> loadButton_;
    shared_ptr<ObjectAcceptor<std::string, Beaker<V>>> loadAcceptor_;
    shared_ptr<WebElementSignal<std::string>> saveButton_;
    shared_ptr<ObjectAcceptor<std::string, Beaker<V>>> saveAcceptor_;
    shared_ptr<ObjectAcceptor<std::string, Beaker<V>>> clearAcceptor_;
    shared_ptr<ObjectAcceptor<std::string, Beaker<V>>> makeNewRuleAcceptor_;
    shared_ptr<ObjectAcceptor<std::string, Beaker<V>>> makeDebugRulesAcceptor_;
    shared_ptr<ObjectAcceptor<std::string, Beaker<V>>> iterateOnceAcceptor_;
    shared_ptr<ObjectAcceptor<std::string, Beaker<V>>> runAcceptor_;

    shared_ptr<WebElementSignal<std::string>> iterationIntervalInput_;
    shared_ptr<WebElementSignal<std::string>> ruleWidthInput_;
    shared_ptr<WebElementSignal<std::string>> ruleHeightInput_;
    // shared_ptr<ObjectEmitter<std::string, Beaker<unsigned char>>> objectEmitter_;
    // shared_ptr<ObjectAcceptor<std::string, Beaker<unsigned char>>> objectAcceptor_;

    shared_ptr<ObjectSignalLoop<std::string, Beaker<unsigned char>>> iterationIntervalLoop_;

    shared_ptr<ObjectSignalLoop<std::string, Beaker<unsigned char>>> ruleWidthLoop_;
    shared_ptr<ObjectSignalLoop<std::string, Beaker<unsigned char>>> ruleHeightLoop_;

    std::string name_;
    bool isReactionRule_ = false;  //!< Set to true if this Beaker is being used as a reaction rule
                                   //!< for an enclosing Beaker.

    int gridPixelWidth_ = 600;   //!< Width in pixels of beaker grid.
    int gridPixelHeight_ = 400;  //!< Height in pixels of beaker grid.

    int gridWidth_ = 60;   //!< Width of beaker grid in cells.
    int gridHeight_ = 40;  //!< Height of beaker grid in cells.

    int ruleGridWidth_ = 5;   //!< Width of new rule grid in cells.
    int ruleGridHeight_ = 3;  //!< Height of new rule grid in cells.

    // V *gridArray_;  //!< The actual grid data to be used by the CanvasGrid in BeakerNode.

    bool isPlaying_ = false;
    int iterationInterval_ = 10;
    val timerId_ = val::null();
    bool iterationLock_ = false;

    std::map<gridCoordinatePairT, std::vector<valuePriorityPairT>> successionMap_;

    int iterationCount_;  // = 0;
    // int iterationCountSave_;
    //!< Counter that advances every time the rules are applied to the grid.

    std::vector<shared_ptr<Beaker<V>>> reactionRules_;

    shared_ptr<Beaker> parentBeaker_;

    shared_ptr<Beaker<V>> successor_;  // = this;    //!< The pattern we replace this one with.
    std::string successorName_;
    int successorOffsetX_ = 0;          //!< X offset of replacement pattern.
    int successorOffsetY_ = 0;          //!< Y offset of replacement pattern.
    priorityT successorPriority_ = 10;  //!< Priority assigned to pixels replaced by application of
                                        //!< this pattern. Lower values take precedence.

    int ruleCount_ = 0;

    bool clean_ = true;  //!< We set this to false when the user modifies a rule or the main grid.

    val elgCallMethodOnObjByName_ = val::global("elgCallMethodOnObjByName");
    val iterateOnceJS_;

    val setInterval_ = val::global("setInterval");
    val clearInterval_ = val::global("clearInterval");

    std::vector<gridCoordinatePairT> matchLists_[4];
    std::map<V, std::vector<gridCoordinatePairT>> valueToCoordinateMaps_[4];

   public:
    // RotationMatrix2D<gridCoordinateT> *r0__, *r90__, *r180__,
    //     *r270__;  //!< I tried to make these static const class members because they're the same
    //     for
    //               //!< all Beakers but ran into huge problems getting link errors and a lot of
    //               //!< errors about forward template definitions. The small space and time
    //               savings
    //               //!< wasn't worth it.

    /**
     * @brief Adapted from this doc:
     * https://en.cppreference.com/w/cpp/memory/enable_shared_from_this
     *
     * @return std::shared_ptr<Beaker<V>>
     */
    std::shared_ptr<Beaker<V>> getptr() { return this->shared_from_this(); }

    // static Beaker<V> *singleton__;

    // static Beaker<V> *getSingleton() {
    //     // shared_ptr<SignalBuilder> sb = make_shared<cl2::SignalBuilder>();
    //     // auto b = make_shared<Beaker<V>>(sb, 90, 60, 900, 600, "JunkBeaker");
    //     // b->finalize();

    //     // if (singleton__ == nullptr) {
    //     //     singleton__ = new Beaker<V>();
    //     // }
    //     return singleton__;
    // }

    //  static void setSingleton(Beaker<V> *singleton) { singleton__ = singleton; }

    void deleteRule() { nameInput_.reset(); }

    /**
     * @brief Needs to be called outside the constructor due to use of getptr()
     *
     */
    void finalize() {
        if (isReactionRule_) {
            this->successor_ = getptr();

            nameInput_ = signalBuilder_->withAttributes({{"class", val("medium_width")}})
                             .textInputWSS<std::string>("nameInput", "Rule Name", false);

            nameInputLoop_ = make_shared<ObjectSignalLoop<std::string, Beaker<unsigned char>>>(
                getptr(), nameInput_, &Beaker<unsigned char>::setName,
                &Beaker<unsigned char>::getName);

            signalBuilder_->connectLoop(nameInputLoop_);

            successorNameInput_ =
                signalBuilder_->withAttributes({{"class", val("medium_width")}})
                    .textInputWSS<std::string>("successorNameInput", "Successor Rule Name", false);

            successorNameInputLoop_ =
                make_shared<ObjectSignalLoop<std::string, Beaker<unsigned char>>>(
                    getptr(), successorNameInput_, &Beaker<unsigned char>::setSuccessorName,
                    &Beaker<unsigned char>::getSuccessorName);

            signalBuilder_->connectLoop(successorNameInputLoop_);

            successorPriorityInput_ =
                signalBuilder_->withAttributes({{"class", val("small_width")}})
                    .textInputWSS<std::string>("successorPriorityInput", "Successor Priority",
                                               false);

            successorPriorityInputLoop_ =
                make_shared<ObjectSignalLoop<std::string, Beaker<unsigned char>>>(
                    getptr(), successorPriorityInput_, &Beaker<unsigned char>::setSuccessorPriority,
                    &Beaker<unsigned char>::getSuccessorPriority);

            signalBuilder_->connectLoop(successorPriorityInputLoop_);

            successorOffsetXInput_ =
                signalBuilder_->withAttributes({{"class", val("small_width")}})
                    .textInputWSS<std::string>("successorOffsetXInput", "SX", false);

            successorOffsetXInputLoop_ =
                make_shared<ObjectSignalLoop<std::string, Beaker<unsigned char>>>(
                    getptr(), successorOffsetXInput_, &Beaker<unsigned char>::setSuccessorOffsetX,
                    &Beaker<unsigned char>::getSuccessorOffsetX);

            signalBuilder_->connectLoop(successorOffsetXInputLoop_);

            successorOffsetYInput_ =
                signalBuilder_->withAttributes({{"class", val("small_width")}})
                    .textInputWSS<std::string>("successorOffsetYInput", "SY", false);

            successorOffsetYInputLoop_ =
                make_shared<ObjectSignalLoop<std::string, Beaker<unsigned char>>>(
                    getptr(), successorOffsetYInput_, &Beaker<unsigned char>::setSuccessorOffsetY,
                    &Beaker<unsigned char>::getSuccessorOffsetY);

            signalBuilder_->connectLoop(successorOffsetYInputLoop_);

            makeNewRuleAcceptor_ = make_shared<ObjectAcceptor<std::string, Beaker<V>>>(getptr());
            makeNewRuleAcceptor_->setSignalAcceptorMethod(&Beaker::deleteRuleSignal);
            deleteRuleButton_ = signalBuilder_->buttonWSS<std::string>("Delete this rule");
            signalBuilder_->connect<std::string>(deleteRuleButton_, makeNewRuleAcceptor_);

        } else {
            // Beaker<unsigned char>::singleton__ = &*this;
            gridControl_->installMousePositionCallback(createGCMousePositionCallback());

            makeNewRuleAcceptor_ = make_shared<ObjectAcceptor<std::string, Beaker<V>>>(getptr());
            makeNewRuleAcceptor_->setSignalAcceptorMethod(&Beaker::makeNewReactionRuleSignal);
            newRuleButton_ = signalBuilder_->buttonWSS<std::string>("New Rule WSS");
            signalBuilder_->connect<std::string>(newRuleButton_, makeNewRuleAcceptor_);

            makeDebugRulesAcceptor_ = make_shared<ObjectAcceptor<std::string, Beaker<V>>>(getptr());
            makeDebugRulesAcceptor_->setSignalAcceptorMethod(&Beaker::makeDebugRulesSignal);
            debugRulesButton_ = signalBuilder_->buttonWSS<std::string>("Debug Rule Set");
            signalBuilder_->connect<std::string>(debugRulesButton_, makeDebugRulesAcceptor_);

            iterateOnceAcceptor_ = make_shared<ObjectAcceptor<std::string, Beaker<V>>>(getptr());
            iterateOnceAcceptor_->setSignalAcceptorMethod(&Beaker::iterateOnceSignalTarget);
            iterateOnceButton_ = signalBuilder_->buttonWSS<std::string>("Iterate Once");
            signalBuilder_->connect<std::string>(iterateOnceButton_, iterateOnceAcceptor_);

            runAcceptor_ = make_shared<ObjectAcceptor<std::string, Beaker<V>>>(getptr());
            runAcceptor_->setSignalAcceptorMethod(&Beaker::runSignalMethod);
            runButton_ = signalBuilder_->buttonWSS<std::string>("Run");
            signalBuilder_->connect<std::string>(runButton_, runAcceptor_);

            clearAcceptor_ = make_shared<ObjectAcceptor<std::string, Beaker<V>>>(getptr());
            clearAcceptor_->setSignalAcceptorMethod(&Beaker::clearGrid);
            clearButton_ = signalBuilder_->buttonWSS<std::string>("Clear Grid");
            signalBuilder_->connect<std::string>(clearButton_, clearAcceptor_);

            loadButton_ = signalBuilder_->buttonWSS<std::string>("Load Rules");
            saveButton_ = signalBuilder_->buttonWSS<std::string>("Save Run");
            saveAcceptor_ = make_shared<ObjectAcceptor<std::string, Beaker<V>>>(getptr());
            saveAcceptor_->setSignalAcceptorMethod(&Beaker::serialize);

            loadAcceptor_ = make_shared<ObjectAcceptor<std::string, Beaker<V>>>(getptr());
            loadAcceptor_->setSignalAcceptorMethod(&Beaker::deserialize);
            signalBuilder_->connect<std::string>(saveButton_, saveAcceptor_);
            signalBuilder_->connect<std::string>(loadButton_, loadAcceptor_);

            iterationIntervalInput_ =
                signalBuilder_->withAttributes({{"class", val("small_width")}})
                    .textInputWSS<std::string>("iterationIntervalInput", "Iteration interval in ms",
                                               false);

            ruleWidthInput_ =
                signalBuilder_->withAttributes({{"class", val("small_width")}})
                    .textInputWSS<std::string>("ruleWidthInput", "Rule Width in pixels", false);

            ruleHeightInput_ =
                signalBuilder_->withAttributes({{"class", val("small_width")}})
                    .textInputWSS<std::string>("ruleHeightInput", "Rule Height in pixels", false);

            iterationIntervalLoop_ =
                make_shared<ObjectSignalLoop<std::string, Beaker<unsigned char>>>(
                    getptr(), iterationIntervalInput_, &Beaker<unsigned char>::setIterationInterval,
                    &Beaker<unsigned char>::getIterationInterval);

            ruleWidthLoop_ = make_shared<ObjectSignalLoop<std::string, Beaker<unsigned char>>>(
                getptr(), ruleWidthInput_, &Beaker<unsigned char>::setRuleGridWidth,
                &Beaker<unsigned char>::getRuleGridWidth);

            ruleHeightLoop_ = make_shared<ObjectSignalLoop<std::string, Beaker<unsigned char>>>(
                getptr(), ruleHeightInput_, &Beaker<unsigned char>::setRuleGridHeight,
                &Beaker<unsigned char>::getRuleGridHeight);

            signalBuilder_->connectLoop(ruleWidthLoop_);
            signalBuilder_->connectLoop(ruleHeightLoop_);
            signalBuilder_->connectLoop(iterationIntervalLoop_);
        }
    }

    void makeNewReactionRuleSignal(const std::string &s) {
        cout << "GOT SIGNAL FROM BUTTON: " << s << endl;
        this->makeNewReactionRule();
    }

    void makeDebugRulesSignal(const std::string &s) {
        cout << "CREATING DEBUG RULE SET: " << s << endl;
        this->makeDebugRuleSet();
    }

    void deleteRuleSignal(const std::string &s) {
        cout << "GOT DELETE SIGNAL FROM BUTTON: " << s << endl;
        // this->makeNewReactionRule();
        shared_ptr<Beaker<unsigned char>> thisRuleInParent = findRuleByName(name_);
        cout << "This rule name: " << name_ << " is found in parent as: " << thisRuleInParent->name_
             << endl;

        // thisRuleInParent->nameInput_->getWebElement()->deleteDomElement();

        if (isReactionRule_) {
            nameInput_.reset();
            nameInputLoop_.reset();
            successorNameInput_.reset();
            successorNameInputLoop_.reset();
            successorPriorityInput_.reset();
            successorPriorityInputLoop_.reset();
            successorOffsetXInput_.reset();
            successorOffsetXInputLoop_.reset();
            successorOffsetYInput_.reset();
            successorOffsetYInputLoop_.reset();
            makeNewRuleAcceptor_.reset();
            deleteRuleButton_.reset();
        } else {
            makeNewRuleAcceptor_.reset();
            newRuleButton_.reset();
            iterateOnceAcceptor_.reset();
            iterateOnceButton_.reset();
            clearAcceptor_.reset();
            clearButton_.reset();
            loadButton_.reset();
            saveButton_.reset();
            saveAcceptor_.reset();
            iterationIntervalInput_.reset();
            ruleWidthInput_.reset();
            ruleHeightInput_.reset();
            iterationIntervalLoop_.reset();
            ruleWidthLoop_.reset();
            ruleHeightLoop_.reset();
        }
        thisRuleInParent->beakerDiv_->deleteDomElement();
        // We need a way to destroy and remove elements!
    }

    // Beaker() {}

    Beaker(shared_ptr<cl2::SignalBuilder> signalBuilder, int gridWidth, int gridHeight,
           int gridPixelWidth, int gridPixelHeight, const std::string &name = "",
           bool isReactionRule = false)
        : signalBuilder_(signalBuilder),
          gridWidth_(gridWidth),
          gridHeight_(gridHeight),
          gridPixelWidth_(gridPixelWidth),
          gridPixelHeight_(gridPixelHeight),
          name_(name),
          isReactionRule_(isReactionRule) {
        // initStandardRotationMatrices();
        cout << "Beaker created!" << endl;
        iterationCount_ = 0;

        beakerDiv_ = make_shared<Div>(signalBuilder->div(name_));
        signalBuilder_ =
            make_shared<SignalBuilder>(signalBuilder_->withParentWebElement(*beakerDiv_));
        // SignalBuilder &sb = *signalBuilder_;
        gridControl_ =
            make_shared<GridControl<V>>(gridWidth_, gridHeight_, gridPixelWidth_, gridPixelHeight_,
                                        signalBuilder_, name_ + "_gc1");
        gridControl_->addColorToPallete(0, "#000000");
        gridControl_->addColorToPallete(1, "#ff0000");
        gridControl_->addColorToPallete(2, "#00ff00");
        gridControl_->addColorToPallete(3, "#0000ff");
        gridControl_->addColorToPallete(4, "#ff00ff");
        gridControl_->addColorToPallete(5, "#ffff00");
        gridControl_->addColorToPallete(6, "#00ffff");
        gridControl_->finalize();

        // Beaker<unsigned char>::singleton__ = this;
    }

    void printBeakerStats() {
        cout << "Name: " << name_ << endl;
        cout << "\t\tSuccessor Name: " << successorName_ << endl;
        cout << "\t\tSuccessor Priority: " << successorPriority_ << endl;
        cout << "\t\tSX: " << successorOffsetX_ << ", SY: " << successorOffsetY_ << endl;
    }

    void setIterationInterval(const std::string &v) {
        iterationInterval_ = std::stoi(v);
        cout << "SETTING iterationInterval_ to " << v << endl;
    }

    std::string getIterationInterval() {
        std::string outstr = std::to_string(iterationInterval_);
        cout << "GETTING iterationInterval_ as string value: '" << outstr << "'" << endl;
        return outstr;
    }

    void setRuleGridWidth(const std::string &v) {
        ruleGridWidth_ = std::stoi(v);
        cout << "SETTING ruleGridWidth_ to " << v << endl;
    }

    std::string getRuleGridWidth() {
        std::string rgwString = std::to_string(ruleGridWidth_);
        cout << "GETTING ruleGridWidth_ as string value: '" << rgwString << "'" << endl;
        return rgwString;
    }

    void setRuleGridHeight(const std::string &v) {
        ruleGridHeight_ = std::stoi(v);
        cout << "SETTING ruleGridHeight_ to " << v << endl;
    }

    std::string getRuleGridHeight() {
        std::string outstr = std::to_string(ruleGridHeight_);
        cout << "GETTING ruleGridHeight_ as string value: '" << outstr << "'" << endl;
        return outstr;
    }

    void setName(const std::string &v) {
        name_ = v;
        cout << "SETTING name to " << v << endl;
    }

    std::string getName() {
        cout << "GETTING name : '" << name_ << "'" << endl;
        return name_;
    }

    void setSuccessorName(const std::string &v) {
        successorName_ = v;
        successor_ = findRuleByName(successorName_);
        cout << "SETTING successor name to " << v << endl;
    }

    std::string getSuccessorName() {
        cout << "GETTING successor name : '" << successorName_ << "'" << endl;
        return successorName_;
    }

    void setSuccessorPriority(const std::string &v) {
        successorPriority_ = std::stoi(v);
        cout << "SETTING successor priority to " << v << endl;
    }

    std::string getSuccessorPriority() {
        std::string outstr = std::to_string(successorPriority_);
        cout << "GETTING successor priority : '" << outstr << "'" << endl;
        return outstr;
    }

    void setSuccessorOffsetX(const std::string &v) {
        successorOffsetX_ = std::stoi(v);
        cout << "SETTING successor offsetX to " << v << endl;
    }

    std::string getSuccessorOffsetX() {
        std::string outstr = std::to_string(successorOffsetX_);
        cout << "GETTING successor offsetX : '" << outstr << "'" << endl;
        return outstr;
    }

    void setSuccessorOffsetY(const std::string &v) {
        successorOffsetY_ = std::stoi(v);
        cout << "SETTING successor offsetY to " << v << endl;
    }

    std::string getSuccessorOffsetY() {
        std::string outstr = std::to_string(successorOffsetY_);
        cout << "GETTING successor offsetY : '" << outstr << "'" << endl;
        return outstr;
    }

    /**
     * @brief Creates a new smaller Beaker to serve as a reaction pattern to run in the main
     * grid.
     *
     */
    void makeNewReactionRule() {
        shared_ptr<Beaker<unsigned char>> reactionRule = make_shared<Beaker<unsigned char>>(
            this->signalBuilder_, this->ruleGridWidth_, this->ruleGridHeight_, 150, 100,
            "rule" + clto_str(++this->ruleCount_), true);
        reactionRule->parentBeaker_ = getptr();
        this->reactionRules_.push_back(reactionRule);
        reactionRule->finalize();

        // nameInput_ =
        //     signalBuilder_->withAttributes({{"class", val("medium_width")}})
        //         .textInputWSS<std::string>("nameInput", "Rule Name", false);

        // reactionRule->nameInputLoop_ = make_shared<ObjectSignalLoop<std::string, Beaker<unsigned
        // char>>>(
        //     reactionRule, reactionRule->nameInput_, &Beaker<unsigned char>::setName,
        //     &Beaker<unsigned char>::getName);

        // signalBuilder_->connectLoop(reactionRule->nameInputLoop_);
    }

    void makeDebugRuleSet() {
        this->setRuleGridHeight("3");
        this->setRuleGridWidth("3");
        this->makeNewReactionRule();
        this->makeNewReactionRule();
        this->setRuleGridHeight("5");
        this->setRuleGridWidth("5");
        this->makeNewReactionRule();

        reactionRules_[1]->setSuccessorName("rule3");
        reactionRules_[1]->setSuccessorOffsetX("-1");
        reactionRules_[1]->setSuccessorOffsetY("-1");
        reactionRules_[0]->setSuccessorName("rule2");
        reactionRules_[0]->gridControl_->setPixelAt(1, 1, 1);
        reactionRules_[1]->gridControl_->setPixelAt(0, 1, 1);
        reactionRules_[1]->gridControl_->setPixelAt(1, 0, 1);
        reactionRules_[1]->gridControl_->setPixelAt(1, 2, 1);
        reactionRules_[1]->gridControl_->setPixelAt(2, 1, 1);

        reactionRules_[2]->gridControl_->setPixelAt(0, 0, 1);
        reactionRules_[2]->gridControl_->setPixelAt(4, 0, 1);
        reactionRules_[2]->gridControl_->setPixelAt(0, 4, 1);
        reactionRules_[2]->gridControl_->setPixelAt(4, 4, 1);

        reactionRules_[2]->gridControl_->setPixelAt(1, 2, 1);
        reactionRules_[2]->gridControl_->setPixelAt(2, 2, 1);
        reactionRules_[2]->gridControl_->setPixelAt(3, 2, 1);
        reactionRules_[2]->gridControl_->setPixelAt(2, 1, 1);
        reactionRules_[2]->gridControl_->setPixelAt(2, 3, 1);
    }

    void wrapCoordinates(gridCoordinatePairT &p) {
        if (p.first < 0)
            p.first = gridWidth_ + p.first;
        else
            p.first = p.first % gridWidth_;
        if (p.second < 0)
            p.second = gridHeight_ + p.second;
        else
            p.second = p.second % gridHeight_;
        // if (p.first < 0 || p.second < 0) cout << "NEGATIVE COORDS!!!!!!!!!" << endl;
    }

    void printSuccessionStackAt(gridCoordinateT x, gridCoordinateT y) {
        // cout << "printSuccessionStackAt: " << x << ", " << y << endl;
        auto vpstack = successionMap_[std::make_pair(x, y)];
        for (auto vp : vpstack) {
            auto [val, priority] = vp;
            cout << "SS: " << x << ", " << y << ": val = " << int(val) << ", pri = " << priority
                 << endl;
        }
    }

    /**
     * @brief Creates the lambda function that is then passed to the GC.
     *
     * @return auto
     */
    auto createGCMousePositionCallback() {
        return [&](double x, double y) { this->printSuccessionStackAt(int(x), int(y)); };
    }

    bool matchAt(shared_ptr<Beaker<V>> reactionRule, gridCoordinateT x, gridCoordinateT y,
                 const RotationMatrix2D<gridCoordinateT> &rotationMatrix) {
        for (gridCoordinateT i = 0; i < reactionRule->gridWidth_; i++) {
            for (gridCoordinateT j = 0; j < reactionRule->gridHeight_; j++) {
                V ruleVal = reactionRule->gridControl_->getPixelAt(i, j);

                gridCoordinatePairT reactionRuleOffset = std::make_pair(i, j);
                reactionRuleOffset = rotationMatrix.rotateCoordinates(reactionRuleOffset);

                gridCoordinateT gx = x + reactionRuleOffset.first;
                gridCoordinateT gy = y + reactionRuleOffset.second;
                gridCoordinatePairT gp = std::make_pair(gx, gy);
                wrapCoordinates(gp);
                V gridVal = this->gridControl_->getPixelAt(gp.first, gp.second);
                // cout << i << ", " << j << ":: " << int(ruleVal) << "|| " << gp.first << ", "
                //      << gp.second << ":: " << int(gridVal) << endl;
                if (gridVal != ruleVal) return false;
            }
        }
        return true;
    }

    /**
     * @brief Meant as sort of a logging tag line so you can keep data relating to the same
     * iteration together.
     *
     */
    inline void printIterationCount() { cout << "ITER: " << iterationCount_; }

    void populateMatchLists() {
        if (isReactionRule_) return;

        auto newPixelMap = gridControl_->getNewPixelMap();
        gridControl_->clearNewPixelMap();

        printIterationCount();

        if (newPixelMap.empty()) {
            cout << ", There are NO NEW PIXELS!" << endl;
            return;
        }
        cout << ", Color stats: ";
        for (const auto &[anchorPixelColor, pixels] : newPixelMap) {
            cout << int(anchorPixelColor) << ":" << pixels.size() << ",";
            for (const auto &p : pixels) {
                // cout << "\t" << p.first << ": " << p.second << endl;
                for (auto reactionRule : reactionRules_) {
                    // cout << "\t\tRule name: " << reactionRule->name_ << endl;
                    if (reactionRule->successor_ == reactionRule ||
                        reactionRule->successor_ == nullptr)
                        continue;

                    for (int r = 0; r < 4; r++) {
                        const auto &rotationMatrix = rotationMatrices[r];
                        auto vcm = reactionRule->valueToCoordinateMaps_[r];
                        auto matchingPixels = vcm[anchorPixelColor];
                        std::set<gridCoordinatePairT> potentialMatchLocations;
                        for (auto [x, y] : matchingPixels) {
                            auto potentialMatchX = p.first - x;
                            auto potentialMatchY = p.second - y;

                            auto potentialMatchLoc =
                                std::make_pair(potentialMatchX, potentialMatchY);
                            // wrapCoordinates(potentialMatchLoc);
                            potentialMatchLocations.insert(potentialMatchLoc);
                        }
                        for (auto loc : potentialMatchLocations) {
                            auto [x, y] = loc;
                            if (matchAt(reactionRule, x, y, rotationMatrix)) {
                                reactionRule->matchLists_[r].push_back(loc);
                            }
                        }
                    }
                }
            }
        }

        cout << endl;
    }

    void debugMatchLists() {
        if (isReactionRule_) return;

        for (gridCoordinateT i = 0; i < gridWidth_; i++) {
            for (gridCoordinateT j = gridHeight_; j > 0; j--) {
                const auto &p = std::make_pair(i, j);
                // cout << "\t" << p.first << ": " << p.second << endl;

                for (auto reactionRule : reactionRules_) {
                    // cout << "\t\tRule name: " << reactionRule->name_ << endl;
                    if (reactionRule->successor_ == reactionRule ||
                        reactionRule->successor_ == nullptr)
                        continue;

                    for (int r = 0; r < 4; r++) {
                        const auto &rotationMatrix = rotationMatrices[r];
                        if (matchAt(reactionRule, p.first, p.second, rotationMatrix)) {
                            reactionRule->matchLists_[r].push_back(
                                std::make_pair(p.first, p.second));
                        }
                    }
                }
            }
        }
        // gridControl_->clearNewPixelMap();
        cout << endl;
    }

    void processMatchLists() {
        for (auto reactionRule : reactionRules_) {
            if (reactionRule->successor_ == reactionRule || reactionRule->successor_ == nullptr)
                continue;
            // cout << "Reaction rule '" << reactionRule->name_ << "'" << endl;
            // cout << "\tSuccessor rule '" << reactionRule->successorName_ << "'" << endl;

            for (int i = 0; i < 4; i++) {
                for (const auto &matchOffset : reactionRule->matchLists_[i]) {
                    auto x = matchOffset.first;
                    auto y = matchOffset.second;
                    // cout << "Rot: " << i * 90 << " - " << x << ", " << y << endl;
                    auto successorOffset = std::make_pair(reactionRule->successorOffsetX_,
                                                          reactionRule->successorOffsetY_);

                    addSuccessorPixels(*reactionRule->successor_, successorOffset, matchOffset, i,
                                       reactionRule->successorPriority_);
                }
            }
        }
    }

    void purgeMatchLists() {
        for (auto reactionRule : reactionRules_) {
            for (int i = 0; i < 4; i++) {
                reactionRule->matchLists_[i].clear();
            }
        }
    }

    /**
     * @brief Performs basic hygeine functions to maintain the lists that are used in matching and
     * iteration.
     *
     */
    void update() {
        if (iterationCount_ == 0) gridControl_->setRecordingOn();
        if (iterationCount_ > 0) gridControl_->makeFrame();
        successionMap_.clear();
        buildPixelValueMaps();
        // printPixelValueMaps();
        // for (auto reactionRule : reactionRules_) {
        //     reactionRule->printBeakerStats();
        // }
        // debugMatchLists();
        populateMatchLists();

        processMatchLists();
        updateGrid();
        purgeMatchLists();
    }

    shared_ptr<Beaker<unsigned char>> findRuleByName(const std::string &ruleName) {
        // auto nameIs = [&ruleName](shared_ptr<Beaker> b) { return (b->name_ == ruleName); };
        // auto it = find_if(parentBeaker_->reactionRules_.begin(),
        //                   parentBeaker_->reactionRules_.end(), nameIs);
        // if (it == reactionRules_.end()) {
        //     return nullptr;
        //     cout << "NO MATCHING RULE FOUND!!" << endl;
        // }
        // cout << "MATCH FOUND: " << *it->name_ << endl;
        // return *it;

        for (auto reactionRule : parentBeaker_->reactionRules_) {
            if (reactionRule->name_ == ruleName) return reactionRule;
        }
        return nullptr;
    }

    std::map<V, std::vector<gridCoordinatePairT>> buildPixelValueMap(
        const RotationMatrix2D<gridCoordinateT> &rotationMatrix) {
        std::map<V, std::vector<gridCoordinatePairT>> valueToCoordinateMap;

        for (gridCoordinateT i = 0; i < gridWidth_; i++) {
            for (gridCoordinateT j = 0; j < gridHeight_; j++) {
                V ruleVal = gridControl_->getPixelAt(i, j);

                gridCoordinatePairT reactionRuleOffset = std::make_pair(i, j);
                reactionRuleOffset = rotationMatrix.rotateCoordinates(reactionRuleOffset);
                valueToCoordinateMap[ruleVal].push_back(reactionRuleOffset);
            }
        }

        return valueToCoordinateMap;
    }

    void buildPixelValueMaps() {
        // cout << "buildPixelValueMaps()" << endl;
        for (auto reactionRule : reactionRules_) {
            for (int i = 0; i < 4; i++) {
                reactionRule->valueToCoordinateMaps_[i].clear();
                reactionRule->valueToCoordinateMaps_[i] =
                    reactionRule->buildPixelValueMap(rotationMatrices[i]);
            }
        }
    }

    void printPixelValueMaps() {
        for (auto reactionRule : reactionRules_) {
            for (int i = 0; i < 4; i++) {
                for (const auto &[v, coordinatePairs] : reactionRule->valueToCoordinateMaps_[i]) {
                    // if (v == 0) continue;
                    //  We aren't going to bother printing all the empty pixels.
                    cout << reactionRule->name_ << " @ " << i * 90 << " Pixel color = " << int(v)
                         << endl;
                    for (const auto &[x, y] : coordinatePairs) {
                        cout << "\t" << x << ", " << y << endl;
                    }
                }
            }
        }
    }

    // void addSuccessorPixel(gridCoordinatePairT matchOffset, gridCoordinatePairT p, V pixelVal,
    //                        priorityT pixelPriority, int rotationIndex) {
    //     const RotationMatrix2D<gridCoordinateT> &rm = rotationMatrices[rotationIndex];
    //     gridCoordinatePairT pPrime = rm.rotateCoordinates(p);
    //     gridCoordinatePairT spLocation =
    //         std::make_pair(matchOffset.first + pPrime.first, matchOffset.second + pPrime.second);
    //     successionMap_[spLocation] = std::make_pair(pixelVal, pixelPriority);
    // }

    void addSuccessorPixels(const Beaker<V> &successor, const gridCoordinatePairT &successorOffset,
                            const gridCoordinatePairT &matchOffset, const int rotationIndex,
                            const priorityT successorPriority) {
        for (auto i = 0; i < successor.gridWidth_; i++) {
            for (auto j = 0; j < successor.gridHeight_; j++) {
                auto successorCoords =
                    std::make_pair(i + successorOffset.first, j + successorOffset.second);

                auto successorPixelVal = successor.gridControl_->getPixelAt(i, j);

                // if (successorPixelVal == 0) continue;

                successorCoords =
                    rotationMatrices[rotationIndex].rotateCoordinates(successorCoords);
                successorCoords.first += matchOffset.first;
                successorCoords.second += matchOffset.second;

                wrapCoordinates(successorCoords);

                valuePriorityPairT vp = std::make_pair(successorPixelVal, successorPriority);
                // successionMap_[successorCoords]
                // cout << "SX: " << successorOffset.first << ", SY: " << successorOffset.second
                //      << ", X: " << successorCoords.first << ", Y: " << successorCoords.second
                //      << ", val: " << int(successorPixelVal) << ", pri: " << successorPriority
                //      << endl;

                successionMap_[successorCoords].push_back(vp);
            }
        }
        // auto successorPixel =
    }

    void serialize(const std::string &s) {
        cout << "Serializing reactor and reaction rules..." << endl;
        val putJSONIntoDomElement = val::global("putJSONIntoDomElement");
        
        ostringstream os = gridControl_->serializeFrames();
        os << endl << serializeBeaker() << endl;
        string frameString = os.str();
        putJSONIntoDomElement(val(frameString));
    }

    //string toJson(std::map<string,)

    string serializeBeaker() {
        ostringstream os;
        MapVal gridWidth  (gridWidth_);
        shared_ptr<string> gridWidthStr = make_shared<string>("gridWidth");
        MapVal gridWidthMV  (gridWidthStr);
        MapVal gridHeight  (gridHeight_);
        os << gridWidthMV.toJson() << ":" << gridWidth.toJson();
        return os.str();

    }

    void deserialize(const std::string &s) {
        cout << "Loading and de-serializing reactor and reaction rules..." << endl;
        val getJSONFromDomElement = val::global("getJSONFromDomElement");
        val json = getJSONFromDomElement();
        cout << json.as<std::string>() << endl;
    }

    //  static void iterateOnceST() { singleton__->iterateOnce(); }

    void iterateOnce() {
        if (iterationLock_) {
            cout << "ITERATION LOCKED!" << endl;
            return;
        }
        iterationLock_ = true;
        auto start = high_resolution_clock::now();
        this->update();
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        printIterationCount();
        cout << ", Upd time: " << duration.count() << " ms" << endl;
        iterationCount_++;
        // cout << "ITER: " << iterationCount_ << endl;
        iterationLock_ = false;
    }

    void iterateOnceSignalTarget(const std::string &s) { iterateOnce(); }

    /**
     * @brief Apply replacement rules to main grid one time.
     *
     */
    void runSignalMethod(const std::string &s) {
        if (!isPlaying_) {
            isPlaying_ = true;
            cout << "STARTING RUN AT ITERATION: " << iterationCount_ << endl;
            // iterateOnceJS_ = val::global("singletonIterationOnce");
            val makeIterateCallback = val::global("makeIterateCallback");
            iterateOnceJS_ = makeIterateCallback(this->getptr());

            timerId_ = setInterval_(iterateOnceJS_, val(iterationInterval_));
        } else {
            isPlaying_ = false;
            cout << "STOPPING RUN AT ITERATION: " << iterationCount_ << endl;
            clearInterval_(timerId_);
        }
    }

    void rectifySuccessionMap() {
        std::vector<gridCoordinatePairT> keysToDestroy;
        for (const auto &[key, value] : this->successionMap_) {
            auto [px, py] = key;
            auto keyPrime = key;
            wrapCoordinates(keyPrime);
            auto [pxPrime, pyPrime] = keyPrime;
            if (px != pxPrime || py != pyPrime) {
                cout << "WRAPPING! " << px << ", " << pxPrime << ", " << py << ", " << pyPrime;
                auto pixelsToBeWrapped = successionMap_[key];
                successionMap_[keyPrime].insert(successionMap_[keyPrime].end(),
                                                std::begin(pixelsToBeWrapped),
                                                std::end(pixelsToBeWrapped));
                //
                keysToDestroy.push_back(key);
                // successionMap_[successorCoords].push_back(vp);
            }
            std::vector<valuePriorityPairT> vpStack = successionMap_[keyPrime];
            cout << " || UPD: " << pxPrime << ", " << pyPrime << " has " << vpStack.size()
                 << " pixels." << endl;
        }
        for (auto key : keysToDestroy) {
            auto [px, py] = key;
            cout << "DESTROYING WRAPPED KEY: " << px << ", " << py << endl;
            successionMap_.erase(key);
        }
    }

    void updateGrid() {
        clean_ = false;
        // rectifySuccessionMap();
        for (const auto &[key, value] : this->successionMap_) {
            auto [px, py] = key;
            std::vector<valuePriorityPairT> vpStack = value;
            // cout << "UPD: " << px << ", " << py << " has " << vpStack.size() << " pixels." <<
            // endl;
            if (!vpStack.empty()) {
                sortValuePriorityStack(vpStack);
                auto [val, pri] = vpStack.back();
                gridControl_->setPixelAt(px, py, val);
                // this->beakerNode_->beakerCanvas_->setValXYNoDraw(px, py, val);
            }
        }
        // gridControl_->redraw();
    }

    void clearGrid(const std::string &s) {
        // cout << "clearGrid()" << endl;
        // this->beakerNode_->beakerCanvas_->clearGridToValue(0);

        cout << "Clearing the grid!" << endl;
        gridControl_->clearGridToValue();
    }

    static bool compareValuePriorityPairs(valuePriorityPairT vp1, valuePriorityPairT vp2) {
        return (vp1.second < vp2.second);
    }

    static bool compareValuePriorityPairsOnValue(valuePriorityPairT vp1, valuePriorityPairT vp2) {
        return (vp1.first > vp2.first);
    }

    void sortValuePriorityStack(std::vector<valuePriorityPairT> &vpStack) const {
        sort(vpStack.begin(), vpStack.end(), compareValuePriorityPairsOnValue);
        sort(vpStack.begin(), vpStack.end(), compareValuePriorityPairs);
        // sort(vpStack.begin(), vpStack.end(), compareValuePriorityPairsOnValue);
    }
};

// template <>
// Beaker<unsigned char>::singleton__ = nullptr;
// template <>
// Beaker<unsigned char> *Beaker<unsigned char>::singleton__ = nullptr;

template <>
const RotationMatrix2D<Beaker<unsigned char>::gridCoordinateT>
    Beaker<unsigned char>::rotationMatrices[4] = {r0, r90, r180, r270};

EMSCRIPTEN_BINDINGS(PixelReactor) {
    emscripten::class_<Beaker<unsigned char>>("Beaker")
        // .function("toggleClean", &Beaker<unsigned char>::toggleClean,
        //           emscripten::allow_raw_pointers())
        // // .function("clearGrid", &Beaker<unsigned char>::clearGrid,
        // // emscripten::allow_raw_pointers())
        // .function("makeDirty", &Beaker<unsigned char>::makeDirty,
        // emscripten::allow_raw_pointers())
        .smart_ptr<std::shared_ptr<Beaker<unsigned char>>>("Beaker")
        .function("runSignalMethod", &Beaker<unsigned char>::runSignalMethod,
                  emscripten::allow_raw_pointers())
        .function("iterateOnce", &Beaker<unsigned char>::iterateOnce,
                  emscripten::allow_raw_pointers())
        // .class_function("iterateOnceST", &Beaker<unsigned char>::iterateOnceST,
        //                 emscripten::allow_raw_pointers())
        // .function("getSingleton", &Beaker<unsigned char>::getSingleton,
        //           emscripten::allow_raw_pointers())
        .function("makeNewReactionRule", &Beaker<unsigned char>::makeNewReactionRule,
                  emscripten::allow_raw_pointers());

    emscripten::class_<SignalBuilder>("SignalBuilder");
    emscripten::class_<GridControl<int>>("GridControl");

    emscripten::register_vector<Beaker<unsigned char>::gridCoordinatesValueTripletT>(
        "std::vector<gridCoordinatesValueTripletT>");
}

/**
 * @brief This is really now just a wrapper around the main Beaker object.
 *
 */
struct PixelReactor {
    shared_ptr<Beaker<unsigned char>> mainBeaker_;
    shared_ptr<SignalBuilder> signalBuilder_;

    PixelReactor() {
        cout << "I'm a Pixelreactor. I need to be redone completely 9!" << endl;
        signalBuilder_ = make_shared<cl2::SignalBuilder>();
        mainBeaker_ =
            make_shared<Beaker<unsigned char>>(signalBuilder_, 90, 60, 900, 600, "Beaker");
        mainBeaker_->finalize();
        BR();
    }
};

#endif
