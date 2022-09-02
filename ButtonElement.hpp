#ifndef ButtonElement_hpp
#define ButtonElement_hpp

#include "WebElemNode.hpp"

namespace clarity {

class ButtonElement : public WebElemNode

{
   public:
    inline ButtonElement(const string &name, const string &tag,
                         const CppType storedValueType)
        : WebElemNode(name, tag, storedValueType) {
        boundField_ = "textContent";
    }
};

}  // namespace clarity

#endif
