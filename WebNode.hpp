#ifndef WebNode_hpp
#define WebNode_hpp

#include "clarity.hpp"
#include "ControlNetworkNode.hpp"

namespace clarity
{

  class WebNode : public ControlNetworkNode
  {
  public:
    WebNode() : ControlNetworkNode() {}
    WebNode(const string& name, const CppType anyvalPtrType) : ControlNetworkNode(anyvalPtrType)
    {

      cout << "DTEST:WebNode(const CppType anyvalPtrType):" << (int)anyvalPtrType << " id = " << id_ << "\n";
    }

    val getVal() const
    {
      // ControlNetworkNode::getVal();
      cout << "GETVAL called for WebNode, "
           << "id = " << id_ << "\n\n";
      val domElement = jsval_["domElement"];
      string valueText = domElement[boundField_].as<string>();

      switch (this->anyvalPtrType_)
      {
      case CppType::Int:
        cout << "GETVAL Int for " << valueText << "\n\n";
        return val(stoi(valueText));
        break;
      case CppType::Float:
        cout << "GETVAL Float\n\n";
        return val(stof(valueText));
        break;
      case CppType::Double:
        cout << "GETVAL Double\n\n";
        return val(stod(valueText));
        break;
      case CppType::String:
        cout << "GETVAL String\n\n";
        return val(valueText);
        break;
      case CppType::NoData:
        cout << "GETVAL NoData\n\n";
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
