#ifndef LabelElement_hpp
#define LabelElement_hpp

#include "WebElemNode.hpp"

namespace clarity
{

  class LabelElement : public WebElemNode

  {
  public:
    LabelElement(const CppType anyvalPtrType, const val &isFor, const string &name = "");
  };

}

#endif
