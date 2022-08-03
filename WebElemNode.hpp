#ifndef WebElemNode_hpp
#define WebElemNode_hpp

#include "clarity.hpp"
#include "WebNode.hpp"

namespace clarity
{

  class WebElemNode : public  WebNode
  {
  public:
  protected:
    vector<ControlNetworkNode *> children_;

    // string tag_, name_;
    string tag_; //, name_;
    string stringId_;

  public:
    /**
     * @brief Construct a new Web Element object
     *
     * @param name String name of element, may be empty.
     * @param tag HTML tag of this element
     * @param anyvalPtrType C++ type of data contained within
     *
     */
    WebElemNode(const string &name, const string &tag,
                const CppType anyvalPtrType) : WebNode(name, anyvalPtrType)
    {
      cout << "WebElemNode(const string &name, const string &tag, const CppType anyvalPtrType): " << (int)anyvalPtrType << " id = " << id_ << "\n";

      // jsval_.set("cpptype", val(anyvalPtrType));
      // jsval_.set("tag", val(tag));
      // jsval_.set("id", val(id_));
      // jsval_.set("name", val(name));

      jsval_.call<void>("createDOMElement", id_, tag, anyvalPtrType, name);
      tag_ = tag;      
      boundField_ = "value";
      ControlNetworkNode::switchboard[id_] = this;
    }

    void setAttribute(const string &attr, const val &value)
    {
      val domElement = jsval_["domElement"];
      domElement.call<void>("setAttribute", attr, value);
    }

    bool appendChild(WebNode *child)
    {
      children_.push_back(child);
      jsval_.call<void>("appendChild", child->getJSval());
      return true; // FIXME: need to check for duplicate ids.
    }

    void addEventListenerByName(const string &eventName, const string &callbackName)
    {
      jsval_.call<void>("addEventListenerById", eventName, callbackName);
    }
    //=====================
    // static map<const int, WebElemNode *> switchboard;
    static map<string, std::function<void()>> callbackMap;

    string getTag() const { return tag_; }

    int getId() const { return id_; }
    // void setId(string id) { id_ = id; }

    void setJsval(val jsval) { jsval_ = jsval; }
    
    CppType getAnyvalPtrType() const { return anyvalPtrType_; }
    void setAnyvalPtrType(CppType cppType)
    {
      anyvalPtrType_ = cppType;
      jsval_.set("cpptype", cppType);
    }

    // static void updateModelFromViewById(const int id) { switchboard[id]->updateModelFromView(); }
    // static WebElemNode &getCLElementById(const int id) { return *(switchboard[id]); }
    static void runCallbackById(const string &id) { callbackMap[id](); }
  };

}

#endif
