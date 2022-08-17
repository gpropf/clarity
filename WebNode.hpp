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
    WebNode(const string &name, const CppType storedValueType) : ControlNetworkNode(name, storedValueType)
    {

      // cout<< "DTEST:WebNode(const CppType storedValueType):" << (int)storedValueType << " id = " << id_ << "\n";
    }

    val getVal() const
    {
      // ControlNetworkNode::getVal();
      printNodeStats("GETVAL");
      val domElement = cle_["domElement"];
      string valueText = domElement[boundField_].as<string>();

      switch (this->storedValueType_)
      {
      case CppType::Int:
        // cout<< "GETVAL Int: " << valueText << "\n";
        return val(stoi(valueText));
        break;
      case CppType::Float:
        // cout<< "GETVAL Float: " << valueText << "\n";
        return val(stof(valueText));
        break;
      case CppType::Double:
        // cout<< "GETVAL Double: " << valueText << "\n";
        return val(stod(valueText));
        break;
      case CppType::String:
        // cout<< "GETVAL String: " << valueText << "\n";
        return val(valueText);
        break;
      case CppType::NoData:
        // cout<< "GETVAL NoData\n";
      default:
        return val(NULL);
        break;
      }
    }

  protected:
    // inline void setAsProxyFor(clarity::WebNode *proxyiedElement)
    // {
    //   boundField_ = proxyiedElement->boundField_;
    // }

  public:
    string boundField_;
    void setVal(const val &inval)
    {
      ControlNetworkNode::setVal(inval);
      val domElement = cle_["domElement"];
      domElement.set(boundField_, inval);
    }

    // }
  };
}
#endif
