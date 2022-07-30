#include "clarity.hpp"
#include "LabelledInput.hpp"

clarity::LabelledInput::LabelledInput(const string &name,
                                      const string &tag,
                                      const CppType anyvalPtrType,
                                      clarity::WebElemNode *innerElement)
    : clarity::CompoundElement(name, tag, anyvalPtrType, innerElement)
{
  label_ = new clarity::WebElemNode("label_" + name, "label", clarity::CppType::String);
  label_->setAttribute("for", val(innerElement_->getId()));
  label_->boundField_ = "innerHTML";
  label_->setVal(val(name));
  cout << "LabelledInput constructor called! id = " << id_ << "\n";
  this->appendChild(label_);
};

val clarity::LabelledInput::getVal() const
{
  return innerElement_->getVal();
}

void clarity::LabelledInput::setVal(const val &inval)
{
  innerElement_->setVal(inval); 
}