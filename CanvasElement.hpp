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

    ~CanvasElement() { cout << "DESTROYING CanvasElement with id: " << this->id_ << "\n"; }

    inline string cppValToString() const {
        if (this->cppVal_ == nullptr) return "CanvasElement NULLPTR";
        return clto_str(*(reinterpret_cast<V *>(this->cppVal_)));
    }

    inline virtual void doNothing() {
        cout << "This method exists so that Embind will create a type when this method in bound."
             << endl;
    }

    // inline virtual void finalize() {
    //     cout << "CanvasElement::finalize()\n";
    //     this->jsProxyNode_.set("clarityNode", this);
    //     val listenerGenerators = ClarityNode::JSProxyNode_["listenerGenerators"];
    //     ClarityNode::JSProxyNode_.call<void>("installEventListenersByTagAndType",
    //     this->jsProxyNode_,
    //                                          listenerGenerators);
    // }

    INLINE virtual void finalize() {
        // cout << "CanvasElement::finalize()\n";
        this->jsProxyNode_.set("clarityNode", this);
        // val Selectables = val::global("Selectables");
        val listenerGenerators = ClarityNode::JSProxyNode_["listenerGenerators"];
        ClarityNode::JSProxyNode_.call<void>("installEventListenersByTagAndType2",
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
    int gridWidth_, gridHeight_, pixelWidth_, pixelHeight_, cellWidth_, cellHeight_;

    V currentCellVal_ = 0;

    static const array<string, 8>
        colors;  //!< FIXME: We should not have the colors hardcoded here but this will do for now.

   public:
    inline void setCurrentCellVal(int v) { currentCellVal_ = v; }

    inline V *getPtr2CurrentCellVal() { return &currentCellVal_; }

    inline string cppValToString() const {
        if (this->cppVal_ == nullptr) return "CanvasGrid NULLPTR";
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
        ClarityNode::JSProxyNode_.call<void>("installEventListenersByTagAndType2",
                                             val("CanvasGrid"), this->jsProxyNode_,
                                             listenerGenerators);
    }

    INLINE virtual string getNodeTypeCode() { return string("CG"); }

    void initcg() {
        cout << "CG init called.\n";
        HybridNode<V>::cppVal_ = new V[gridWidth_ * gridHeight_];
        int cellCount = 0;
        for (int i = 0; i < gridWidth_; i++) {
            for (int j = 0; j < gridHeight_; j++) {
                int addr = gridWidth_ * j + i;
                *(HybridNode<V>::cppVal_ + addr) = currentCellVal_;
            }
            // cout << "\n";
            cellCount++;
        }
        drawGrid();
    }

    int setValXY(int x, int y) {
        int addr = (y * gridWidth_ + x) * sizeof(V);
        *(this->cppVal_ + addr) = currentCellVal_;
        drawGrid();
        return addr;
    }

    V getValXY(int x, int y) {
        int addr = (y * gridWidth_ + x) * sizeof(V);
        V xyVal = *(this->cppVal_ + addr);
        nodelog("Value at " + clto_str(x) + ", " + clto_str(y) + " is " + clto_str(xyVal));
        return xyVal;
    }

    void drawGrid() const {
        val ctx = this->domElement_.template call<val>("getContext", val("2d"));
        val domElement = this->jsProxyNode_["domElement"];
        domElement.set(
            "gridRef",
            const_cast<CanvasGrid *>(this));  // Very bizarre errors when trying to use domElement_.
        domElement.set("gw", gridWidth_);
        domElement.set("gh", gridHeight_);
        domElement.set("cw", cellWidth_);
        domElement.set("ch", cellHeight_);
        ctx.set("fillStyle", "blue");
        // int width = this->datum_->dataDimensionality_[0];
        // int height = this->datum_->dataDimensionality_[1];
        // cout << "pixelWidth_ = " << pixelWidth_ << "\n";
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

    ~CanvasGrid() { cout << "DESTROYING CanvasGrid with id: " << this->id_ << "\n"; }
};

}  // namespace clarity

#endif
