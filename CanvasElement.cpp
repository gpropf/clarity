//#include "clarity.hpp"
#include "CanvasElement.hpp"

CanvasElement::CanvasElement(int width, int height, const string &name)
    : WebElemNode(name, "canvas", CppType::NoData),
      width_(width),
      height_(height) {
    setAttribute("width", val(width_));
    setAttribute("height", val(height_));
}
