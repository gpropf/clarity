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
      cout << "WebElemNodeWebElemNode\n\n\n";

      cout << "DTEST:WebElemNode(const string &name, const string &tag, const CppType anyvalPtrType): " << (int)anyvalPtrType << " id = " << id_ << "\n";

      jsval_.set("cpptype", val(anyvalPtrType));
      jsval_.set("tag", val(tag));

      jsval_.set("id", val(id_));
      jsval_.set("name", val(name));

      tag_ = tag;
      anyvalPtr_ = nullptr;
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

    string getId() const { return name_; }
    // void setId(string id) { id_ = id; }

    void setJsval(val jsval) { jsval_ = jsval; }
    void *getAnyvalPtr() const { return anyvalPtr_; }
    void setAnyvalPtr(void *valptr) { anyvalPtr_ = valptr; }
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
