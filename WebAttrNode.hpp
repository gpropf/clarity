#ifndef WebAttrNode_hpp
#define WebAttrNode_hpp

#include "WebNode.hpp"
#include "clarity.hpp"

namespace clarity {

/**
 * @brief Represents an attribute of another element such as bgcolor.
 * This is not necessarily a dom element that we control though it may be.
 *
 */
class WebAttrNode : public WebNode {
   protected:
    string attributeName_;

   public:
    WebAttrNode(const string &attributeName, const CppType storedValueType,
                ControlNetworkNode *parent);

    virtual void setVal(const val &inval);
};

}  // namespace clarity
#endif
