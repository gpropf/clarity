
#ifndef ModelNode_hpp
#define ModelNode_hpp

#include "clarity.hpp"
#include "ControlNetworkNode.hpp"

namespace clarity
{
    template <typename T>
    class ModelNode : public ControlNetworkNode
    {
    public:
        // ModelNode(void *storedValue) : ControlNetworkNode(storedValue) {}
        ModelNode(CppType storedValueType) : ControlNetworkNode(storedValueType)
        {
            // cout<< "ModelNode(CppType storedValueType): " << (int)storedValueType << " id = " << id_ << "\n";
        }

        ModelNode(T *dynval) : dynval_(dynval)
        {
            // cout<< "ModelNode(T * dynval)" << typeid(T).name() << " id = " << id_ << "\n";
            //// cout<< "ModelNode(T * dynval) id = " << id_ << "\n";
        }

        ModelNode(T *dynval, CppType storedValueType) : ControlNetworkNode(storedValueType)
        {
            dynval_ = dynval;
            // cout<< "ModelNode(T *dynval, CppType storedValueType)" << typeid(T).name() << " id = " << id_ << "\n";
        }

        virtual val getVal() const
        {
            if (dynval_ == nullptr)
            {
                return val(NULL);
            }

            return val(cpp2js<T>(dynval_));            
        }

        virtual void setVal(const val &inval)
        {
            ControlNetworkNode::setVal(inval);
            *reinterpret_cast<T *>(dynval_) = this->cle_.template call<T>("jsToCPPVal", inval);
            // cout<< "ModelNode::setVal() value is: " << *reinterpret_cast<T *>(dynval_) << ", id = " << id_ << "\n";            
            pushValToPeersThruAL(this);
        }

        T *dynval_;
    };
}

#endif
