#include "clarity.hpp"
#include "LabelledInput.hpp"

clarity::LabelledInput::LabelledInput(const string &name,
                                          const string &tag,
                                          const CppType anyvalPtrType,
                                          clarity::WebElemNode *innerElement) : clarity::CompoundElement(name, tag, anyvalPtrType),
                                                                                innerElement_(innerElement)
{
  
};

val clarity::LabelledInput::getVal() const
{
  return innerElement_->getVal();
}

void clarity::LabelledInput::setVal(const val &inval) {
  innerElement_->setVal(inval);
}