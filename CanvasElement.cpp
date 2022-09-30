#include "CanvasElement.hpp"

#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"

val CLElement_ = val::global("CLElement");

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

/**
 * @brief Grid representation of a 2D array of values using a CanvasElement as
 * it's base object. We obviously need some kind of direct "pull" method to
 * actively get the array data and map it into the canvas quickly.
 *
 */

CanvasGrid::CanvasGrid(const string &name, const string &tag,
                       const CppType storedValueType,
                       bool useExistingDOMElement)
    : CanvasElement(name, tag, storedValueType, useExistingDOMElement) {}

inline void CanvasGrid::setVal(const val &inval) {
    clean_ = false;
    // Needs to be given pointer to array data and dataDimensionality info.
    // So this information should reside in inval.
}

CanvasGrid::CanvasGrid(const string &name, const string &tag,
                       const CppType storedValueType,
                       bool useExistingDOMElement, int gridWidth,
                       int gridHeight, int pixelWidth, int pixelHeight)
    : CanvasGrid(name, tag, storedValueType, useExistingDOMElement) {
    gridWidth_ = gridWidth;
    gridHeight_ = gridHeight;
    pixelWidth_ = pixelWidth;
    pixelHeight_ = pixelHeight;
    cellWidth_ = pixelWidth_ / gridWidth_;
    cellHeight_ = pixelHeight_ / gridHeight_;
}

val CanvasGrid::getVal() const {
    val domElement = cle_["domElement"];

    // Needs to read the internal state of the CG object and transfer it
    // back to the array.
    return val(NULL);  // FIXME
}

void CanvasGrid::drawGrid() const {
    val ctx = this->domElement_.template call<val>("getContext", val("2d"));
    val domElement = cle_["domElement"];
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

int CanvasGrid::setValXY(int x, int y) {
    int addr = (y * gridWidth_ + x) * sizeof(unsigned char);
    *(this->dataptr_ + addr) = currentCellVal_;
    drawGrid();
    return addr;
}

void CanvasGrid::initcg() {
    cout << "CG init called.\n";
    dataptr_ = new unsigned char[gridWidth_ * gridHeight_];
    int cellCount = 0;
    for (int i = 0; i < gridWidth_; i++) {
        for (int j = 0; j < gridHeight_; j++) {
            int addr = gridWidth_ * j + i;
            *(dataptr_ + addr) = 1;
        }
        // cout << "\n";
        cellCount++;
    }
    domElement_.call<void>("addEventListener", val("click"),
                           CLElement_["setGridLocToCurrentVal"]);
    // addJSEventListener("click", CLElement_["setGridLocToCurrentVal"] );
    drawGrid();
}
