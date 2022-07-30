#ifndef ButtonElement_hpp
#define ButtonElement_hpp

//#include "clarity.hpp"
#include "WebElemNode.hpp"

namespace clarity
{

  class ButtonElement : public WebElemNode

  {
  public:
    ButtonElement(const string &name, const string &tag, const CppType anyvalPtrType);
  };

}

#endif
