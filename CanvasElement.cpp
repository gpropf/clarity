//#include "clarity.hpp"
#include "CanvasElement.hpp"

CanvasElement::CanvasElement(int width, int height, const string &name)
    : ClarityNode(name, "canvas", CppType::NoData),
      width_(width),
      height_(height) {
    setAttribute("width", val(width_));
    setAttribute("height", val(height_));
    val domElement = cle_["domElement"];
    val canvasInit = 

}
