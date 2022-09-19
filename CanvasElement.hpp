#ifndef CanvasElement_hpp
#define CanvasElement_hpp

#include "ClarityNode.hpp"

namespace clarity {

class CanvasElement : public ClarityNode

{
   public:
    CanvasElement() : ClarityNode() {
        cout << "NO ARGS: CanvasElement!! CanvasElement!! CanvasElement!! CanvasElement!! CanvasElement!! CanvasElement!! \n";
    }

    CanvasElement(const string &drawFuntionName);

    CanvasElement(const string &name, const string &tag,
                  const CppType storedValueType, bool useExistingDOMElement)
        : ClarityNode(name, tag, storedValueType, useExistingDOMElement) {
        cle_.call<void>("drawBlueTestPattern");
        cout << "CanvasElement!! CanvasElement!! CanvasElement!! CanvasElement!! CanvasElement!! CanvasElement!! \n";
    }

    // CanvasElement(int width, int height, const string &name = "",
    //               const string &drawFuntionName = "");

    // CanvasElement(const string &drawFuntionName);

    inline void setDrawFuntionName(const string &drawFuntionName) {
        drawFuntionName_ = drawFuntionName;
    }

    inline void refreshView() { cle_.call<void>(drawFuntionName_.c_str()); }

   protected:
    int width_, height_;
    val drawFunction_;
    string drawFuntionName_;
};

}  // namespace clarity

#endif
