#ifndef MATCHSTICKS_hpp
#define MATCHSTICKS_hpp

#include "CLNodeFactory.hpp"
#include "CanvasElement.hpp"
#include "ClarityNode.hpp"
#include "clarity.hpp"
#include "globals.hpp"

using namespace clarity;

typedef pair<double, double> coordinatePair;
typedef double coordinate;

struct ColorRGBA {
    unsigned char r_, g_, b_;
    double a_;
    ColorRGBA(unsigned char r, unsigned char g, unsigned char b, double a)
        : r_(r), g_(g), b_(b), a_(a) {}
    string toString() {
        return "rgba(" + clto_str(int(r_)) + "," + clto_str(int(g_)) + "," + clto_str(int(b_)) +
               "," + clto_str(a_) + ")";
    }

    ColorRGBA(string sextuplet) {
        // int r,g,b;
        string rhex = "0x" + sextuplet.substr(0, 2);
        string ghex = "0x" + sextuplet.substr(2, 2);
        string bhex = "0x" + sextuplet.substr(4, 2);
        r_ = stoi(rhex, 0, 16);
        g_ = stoi(ghex, 0, 16);
        b_ = stoi(bhex, 0, 16);
        a_ = 1.0;
    }
};

struct Matchstick {
    coordinatePair fromPoint_, toPoint_;
    ColorRGBA stickColor_;  //, toCol_;
    double length_;
    double angle_;
    int lineWidth_;

    Matchstick(coordinatePair fromPoint, coordinatePair toPoint, ColorRGBA stickColor,
               int lineWidth)
        : fromPoint_(fromPoint),
          toPoint_(toPoint),
          stickColor_(stickColor),
          lineWidth_(lineWidth) {}

    static Matchstick makeRandomStick(int xRange, int yRange, coordinate length,
                                      ColorRGBA &stickColor, int stickWidth = 1) {
        coordinate x = rand() % xRange;
        coordinate y = rand() % yRange;
        double angle = (rand() % 360) / (2 * M_PI);
        coordinate xto = sin(angle) * length + x;
        coordinate yto = cos(angle) * length + y;
        coordinatePair fromPoint = pair(x, y);
        coordinatePair toPoint = pair(xto, yto);
        ColorRGBA blueGreen(50, 200, 220, 1);
        // if (rand() % 10 == 0)
        // return Matchstick(fromPoint, toPoint, blueGreen, 4);
        // else
        return Matchstick(fromPoint, toPoint, stickColor, stickWidth);
    }

    void draw(val ctx) {
        ctx.call<void>("beginPath");
        ctx.call<void>("moveTo", val(fromPoint_.first), val(fromPoint_.second));
        ctx.call<void>("lineTo", val(toPoint_.first), val(toPoint_.second));
        // string colorStr = "rgba(" + clto_str(int(stickColor_.r_)) + "," +
        //                   clto_str(int(stickColor_.g_)) + "," + clto_str(int(stickColor_.b_)) +
        //                   "," + clto_str(int(stickColor_.a_)) + ")";
        string colorStr = stickColor_.toString();
        cout << "Color for line is " << colorStr << endl;
        ctx.set("strokeStyle", colorStr);
        ctx.set("lineWidth", lineWidth_);
        ctx.set("lineCap", "round");
        ctx.call<void>("stroke");
    }
};

/**
 * @brief This is the complex control that allows the user to edit and run the StickWorld. Probably
 * a good thing to copy if you're planning to make your own complex controls.
 *
 * @tparam SW This 'SW' is actually a Stickworld object. So the `cppVal_` member of this
 * kind of node points to a StickWorld.
 */
template <class SW>
class StickWorldNode : public HybridNode<SW> {
   private:
    // ClarityNode *iterationCounter_;

   public:
    StickWorldNode(const string &name, const string &tag, bool useExistingDOMElement,
                   ClarityNode::AttachmentMode attachmentMode = ClarityNode::AttachmentMode::NEW,
                   const string &attachmentId = "")
        : HybridNode<SW>(name, tag, useExistingDOMElement, attachmentMode, attachmentId) {}

    /**
     * @brief In any node representing a complex object a call to HybridNode::refresh() is mandatory
     * as that is the only way child nodes representing values within the complex structure can be
     * updated when the object state changes.
     *
     */
    inline virtual void refresh() { HybridNode<SW>::refresh(); }

    inline void iterate() {
        if (this->cppVal_->playMode_ == 0) {
            if (this->playPauseButton_ != nullptr) this->playPauseButton_->setDOMVal(val("Play"));
            return;
        } else {
            if (this->playPauseButton_ != nullptr) this->playPauseButton_->setDOMVal(val("Pause"));
        }

        cout << "iterate()" << endl;
        this->cppVal_->iterationCount_++;
        int i = this->cppVal_->iterationCount_;

        // ColorRGBA stickColor(230, 55, 100, 1);
        ColorRGBA stickColor(this->cppVal_->lineColorString_);
        Matchstick m = Matchstick::makeRandomStick(this->cppVal_->swCanvasWidth_,
                                                   this->cppVal_->swCanvasHeight_,
                                                   this->cppVal_->lineLength_, stickColor, this->cppVal_->lineWidth_);
        val ctx = swCanvas_->getContext2d();
        m.draw(ctx);
    }

    inline CanvasElement<int> *getSWCanvas() { return this->swCanvas_; }

    /**
     * @brief This overload of `finalize()` shows that in the case of complex data types with many
     * subcomponents the finalize method is reponsible for setting up the nodes responsible for
     * manipulating them. This is also where essential things like the pointer in the model object
     * that points back to the GUI node are set up. This pointer is what allows the model object to
     * automatically update the interface when its internal state changes.
     *
     */
    virtual void finalize() {
        this->nodelog("StickWorldNode::finalize(): ");

        this->jsProxyNode_.set("clarityNode", this);

        CLNodeFactory<HybridNode, string, int> builder("div", "stickworldDiv");
        CLNodeFactory<HybridNode, string, int> stringBuilder(builder.withChildrenOf(this));
        CLNodeFactory<HybridNode, int, int> intBuilder(builder.withChildrenOf(this));
        CLNodeFactory<HybridNode, double, double> doubleBuilder(builder.withChildrenOf(this));

        swCanvas_ = intBuilder.withName("canvas1")
                        .withTag("canvas")
                        .withAttributes({{"style", val("border: 1px solid green")},
                                         {"width", val(this->cppVal_->swCanvasWidth_)},
                                         {"height", val(this->cppVal_->swCanvasHeight_)}})
                        .canvasElement();

        stringBuilder.br();

        context2d_ = swCanvas_->getContext2d();

        auto *lineLength_tinp = doubleBuilder.withName("lineLength_tinp")
                                    .withLabelText("Line Length")
                                    .withHoverText("Line Length")
                                    .withClass("small_width")
                                    .withCppVal(&this->cppVal_->lineLength_)
                                    .textInput();

        stringBuilder.labelGivenNode(lineLength_tinp, "Line Length");


        auto *lineWidthTextInput_ = intBuilder.withName("lineWidthTextInput_")
                                        .withLabelText("Line width (integer)")
                                        .withHoverText("Line width (integer)")
                                        .withClass("medium_width")
                                        .withCppVal(&this->cppVal_->lineWidth_)
                                        .textInput();

        stringBuilder.labelGivenNode(lineWidthTextInput_, "Line width (integer)");

        auto *lineColorTextInput_ = stringBuilder.withName("lineColorTextInput_")
                                        .withLabelText("Line color string")
                                        .withHoverText("Line color string")
                                        .withClass("medium_width")
                                        .withCppVal(&this->cppVal_->lineColorString_)
                                        .textInput();

        stringBuilder.labelGivenNode(lineColorTextInput_, "Line Color String");

        val ctx = swCanvas_->getContext2d();

        ctx.set("fillStyle", val("#eeeeee"));
        ctx.call<void>("fillRect", val(0), val(0), val(this->cppVal_->getSWCanvasWidth()),
                       val(this->cppVal_->getSWCanvasHeight()));

        val playPauseClassMap = ClarityNode::JSProxyNode_["playPauseClassMap"];
        NumWrapper<int> toggleWrapper(&this->cppVal_->playMode_, 2);
        this->playPauseButton_ =
            stringBuilder.withName("playPauseButton_")
                .withClass("small_width")
                .cycleButton<NumWrapper<int>>("--", toggleWrapper, playPauseClassMap);

        val Util = val::global("Util");
        val timerId = Util["setIntervalForObjectWithNamedMethod"](*this, val("iterate"), 75);
        cout << "Timer id: " << timerId.as<int>() << endl;
    }

   public:
    // FIXME: Would be nice to keep these members protected or private but
    // this is an easy way to let StickWorld see it for now.

    ClarityNode *reactionRulesDiv_;
    CanvasElement<int> *swCanvas_;
    ClarityNode *stickworldName_tinp;
    HybridNode<string> *playPauseButton_;
    val context2d_;
};

class StickWorld {
   public:
    StickWorld(int swCanvasWidth, int swCanvasHeight)
        : swCanvasWidth_(swCanvasWidth), swCanvasHeight_(swCanvasHeight) {}

    inline int getSWCanvasWidth() { return this->swCanvasWidth_; }
    inline int getSWCanvasHeight() { return this->swCanvasHeight_; }

   protected:
    int swCanvasWidth_ = 600;   //!< Width in pixels of stickWorld canvas.
    int swCanvasHeight_ = 400;  //!< Height in pixels of stickWorld canvas.

    int iterationCount_ = 0;
    int playMode_ = 0;

    double lineLength_ = 50;
    int lineWidth_ = 1;
    string lineColorString_ = "f045a0";

    StickWorldNode<StickWorld>
        *stickWorldNode_;  //!< Pointer back to containing SWN so that SWN->refresh()
                           //!< can be called when this updates.

    friend class StickWorldNode<StickWorld>;
};

EMSCRIPTEN_BINDINGS(Matchsticks) {
    class_<StickWorldNode<StickWorld>>("StickWorldNode")
        .function("iterate", &StickWorldNode<StickWorld>::iterate, allow_raw_pointers());
    class_<StickWorld>("StickWorld");
    class_<CanvasElement<int>>("CanvasElement_i");
}

/**
 * @brief The Matchsticks app from the old ClojureScript site redone in C++.
 *
 */
struct Matchsticks : public PageContent {
    ClarityNode *content(ClarityNode *innerContent = nullptr) {
        CLNodeFactory<HybridNode, double, double> builder("div", "maindiv");

#ifdef USETF
        auto *maindiv = builder.build();
#else
        auto *maindiv = builder.withAttachmentId("hookid")
                            .withAttachmentMode(clarity::ClarityNode::AttachmentMode::REPLACE_ID)
                            .build();
#endif

        CLNodeFactory<StickWorldNode, StickWorld, int> stickworldBuilder(
            builder.withChildrenOf(maindiv));

        StickWorld *stickWorld = new StickWorld(600, 400);

        StickWorldNode<StickWorld> *stickWorldNode =
            stickworldBuilder.withTag("div").withName("stickWorld").withCppVal(stickWorld).build();

        cout << "Setup complete!" << endl;
        return maindiv;
    }
};

#endif
