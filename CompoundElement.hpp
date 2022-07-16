#ifndef LabelledInputField_hpp
#define LabelledInputField_hpp

//#include "clarity.hpp"
#include "WebElemNode.hpp"

namespace clarity
{

  class CompoundElement : public WebElemNode

  {
  public:
    CompoundElement(const string &name, const string &tag, const CppType anyvalPtrType);
    //string *labelText_;
    clarity::WebElemNode *mainDiv_;
    // clarity::WebElemNode *label_;
    // clarity::WebElemNode *inputField_;    
  };

}

#endif