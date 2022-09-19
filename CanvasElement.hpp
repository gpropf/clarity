#ifndef CanvasElement_hpp
#define CanvasElement_hpp

#include "ClarityNode.hpp"

namespace clarity {

class CanvasElement : public ClarityNode

{
   public:
    CanvasElement(int width, int height, const string &name = "",
                  val drawFunction_ = val(NULL));

    CanvasElement(int width, int height, const string &name = "",
                  const string &drawFuntionName = "");

   protected:
    int width_, height_;
    val drawFunction_;
};

}  // namespace clarity

#endif
