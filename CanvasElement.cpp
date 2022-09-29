#include "CanvasElement.hpp"

template <>
void CanvasGrid<unsigned char>::cppValToDomElement() {
    // this->setVal(val(*this->cppVal_));
    // ClarityNode::
    this->domElement_.set("cellDimensions", "cellDimensions");
}

// template <>
// void CanvasElement<float>::loadCppValFromString(string sval) {
//     *cppVal_ = stof(sval);
// }