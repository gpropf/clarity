#include "LabelElement.hpp"

#include "clarity.hpp"

clarity::LabelElement::LabelElement(const CppType storedValueType,
                                    const val &isFor, const string &name)
    : WebElemNode(name, "label", storedValueType) {
    boundField_ = "innerHTML";
    setAttribute("for", isFor);
}
