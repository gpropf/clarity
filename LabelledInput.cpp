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
  // cout<< "LabelledInput constructor called! id = " << id_ << "\n";
  this->appendChild(label_);
  printNodeStats("LabelledInput");
}

clarity::LabelledInput::LabelledInput(const string &name,
                                      const string &inputFieldTag,
                                      const CppType inputFieldType)
{
  innerElement_ = new WebElemNode(name, inputFieldTag, inputFieldType);
  innerElement_->setAttribute("type", val("text"));
  label_ = new clarity::LabelElement(CppType::NoData, val(innerElement_->getId()));
  this->appendChild(label_);
  this->appendChild(innerElement_);
}

// template <typename T>
// clarity::LabelledInput::LabelledInput(const string &name,
//                                       const string &inputFieldTag,
//                                       const CppType inputFieldType,
//                                       T *modelField)
//     : ControlNetworkNode(name, inputFieldType, modelField)
// {  
//   innerElement_ = new WebElemNode(name, inputFieldTag, inputFieldType);
//   innerElement_->setAttribute("type", val("text"));
//   label_ = new clarity::LabelElement(CppType::NoData, val(innerElement_->getId()));
//   this->appendChild(label_);
//   this->appendChild(innerElement_);
// }

val clarity::LabelledInput::getVal() const
{
  return innerElement_->getVal();
}

void clarity::LabelledInput::setVal(const val &inval)
{
  innerElement_->setVal(inval);
}