#include "clarity.hpp"
#include "LabelledInput.hpp"
#include "LabelElement.hpp"

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
  printNodeStats("LabelledInput");
}

clarity::LabelledInput::LabelledInput(const string &name,
              const string &inputFieldTag,
              const CppType inputFieldType)
{
  cout << "clarity::LabelledInput: 1\n";
  innerElement_ = new WebElemNode(name, inputFieldTag, inputFieldType);
  cout << "clarity::LabelledInput: 2\n";
  innerElement_->setAttribute("type", val("text"));
  cout << "clarity::LabelledInput: 3\n";
  label_ = new clarity::LabelElement(CppType::NoData, val(innerElement_->getId()));
  cout << "clarity::LabelledInput: 4\n";
  //clarity::CompoundElement(name, "div", CppType::NoData, innerElement_);
  cout << "clarity::LabelledInput: 5\n";
  this->appendChild(label_);
  cout << "clarity::LabelledInput: 6\n";
  this->appendChild(innerElement_);
  cout << "clarity::LabelledInput: 7\n";
}

val clarity::LabelledInput::getVal() const
{
  return innerElement_->getVal();
}

void clarity::LabelledInput::setVal(const val &inval)
{
  innerElement_->setVal(inval);
}