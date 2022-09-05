
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

    void setVal(const val &inval) {
        cout << "ModelNode::setVal 1\n";
        //ControlNetworkNode::setVal(inval);
        cout << "ModelNode::setVal 2\n";
        *reinterpret_cast<T *>(dynval_) =
            this->cle_.template call<T>("jsToCPPVal", inval);
        cout << "ModelNode::setVal 3\n";
        pushValToPeers(this);
        cout << "ModelNode::setVal 4\n";
    }

    T *dynval_;
};
}  // namespace clarity

#endif
