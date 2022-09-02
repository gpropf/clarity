#ifndef WebNode_hpp
#define WebNode_hpp

#include "clarity.hpp"
#include "ControlNetworkNode.hpp"
#include "ModelNode.hpp"

namespace clarity
{
  class WebNode : public ControlNetworkNode
  {
  public:
    WebNode() : ControlNetworkNode() {}
    WebNode(const string &name, const CppType storedValueType) : ControlNetworkNode(name, storedValueType) {}

    template <typename T>
    void installModelNode(T *dynval)
    {
      ModelNode<T> *m = new ModelNode<T>(dynval, storedValueType_);
      appendChild(m);
      m->addALPeer(ActiveLink(this, 1));
      m->pushValToPeersThruAL(m);
    }

    val getVal() const
    {
      val domElement = cle_["domElement"];
      string valueText = domElement[boundField_].as<string>();

      switch (this->storedValueType_)
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

  public:
    string boundField_;
    void setVal(const val &inval)
    {
      ControlNetworkNode::setVal(inval);
      val domElement = cle_["domElement"];
      domElement.set(boundField_, inval);
    }
  };
}

#endif
