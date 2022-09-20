#ifndef CanvasElement_hpp
#define CanvasElement_hpp

#include "ClarityNode.hpp"

namespace clarity {

class CanvasElement : public ClarityNode {
   public:
    CanvasElement(const string &name, const string &tag,
                  const CppType storedValueType, bool useExistingDOMElement)
        : ClarityNode(name, tag, storedValueType, useExistingDOMElement) {}

    inline void setDrawFuntionName(const string &drawFuntionName) {
        drawFuntionName_ = drawFuntionName;
    }

    inline void refreshView() { cle_.call<void>(drawFuntionName_.c_str()); }

   protected:
    int width_, height_;
    val drawFunction_;
    string drawFuntionName_;
};

/**
 * @brief Grid representation of a 2D array of values using a CanvasElement as
 * it's base object. We obviously need some kind of direct "pull" method to
 * actively get the array data and map it into the canvas quickly.
 *
 */
class CanvasGrid : public CanvasElement {
    // void setVal(const val &inval) {}
    // val getVal() const {}
};

}  // namespace clarity

#endif
