#ifndef CanvasElement_hpp
#define CanvasElement_hpp

#include "ClarityNode.hpp"

namespace clarity {

class CanvasElement : public ClarityNodeBase {
   public:
    CanvasElement(const string &name, const string &tag,
                   bool useExistingDOMElement)
        : ClarityNodeBase(name, tag, useExistingDOMElement) {}

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
    CanvasGrid(const string &name, const string &tag,
               const CppType storedValueType, bool useExistingDOMElement)
        : CanvasElement(name, tag, storedValueType, useExistingDOMElement) {}

    inline virtual void setVal(const val &inval) {
        clean_ = false;
        // Needs to be given pointer to array data and dataDimensionality info.
        // So this information should reside in inval.
    }

    virtual val getVal() const {
        val domElement = cle_["domElement"];

        // Needs to read the internal state of the CG object and transfer it
        // back to the array in the MN.
        
        return domElement; // Fixme: I'm doing this just to shut the compiler up about not returning a value.
    }

   protected:
    int width_, height_;  //!< Width and Height in screen pixels.
    double scaleFactorH_ = 1.0;
    double scaleFactorV_ = 1.0;
};

}  // namespace clarity

#endif
