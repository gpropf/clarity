#ifndef CanvasElement_hpp
#define CanvasElement_hpp

//#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"

namespace clarity {

class CanvasElement : public ClarityNode {
   public:
    CanvasElement(const string &name, const string &tag,
                  const CppType storedValueType, bool useExistingDOMElement);

    void setDrawFuntionName(const string &drawFuntionName);

    void refreshView();

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
    //  class CLNodeFactory;
    // friend class CLNodeFactory;

    static const array<string, 8> colors;

    CanvasGrid(const string &name, const string &tag,
               const CppType storedValueType, bool useExistingDOMElement);

    virtual void setVal(const val &inval);

    virtual val getVal() const;

   protected:
    int width_, height_;  //!< Width and Height in screen pixels.
    double scaleFactorH_ = 1.0;
    double scaleFactorV_ = 1.0;
    int gridWidth_, gridHeight_, pixelWidth_, pixelHeight_, cellWidth_,
        cellHeight_;
    unsigned char *dataptr_;
    unsigned char currentCellVal_ = 0;

    virtual void drawGrid() const;

   public:
    EMSCRIPTEN_KEEPALIVE
    CanvasGrid(const string &name, const string &tag,
               const CppType storedValueType, bool useExistingDOMElement,
               int gridWidth, int gridHeight, int pixelWidth, int pixelHeight);

    EMSCRIPTEN_KEEPALIVE void initcg();
    int setValXY(int x, int y);
};

}  // namespace clarity

#endif
