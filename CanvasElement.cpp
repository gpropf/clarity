#include "CanvasElement.hpp"
#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"

// class CanvasElement : public ClarityNode {
//     friend class CLNodeFactory;

CanvasElement::CanvasElement(const string &name, const string &tag,
                             const CppType storedValueType,
                             bool useExistingDOMElement)
    : ClarityNode(name, tag, storedValueType, useExistingDOMElement) {}

inline void CanvasElement::setDrawFuntionName(const string &drawFuntionName) {
    drawFuntionName_ = drawFuntionName;
}

inline void CanvasElement::refreshView() {
    cle_.call<void>(drawFuntionName_.c_str());
}

//};

/**
 * @brief Grid representation of a 2D array of values using a CanvasElement as
 * it's base object. We obviously need some kind of direct "pull" method to
 * actively get the array data and map it into the canvas quickly.
 *
 */

CanvasGrid::CanvasGrid(const string &name, const string &tag, const CppType storedValueType,
           bool useExistingDOMElement)
    : CanvasElement(name, tag, storedValueType, useExistingDOMElement) {}

inline virtual void CanvasGrid::setVal(const val &inval) {
    clean_ = false;
    // Needs to be given pointer to array data and dataDimensionality info.
    // So this information should reside in inval.
}

val CanvasGrid::getVal() const {
    val domElement = cle_["domElement"];

    // Needs to read the internal state of the CG object and transfer it
    // back to the array.
    return val(NULL);  // FIXME
}

void CanvasGrid::drawGrid() const {
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

void CanvasGrid::initcg() { cout << "CG init called.\n"; }
