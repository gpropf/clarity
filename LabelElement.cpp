#include "clarity.hpp"
#include "LabelElement.hpp"

clarity::LabelElement::LabelElement(const CppType anyvalPtrType, const val &isFor, const string &name)
    : WebElemNode(name, "label", anyvalPtrType)
{
  boundField_ = "innerHTML";
  setAttribute("for", isFor);
}
