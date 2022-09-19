//#include "clarity.hpp"
#include "CanvasElement.hpp"

// CanvasElement::CanvasElement(int width, int height, const string &name,
//                              val drawFunction)
//     : ClarityNode(name, "canvas", CppType::NoData),
//       width_(width),
//       height_(height) {
//     setAttribute("width", val(width_));
//     setAttribute("height", val(height_));
//     val domElement = cle_["domElement"];
//     // if (drawFunction != val(NULL)) {
//     //     drawFunction_ = drawFunction;
//     // } else {
//     //     drawFunction_ = cle_["drawGreenTestPattern"];
//     // }
//     // drawFunction_();
//     cle_.call<void>("drawGreenTestPattern");

//     // drawFunction_();
//     //   val canvasInit =
// }

// CanvasElement::CanvasElement(int width, int height, const string &name,
//                              const string &drawFuntionName)
//     : CanvasElement(width, height, name, val(NULL)) {
//     drawFunction_ = cle_[drawFuntionName];

//     //cle_.call<void>("drawGreenTestPattern");

//    cle_.call<void>(drawFuntionName.c_str());
//   //   drawFunction_();
// }

CanvasElement::CanvasElement(const string &drawFuntionName)
    : CanvasElement() {
    drawFunction_ = cle_[drawFuntionName];

    //cle_.call<void>("drawGreenTestPattern");

   cle_.call<void>(drawFuntionName.c_str());
  //   drawFunction_();
}
