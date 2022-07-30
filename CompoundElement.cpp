#include "clarity.hpp"
#include "CompoundElement.hpp"

clarity::CompoundElement::CompoundElement(const string &name,
                                          const string &tag,
                                          const CppType anyvalPtrType,
                                          clarity::WebElemNode *innerElement)
                                           : clarity::WebElemNode(name, tag, anyvalPtrType),
                                                                                innerElement_(innerElement)
{
  //boundField_ = "value";
  this->appendChild(innerElement);
  outerElement_ = new clarity::WebElemNode(name, "div", CppType::NoData);
};

 

val clarity::CompoundElement::getVal() const
{
  return innerElement_->getVal();
}

void clarity::CompoundElement::setVal(const val &inval) {
  innerElement_->setVal(inval);
}