/**
 * @file CanvasElement.hpp
 * @author your name (you@domain.com)
 * @brief CanvasElement and CanvasGrid classes.
 * @version 1.0
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CanvasElement_hpp
#define CanvasElement_hpp

#include "ClarityNode.hpp"

namespace clarity {

/**
 * @brief Simple canvas element for drawing on. Might add some extras to draw sprites or something
 * later.
 *
 * @tparam V
 */
template <typename V>
class CanvasElement : public HybridNode<V> {
   public:
    CanvasElement(const string &name, const string &tag, bool useExistingDOMElement,
                  ClarityNode::AttachmentMode attachmentMode, const string &attachmentId = "")
        : HybridNode<V>(name, tag, useExistingDOMElement, attachmentMode, attachmentId) {}

    virtual ~CanvasElement() { this->nodelog("DESTROYING CanvasElement"); }

    inline string cppValToString() const {
        if (this->cppVal_ == nullptr) return "CanvasElement nullptr";
        return clto_str(*(reinterpret_cast<V *>(this->cppVal_)));
    }

    inline virtual void doNothing() {
        cout << "This method exists so that Embind will create a type when this method in bound."
             << endl;
    }

    inline val getContext2d() {
        return this->getDomElement().template call<val>("getContext", val("2d"));
    }

    // inline void setDrawFunction(val drawFunction) {
    //     this->drawFunction_ = drawFunction;
    // }

    // inline void runDrawFunction() {
    //     this->drawFunction_(this);
    // }

    INLINE virtual void finalize() {
        this->jsProxyNode_.set("clarityNode", this);
        val listenerGenerators = ClarityNode::JSProxyNode_["listenerGenerators"];
        ClarityNode::JSProxyNode_.call<void>("installEventListenersByTagAndType",
                                             val("CanvasElement"), this->jsProxyNode_,
                                             listenerGenerators);
    }

    virtual void refreshDOMValueFromModel(){};  // FIXME: should probably move the initcg or
                                                // testfunction code in here.

    inline void setDrawFuntionName(const string &drawFuntionName) {
        drawFuntionName_ = drawFuntionName;
    }

    inline void refreshView() { this->jsProxyNode_.template call<void>(drawFuntionName_.c_str()); }

   protected:
    int width_, height_;
    val drawFunction_;
    string drawFuntionName_;
};

/**
 * @brief Grid representation of a 2D array of values using a CanvasElement as
 * it's base object. We obviously need some kind of direct "pull" method to
 * actively get the array data and map it into the canvas quickly.
 *
 */
template <typename V>
class CanvasGrid : public CanvasElement<V> {
   protected:
    int width_, height_;  //!< Width and Height in screen pixels.
    double scaleFactorH_ = 1.0;
    double scaleFactorV_ = 1.0;
    int currentX_, currentY_;

    int gridWidth_, gridHeight_, pixelWidth_, pixelHeight_, cellWidth_, cellHeight_;

    V currentCellVal_ = 0;

    static const array<string, 8>
        colors;  //!< FIXME: We should not have the colors hardcoded here but this will do for now.

   public:
    vector<tuple<int, int, V>> pixelBuffer_;

    inline void setCurrentCellVal(int v) { currentCellVal_ = v; }

    inline V *getPtr2CurrentCellVal() { return &currentCellVal_; }

    inline string cppValToString() const {
        if (this->cppVal_ == nullptr) return "CanvasGrid nullptr";
        return clto_str(*(reinterpret_cast<V *>(this->cppVal_)));
    }

    inline virtual void setDOMVal(const val &inval) {
        this->clean_ = false;
        // Needs to be given pointer to array data and dataDimensionality info.
        // So this information should reside in inval.
    }

    // inline virtual void finalize() {
    //     cout << "CG: virtual void finalize()\n";
    // }

    virtual void refreshDOMValueFromModel(){};  // FIXME: should probably move the initcg or
                                                // testfunction code in here.

    val getVal() const {
        val domElement = this->jsProxyNode_["domElement"];

        // Needs to read the internal state of the CG object and transfer it
        // back to the array.
        return val(NULL);  // FIXME
    }

    inline virtual void doNothing() {
        cout << "This method exists so that Embind will create a type when this method in bound."
             << endl;
    }

    // inline virtual void finalize() {
    //     cout << "CanvasGrid::finalize()\n";
    //     this->jsProxyNode_.set("clarityNode", this);
    //     val listenerGenerators = ClarityNode::JSProxyNode_["listenerGenerators"];
    //     ClarityNode::JSProxyNode_.call<void>("installEventListenersByTagAndType",
    //     this->jsProxyNode_,
    //                                          listenerGenerators);
    // }

    INLINE virtual void finalize() {
        // cout << "CanvasGrid::finalize()\n";
        this->jsProxyNode_.set("clarityNode", this);
        // val Selectables = val::global("Selectables");
        val listenerGenerators = ClarityNode::JSProxyNode_["listenerGenerators"];
        ClarityNode::JSProxyNode_.call<void>("installEventListenersByTagAndType", val("CanvasGrid"),
                                             this->jsProxyNode_, listenerGenerators);
    }

    INLINE virtual string getNodeTypeCode() { return string("CG"); }

    void initcg() {
        cout << "CG init called.\n";
        HybridNode<V>::cppVal_ = new V[gridWidth_ * gridHeight_];
        // int cellCount = 0;
        for (int i = 0; i < gridWidth_; i++) {
            for (int j = 0; j < gridHeight_; j++) {
                // int addr = gridWidth_ * j + i;
                // *(HybridNode<V>::cppVal_ + addr) = currentCellVal_;
                setValXYNoDraw(i, j, currentCellVal_);
            }
            // cout << "\n";
            // cellCount++;
        }
        drawGrid();
    }

    void clearGridToValue(V v, bool redraw = true) {
        int gridArrayLength = gridWidth_ * gridHeight_;
        while (gridArrayLength--) {
            this->cppVal_[gridArrayLength] = v;
        }
        if (redraw) drawGrid();
    }

    int calculateGridCellAddress(int x, int y) const {
        int addr = (y * gridWidth_ + x) * sizeof(V);
        assert(addr >= 0);
        return addr;
    }

    /**
     * @brief Set the pixel at x,y to the value cellVal. Also pushes the pixel onto the pixelBuffer_
     * stack so it can be easily seen by the canvas owner. This method is also called by
     * `setValXYNoDraw()`.
     *
     * @param x
     * @param y
     * @param cellVal
     */
    inline void setValXYNoDraw(int x, int y, V cellVal) {
        pixelBuffer_.push_back(make_tuple(x, y, cellVal));
        //int addr = (y * gridWidth_ + x) * sizeof(V);
         int addr = calculateGridCellAddress(x,y);
         
        *(this->cppVal_ + addr) = cellVal;
    }

    inline void setValXY(int x, int y) {
        setCurrentXY(x, y);
        setValXYNoDraw(x, y, currentCellVal_);
        drawGrid();
        // V checkVal = getValXY(x, y);
        // this->nodelog("Value at " + clto_str(x) + "," + clto_str(y) + " is " +
        //               clto_str(int(checkVal)));
        // return addr;
    }

    tuple<int, int, V> getLatestPixel() { return pixelBuffer_.back(); }

    void flushPixelBuffer() { pixelBuffer_.clear(); }

    inline void setCurrentXY(int x, int y) {
        currentX_ = x;
        currentY_ = y;
    }

    V getValXY(int x, int y) {
      // int addr = (y * gridWidth_ + x) * sizeof(V);
      int addr = calculateGridCellAddress(x,y);

        V xyVal = *(this->cppVal_ + addr);
        // nodelog("Value at " + clto_str(x) + ", " + clto_str(y) + " is " + clto_str(xyVal));
        return xyVal;
    }

    /**
     * @brief Wrap x,y coordinates in case they go outside the grid. Values are modified in place.
     * DEPRECATED! Do not use!
     *
     * @param x
     * @param y
     */
    // void wrapCoordiates(int &x, int &y) {
    //     x = x % this->gridWidth_;
    //     y = y % this->gridHeight_;
    // }

    /**
     * @brief Better version of mod-in-place method above.
     *
     * @param x
     * @param y
     * @return pair<int,int>
     */
    pair<int, int> wrapCoordiates(int x, int y) {
        int xp = x % this->gridWidth_;
        int yp = y % this->gridHeight_;
        if (xp < 0) xp += gridWidth_;
        if (yp < 0) yp += gridHeight_;
        assert(xp >= 0);
        assert(yp >= 0);
        return pair(xp, yp);
    }

    void drawGrid() const {
        val ctx = this->domElement_.template call<val>("getContext", val("2d"));
        val domElement = this->jsProxyNode_["domElement"];
        domElement.set(
            "gridRef",
            const_cast<CanvasGrid *>(this));  // Very bizarre errors when trying to use domElement_.

        // These values are set up here for the JS event handlers to use.
        domElement.set("gw", gridWidth_);
        domElement.set("gh", gridHeight_);
        domElement.set("cw", cellWidth_);
        domElement.set("ch", cellHeight_);

        // Some code from the Matchsticks app to show how to do the fine divider lines.
        // ------------------------------------------------
        // ctx.call<void>("beginPath");
        // ctx.call<void>("moveTo", val(fromPoint_.first), val(fromPoint_.second));
        // ctx.call<void>("lineTo", val(toPoint_.first), val(toPoint_.second));

        // string colorStr = stickColor_.toString();
        // cout << "Color for line is " << colorStr << endl;
        // ctx.set("strokeStyle", colorStr);
        // ctx.set("lineWidth", lineWidth_);
        // ctx.set("lineCap", "round");
        // ctx.call<void>("stroke");
        // ------------------------------------------------

        int cellCount = 0;
        for (int i = 0; i < gridWidth_; i++) {
            for (int j = 0; j < gridHeight_; j++) {
                int addr = gridWidth_ * j + i;
                V v = reinterpret_cast<V>(*(HybridNode<V>::cppVal_ + addr));

                ctx.set("fillStyle", colors[v]);

                ctx.call<void>("fillRect", val(i * cellWidth_), val(j * cellHeight_),
                               val(cellWidth_), val(cellHeight_));
            }
            cellCount++;
        }
    }

    CanvasGrid(const string &name, const string &tag, bool useExistingDOMElement,
               ClarityNode::AttachmentMode attachmentMode, const string &domId = "")
        : CanvasElement<V>(name, tag, useExistingDOMElement, attachmentMode, domId) {}

    CanvasGrid(const string &name, const string &tag, bool useExistingDOMElement,
               ClarityNode::AttachmentMode attachmentMode, int gridWidth, int gridHeight,
               int pixelWidth, int pixelHeight, const string &domId = "")
        : CanvasGrid(name, tag, useExistingDOMElement, attachmentMode, domId) {
        gridWidth_ = gridWidth;
        gridHeight_ = gridHeight;
        pixelWidth_ = pixelWidth;
        pixelHeight_ = pixelHeight;
        cellWidth_ = pixelWidth_ / gridWidth_;
        cellHeight_ = pixelHeight_ / gridHeight_;
    }

    virtual ~CanvasGrid() { this->nodelog("DESTROYING CanvasGrid"); }
};

}  // namespace clarity

#endif
