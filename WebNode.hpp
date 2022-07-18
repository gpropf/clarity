#ifndef WebNode_hpp
#define WebNode_hpp

#include "clarity.hpp"
#include "ControlNetworkNode.hpp"

namespace clarity
{

  class WebNode : public ControlNetworkNode
  {
  public:
    WebNode(const CppType anyvalPtrType) : ControlNetworkNode(anyvalPtrType) {}
    virtual val getVal() const
    {
      ControlNetworkNode::getVal();
      // cout << "GETVAL called for WebNode!\n\n";
      val domElement = jsval_["domElement"];
      string valueText = domElement[boundField_].as<string>();

      switch (this->anyvalPtrType_)
      {
      case CppType::Int:
        return val(stoi(valueText));
        break;
      case CppType::Float:
        return val(stof(valueText));
        break;
      case CppType::Double:
        return val(stod(valueText));
        break;
      case CppType::String:
        return val(valueText);
        break;
      case CppType::NoData:
      default:
        return val(NULL);
        break;
      }
    }

  protected:
    string boundField_;   
    // inline void setAsProxyFor(clarity::WebNode *proxyiedElement)
    // {
    //   boundField_ = proxyiedElement->boundField_;
    // }

  public:
    void setVal(const val &inval)
    {
      ControlNetworkNode::setVal(inval);
      val domElement = jsval_["domElement"];
      domElement.set(boundField_, inval);
    }

    // }
  };
}
#endif
