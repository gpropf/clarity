
#ifndef ModelNode_hpp
#define ModelNode_hpp

#include "ControlNetworkNode.hpp"
#include "clarity.hpp"

namespace clarity {
template <typename T>
class ModelNode : public ControlNetworkNode {
   public:
    ModelNode(CppType storedValueType) : ControlNetworkNode(storedValueType) {}

    ModelNode(T *dynval) : dynval_(dynval) {}

    ModelNode(T *dynval, CppType storedValueType)
        : ControlNetworkNode(storedValueType) {
        dynval_ = dynval;
    }

    virtual val getVal() const {
        if (dynval_ == nullptr) {
            return val(NULL);
        }

        return val(cpp2js<T>(dynval_));
    }

    virtual void setVal(const val &inval) {
        ControlNetworkNode::setVal(inval);
        *reinterpret_cast<T *>(dynval_) =
            this->cle_.template call<T>("jsToCPPVal", inval);
        pushValToPeersThruAL(this);
    }

    T *dynval_;
};
}  // namespace clarity

#endif
