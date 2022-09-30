#ifndef CanvasElement_hpp
#define CanvasElement_hpp

#include "ClarityNode.hpp"

namespace clarity {

class CanvasElement : public ClarityNode {
   public:
    CanvasElement(const string &name, const string &tag,
                  const CppType storedValueType, bool useExistingDOMElement)
        : ClarityNode(name, tag, storedValueType, useExistingDOMElement) {}

    inline void setDrawFuntionName(const string &drawFuntionName) {
        drawFuntionName_ = drawFuntionName;
    }

    inline void refreshView() { cle_.call<void>(drawFuntionName_.c_str()); }

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
class CanvasGrid : public CanvasElement {
    // void setVal(const val &inval) {}
    // val getVal() const {}

    static const array<string, 8> colors;

    CanvasGrid(const string &name, const string &tag,
               const CppType storedValueType, bool useExistingDOMElement)
        : CanvasElement(name, tag, storedValueType, useExistingDOMElement) {}

    inline virtual void setVal(const val &inval) {
        clean_ = false;
        // Needs to be given pointer to array data and dataDimensionality info.
        // So this information should reside in inval.
    }

    virtual val getVal() const {
        val domElement = cle_["domElement"];

        // Needs to read the internal state of the CG object and transfer it
        // back to the array.
        return val(NULL);  // FIXME
    }

   protected:
    int width_, height_;  //!< Width and Height in screen pixels.
    double scaleFactorH_ = 1.0;
    double scaleFactorV_ = 1.0;
    int gridWidth_, gridHeight_, pixelWidth_, pixelHeight_, cellWidth_,
        cellHeight_;
    unsigned char *dataptr_;

    void init() {
        cout << "CG init called.\n";
    }

    virtual void drawGrid() const {
        val ctx = this->domElement_.template call<val>("getContext", val("2d"));
        ctx.set("fillStyle", "blue");
        // int width = this->datum_->dataDimensionality_[0];
        // int height = this->datum_->dataDimensionality_[1];
        cout << "pixelWidth_ = " << pixelWidth_ << "\n";
        int cellCount = 0;
        for (int i = 0; i < gridWidth_; i++) {
            for (int j = 0; j < gridHeight_; j++) {
                int addr = gridWidth_ * j + i;
                unsigned char v =
                    reinterpret_cast<unsigned char>(*(dataptr_ + addr));

                ctx.set("fillStyle", colors[v]);

                ctx.call<void>("fillRect", val(i * cellWidth_),
                               val(j * cellHeight_), val(cellWidth_),
                               val(cellHeight_));
            }
            // cout << "\n";
            cellCount++;
        }
    }
};

}  // namespace clarity

#endif
