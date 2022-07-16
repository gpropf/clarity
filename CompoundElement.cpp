#include "clarity.hpp"
#include "CompoundElement.hpp"

clarity::CompoundElement::CompoundElement(const string &name,
                                                const string &tag,
                                                const CppType anyvalPtrType) : WebElemNode(name, tag, anyvalPtrType)
{
  boundField_ = "value";
  mainDiv_ = new clarity::WebElemNode(name, tag, anyvalPtrType);
}
