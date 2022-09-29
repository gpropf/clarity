#ifndef CanvasElement_hpp
#define CanvasElement_hpp

#include "ClarityNode.hpp"

namespace clarity {

template <typename Dt>
class CanvasElement : public ClarityNode<Dt> {
   public:
    CanvasElement(const string &name, const string &tag,
                  bool useExistingDOMElement)
        : ClarityNode<Dt> (name, tag, useExistingDOMElement) {}

    inline void setDrawFuntionName(const string &drawFuntionName) {
        drawFuntionName_ = drawFuntionName;
    }

    inline void refreshView() {
        CanvasElement::cle_.template call<void>(drawFuntionName_.c_str());
    }

    void cppValToDomElement() { this->setVal(val(*this->cppVal_)); }

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
template <class Dt>
class CanvasGrid : public CanvasElement<Dt> {
    // void setVal(const val &inval) {}
    // val getVal() const {}
   public:
    CanvasGrid(const string &name, const string &tag,
               const CppType storedValueType, bool useExistingDOMElement)
        : CanvasElement<Dt>(name, tag, storedValueType, useExistingDOMElement) {
    }

    CanvasGrid(const string &name, const string &tag,
               bool useExistingDOMElement)
        : CanvasElement<Dt>(name, tag, useExistingDOMElement) {}

    inline virtual void setVal(const val &inval) {
        this->clean_ = false;
        // Needs to be given pointer to array data and dataDimensionality info.
        // So this information should reside in inval.
    }

    virtual val getVal() const {
        val domElement = this->cle_["domElement"];

        // Needs to read the internal state of the CG object and transfer it
        // back to the array in the MN.

        return domElement;  // Fixme: I'm doing this just to shut the compiler
                            // up about not returning a value.
    }

    void cppValToDomElement() { cout <<"cppValToDomElement!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"; }
    //void cppValToDomElement();

   protected:
    int width_, height_;  //!< Width and Height in screen pixels.
    double scaleFactorH_ = 1.0;
    double scaleFactorV_ = 1.0;
};

}  // namespace clarity

#endif
