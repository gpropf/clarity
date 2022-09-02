#include "CompoundElement.hpp"

#include "clarity.hpp"

clarity::CompoundElement::CompoundElement(const string &name, const string &tag,
                                          const CppType anyvalPtrType,
                                          clarity::WebElemNode *innerElement)
    : clarity::WebElemNode(name, tag, anyvalPtrType),
      innerElement_(innerElement) {
    this->appendChild(innerElement);
};

val clarity::CompoundElement::getVal() const { return innerElement_->getVal(); }

void clarity::CompoundElement::setVal(const val &inval) {
    innerElement_->setVal(inval);
}