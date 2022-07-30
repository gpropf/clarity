#include "clarity.hpp"
#include "ButtonElement.hpp"

clarity::ButtonElement::ButtonElement(const string &name, const string &tag, const CppType anyvalPtrType) : WebElemNode(name, tag, anyvalPtrType)
{
  boundField_ = "textContent";
}
