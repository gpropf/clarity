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

template <typename V>
class CanvasElement : public HybridNode<V> {
   public:
    CanvasElement(const string &name, const string &tag, bool useExistingDOMElement)
        : HybridNode<V>(name, tag, useExistingDOMElement) {}

    ~CanvasElement() { cout << "DESTROYING CanvasElement with id: " << this->id_ << "\n"; }

    inline string cppValToString() const {
        if (this->cppVal_ == nullptr) return "CanvasElement NULLPTR";
        return clto_str(*(reinterpret_cast<V *>(this->cppVal_)));
    }

    virtual void refreshDOMValueFromModel(){};

    inline void setDrawFuntionName(const string &drawFuntionName) { drawFuntionName_ = drawFuntionName; }

    inline void refreshView() { this->cle_.template call<void>(drawFuntionName_.c_str()); }

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
    // void setVal(const val &inval) {}
    // val getVal() const {}
    //  class CLNodeFactory;
    // friend class CLNodeFactory;

    val CLElement_ = val::global("CLElement");

   protected:
    int width_, height_;  //!< Width and Height in screen pixels.
    double scaleFactorH_ = 1.0;
    double scaleFactorV_ = 1.0;
    int gridWidth_, gridHeight_, pixelWidth_, pixelHeight_, cellWidth_, cellHeight_;

    V currentCellVal_ = 0;

    static const array<string, 8> colors;

   public:
    inline void setCurrentCellVal(int v) { currentCellVal_ = v; }

    inline V *getPtr2CurrentCellVal() { return &currentCellVal_; }

    inline string cppValToString() const {
        if (this->cppVal_ == nullptr) return "CanvasGrid NULLPTR";
        return clto_str(*(reinterpret_cast<V *>(this->cppVal_)));
    }

    inline virtual void setVal(const val &inval) {
        this->clean_ = false;
        // Needs to be given pointer to array data and dataDimensionality info.
        // So this information should reside in inval.
    }

    virtual void refreshDOMValueFromModel(){};

    val getVal() const {
        val domElement = this->cle_["domElement"];

        // Needs to read the internal state of the CG object and transfer it
        // back to the array.
        return val(NULL);  // FIXME
    }

    void initcg() {
        cout << "CG init called.\n";
        HybridNode<V>::cppVal_ = new V[gridWidth_ * gridHeight_];
        int cellCount = 0;
        for (int i = 0; i < gridWidth_; i++) {
            for (int j = 0; j < gridHeight_; j++) {
                int addr = gridWidth_ * j + i;
                *(HybridNode<V>::cppVal_ + addr) = 1;
            }
            // cout << "\n";
            cellCount++;
        }
        this->domElement_.template call<void>("addEventListener", val("click"), CLElement_["setGridLocToCurrentVal"]);
        // addJSEventListener("click", CLElement_["setGridLocToCurrentVal"] );
        drawGrid();
    }

    int setValXY(int x, int y) {
        int addr = (y * gridWidth_ + x) * sizeof(V);
        *(this->cppVal_ + addr) = currentCellVal_;
        drawGrid();
        return addr;
    }

    void drawGrid() const {
        val ctx = this->domElement_.template call<val>("getContext", val("2d"));
        val domElement = this->cle_["domElement"];
        domElement.set("gridRef",
                       const_cast<CanvasGrid *>(this));  // Very bizarre errors when trying to use domElement_.
        domElement.set("gw", gridWidth_);
        domElement.set("gh", gridHeight_);
        domElement.set("cw", cellWidth_);
        domElement.set("ch", cellHeight_);
        ctx.set("fillStyle", "blue");
        // int width = this->datum_->dataDimensionality_[0];
        // int height = this->datum_->dataDimensionality_[1];
        cout << "pixelWidth_ = " << pixelWidth_ << "\n";
        int cellCount = 0;
        for (int i = 0; i < gridWidth_; i++) {
            for (int j = 0; j < gridHeight_; j++) {
                int addr = gridWidth_ * j + i;
                V v = reinterpret_cast<V>(*(HybridNode<V>::cppVal_ + addr));

                ctx.set("fillStyle", colors[v]);

                ctx.call<void>("fillRect", val(i * cellWidth_), val(j * cellHeight_), val(cellWidth_),
                               val(cellHeight_));
            }
            cellCount++;
        }
    }

    CanvasGrid(const string &name, const string &tag, bool useExistingDOMElement)
        : CanvasElement<V>(name, tag, useExistingDOMElement) {}

    CanvasGrid(const string &name, const string &tag, bool useExistingDOMElement, int gridWidth, int gridHeight,
               int pixelWidth, int pixelHeight)
        : CanvasGrid(name, tag, useExistingDOMElement) {
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
