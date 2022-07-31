
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
        // ModelNode(void *anyvalPtr) : ControlNetworkNode(anyvalPtr) {}
        ModelNode(CppType anyvalPtrType) : ControlNetworkNode(anyvalPtrType)
        {
            cout << "ModelNode(CppType anyvalPtrType): " << (int)anyvalPtrType << " id = " << id_ << "\n";
        }

        ModelNode(T *dynval) : dynval_(dynval)
        {
            cout << "ModelNode(T * dynval)" << typeid(T).name() << " id = " << id_ << "\n";
            //cout << "ModelNode(T * dynval) id = " << id_ << "\n";
        }

        ModelNode(T *dynval, CppType anyvalPtrType) : ControlNetworkNode(anyvalPtrType)
        {
            dynval_ = dynval;
            cout << "ModelNode(T *dynval, CppType anyvalPtrType)" << typeid(T).name() << " id = " << id_ << "\n";
        }
       
        virtual void updatePeers() {}        

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
            *reinterpret_cast<T *>(dynval_) = this->jsval_.template call<T>("jsToCPPVal", inval);
        }

        T *dynval_;
    };
}

#endif
