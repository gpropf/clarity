#include "CanvasElement.hpp"

#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"

val CLElement_ = val::global("CLElement");

CanvasElement::CanvasElement(const string &name, const string &tag,
                             const CppType storedValueType,
                             bool useExistingDOMElement)
    : ClarityNode(name, tag, storedValueType, useExistingDOMElement) {}

inline void CanvasElement::setDrawFuntionName(const string &drawFuntionName) {
    drawFuntionName_ = drawFuntionName;
}

inline void CanvasElement::refreshView() {
    cle_.call<void>(drawFuntionName_.c_str());
}

/**
 * @brief Grid representation of a 2D array of values using a CanvasElement as
 * it's base object. We obviously need some kind of direct "pull" method to
 * actively get the array data and map it into the canvas quickly.
 *
 */






