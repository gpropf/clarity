#ifndef CanvasElement_hpp
#define CanvasElement_hpp

#include "WebElemNode.hpp"

namespace clarity
{

  class CanvasElement : public WebElemNode

  {
  public:
    CanvasElement(int width, int height, const string &name = "");
protected:
  int width_, height_;

  };

}

#endif
