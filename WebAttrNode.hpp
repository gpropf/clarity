#ifndef WebAttrNode_hpp
#define WebAttrNode_hpp

#include "clarity.hpp"
#include "WebNode.hpp"

namespace clarity
{

  /**
   * @brief Represents an attribute of another element such as bgcolor.
   * This is not necessarily a dom element that we control though it may be.
   *
   */
  class WebAttrNode : public WebNode
  {
  protected:
    string attributeName_;

  public:    

    WebAttrNode(const string &attributeName, const CppType storedValueType,
                ControlNetworkNode *parent) : WebNode(attributeName, storedValueType)
    {
      //WebNode(attributeName, storedValueType);
      parent_ = parent;
      boundField_ = attributeName;
      val parentDomelement = parent_->getJSval()["domElement"];
      // bool cln = parent->clean_;
      cle_.set("domElement", parentDomelement);
    }

    // void updateViewFromModel()
    // {
    //   // FIXME: fill in method
    // }

    virtual void setVal(const val &inval)
    {
      ControlNetworkNode::setVal(inval);
      val domElement = cle_["domElement"];
      domElement.call<void>("setAttribute", boundField_, inval);
    }
  };

}
#endif
