//#include "clarity.hpp"
#include "CanvasElement.hpp"

CanvasElement::CanvasElement(int width, int height, const string &name,
                             val drawFunction)
    : ClarityNode(name, "canvas", CppType::NoData),
      width_(width),
      height_(height) {
    setAttribute("width", val(width_));
    setAttribute("height", val(height_));
    val domElement = cle_["domElement"];
    drawFunction_ = drawFunction;
    //drawFunction_();
    //  val canvasInit =
}
