
#ifndef ModelNode_hpp
#define ModelNode_hpp

#include "ClarityNode.hpp"
#include "clarity.hpp"

namespace clarity {
template <typename T>
class ModelNode : public ClarityNode {
   public:
    ModelNode(CppType storedValueType) : ClarityNode(storedValueType) {}

    ModelNode(T *dynval) : dynval_(dynval) {}

    ModelNode(T *dynval, CppType storedValueType)
        : ClarityNode(storedValueType) {
        dynval_ = dynval;
    }

    ModelNode(T *dynval, CppType storedValueType, const string& name)
        : ModelNode(dynval, storedValueType) {
        name_ = name;
    }

    virtual val getVal() const {
        if (dynval_ == nullptr) {
            return val(NULL);
        }

        return val(cpp2js<T>(dynval_));
    }

    void setVal(const val &inval) {
        //cout << "ModelNode::setVal 1\n";
        //ControlNetworkNode::setVal(inval);
       // cout << "ModelNode::setVal 2\n";
       assert(dynval_ != nullptr);
        *reinterpret_cast<T *>(dynval_) =
            this->cle_.template call<T>("jsToCPPVal", inval);
       // cout << "ModelNode::setVal 3\n";
        pushValToPeers(this);
       // cout << "ModelNode::setVal 4\n";
    }

    T *dynval_;
};
}  // namespace clarity

#endif
