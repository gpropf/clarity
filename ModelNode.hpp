
#ifndef ModelNode_hpp
#define ModelNode_hpp

#include "clarity.hpp"
#include "ControlNetworkNode.hpp"

namespace clarity
{
    class ModelNode : public ControlNetworkNode
    {
    public:
        ModelNode(void *anyvalPtr) : ControlNetworkNode(anyvalPtr) {}
        ModelNode(CppType anyvalPtrType) : ControlNetworkNode(anyvalPtrType) {}
        //ModelNode(void *anyvalPtr, CppType anyvalPtrType): ControlNetworkNode(anyvalPtr, anyvalPtrType) {}
        // void updateViewFromModel() {}
        virtual void updatePeers() {}
        // virtual void updateModelFromView() {}

        virtual val getVal() const
        {
            //ControlNetworkNode::getVal();
            // cout << "GETVAL called for ModelNode!\n\n";
            if (anyvalPtr_ == nullptr)
            {
                return val(NULL);
            }
            switch (this->anyvalPtrType_)
            {
            case CppType::Int:
            cout << "GETVAL called for ModelNode! Int\n\n";
                return val(cpp2js<int>(anyvalPtr_));
                break;
            case CppType::Float:
            cout << "GETVAL called for ModelNode! Float\n\n";
                return val(cpp2js<float>(anyvalPtr_));
                break;
            case CppType::Double:
            cout << "GETVAL called for ModelNode! Double\n\n";
                return val(cpp2js<double>(anyvalPtr_));
                break;
            case CppType::String:
            cout << "GETVAL called for ModelNode! String\n\n";
                return val(cpp2js<string>(anyvalPtr_));
                break;
            case CppType::NoData:
            cout << "GETVAL called for ModelNode! NoData\n\n";
            default:
                return val(NULL);
                break;
            }
        }

        virtual void setVal(const val &inval)
        {
            ControlNetworkNode::setVal(inval);
            nodeVal = inval;

            if (anyvalPtr_ == nullptr)
            {
                return;
            }

            switch (this->anyvalPtrType_)
            {
            case CppType::Int:
                *reinterpret_cast<int *>(anyvalPtr_) = this->jsval_.call<int>("jsToCPPVal", inval);
                cout << "C++ side: New Int Value: " << *reinterpret_cast<int *>(anyvalPtr_) << endl;
                break;
            case CppType::Float:
                *reinterpret_cast<float *>(anyvalPtr_) = this->jsval_.call<float>("jsToCPPVal", inval);
                cout << "C++ side: New Float Value: " << *reinterpret_cast<float *>(anyvalPtr_) << endl;
                break;
            case CppType::Double:
                //*reinterpret_cast<double *>(anyvalPtr_) = this->jsval_["anyval"].as<double>();
                *reinterpret_cast<double *>(anyvalPtr_) = this->jsval_.call<double>("jsToCPPVal", inval);
                cout << "C++ side: New Double Value: " << *reinterpret_cast<double *>(anyvalPtr_) << endl;
                break;
            case CppType::String:
                *reinterpret_cast<string *>(anyvalPtr_) = this->jsval_.call<string>("jsToCPPVal", inval);
                cout << "C++ side: New String Value: " << *reinterpret_cast<string *>(anyvalPtr_) << endl;
                break;
            case CppType::NoData:
                cout << "C++ side: This element contains no data." << endl;
                break;
            default:
                break;
            }

            return; // this->jsval_["anyval"];
        }
    };
}

#endif
