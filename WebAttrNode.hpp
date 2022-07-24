#ifndef WebAttrNode_hpp
#define WebAttrNode_hpp

#include "clarity.hpp"
#include "WebNode.hpp"


namespace clarity {


/**
   * @brief Represents an attribute of another element such as bgcolor.
   * This is not necessarily a dom element that we control though it may be.
   *
   */
  class WebAttrNode : public virtual WebNode
  {
  protected:
    string attributeName_;

  public:
    void updatePeers() {} // FIXME

    WebAttrNode(const string &attributeName, const CppType anyvalPtrType,
                ControlNetworkNode *parent) : attributeName_(attributeName),
                                              WebNode(anyvalPtrType)
    {
      parent_ = parent;
      boundField_ = attributeName;
      val parentDomelement = parent_->getJSval()["domElement"];
      // bool cln = parent->clean_;
      jsval_.set("domElement", parentDomelement);
    }

    // void updateViewFromModel()
    // {
    //   // FIXME: fill in method
    // }

    virtual void setVal(const val &inval)
    {
      ControlNetworkNode::setVal(inval);
      val domElement = jsval_["domElement"];
      domElement.call<void>("setAttribute", boundField_, inval);
    }
  };
  
}
#endif
