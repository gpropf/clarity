#ifndef CanvasElement_hpp
#define CanvasElement_hpp

#include "ClarityNode.hpp"

namespace clarity {

class CanvasElement : public ClarityNode

{
   public:
    CanvasElement(int width, int height, const string &name = "");

   protected:
    int width_, height_;
};

}  // namespace clarity

#endif
