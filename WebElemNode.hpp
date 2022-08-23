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
  

    // string tag_, name_;
    string tag_; //, name_;
    string stringId_;

  public:
    /**
     * @brief Construct a new Web Element object
     *
     * @param name String name of element, may be empty.
     * @param tag HTML tag of this element
     * @param storedValueType C++ type of data contained within
     *
     */
    WebElemNode(const string &name, const string &tag,
                const CppType storedValueType) : WebNode(name, storedValueType)
    {
      // cout<< "WebElemNode(const string &name, const string &tag, const CppType storedValueType): " << (int)storedValueType << " id = " << id_ << "\n";

      // jsval_.set("cpptype", val(storedValueType));
      // jsval_.set("tag", val(tag));
      // jsval_.set("id", val(id_));
      // jsval_.set("name", val(name));

      cle_.call<void>("createDOMElement", id_, tag, storedValueType, name);
      cle_.set("name", val(name));
      tag_ = tag;  
      name_ = name;    
      boundField_ = "value";
      ControlNetworkNode::switchboard[id_] = this;
    }

    

    void setAttribute(const string &attr, const val &value)
    {
      val domElement = cle_["domElement"];
      domElement.call<void>("setAttribute", attr, value);
    }

    

    void addEventListenerByName(const string &eventName, const string &callbackName)
    {
      cle_.call<void>("addEventListenerById", eventName, callbackName);
    }
    //=====================
    // static map<const int, WebElemNode *> switchboard;
    static map<string, std::function<void()>> callbackMap;

    string getTag() const { return tag_; }

    int getId() const { return id_; }
    // void setId(string id) { id_ = id; }

    void setJsval(val jsval) { cle_ = jsval; }
    
    CppType getStoredValueType() const { return storedValueType_; }
    void setStoredValueType(CppType cppType)
    {
      storedValueType_ = cppType;
      cle_.set("cpptype", cppType);
    }

    // static void updateModelFromViewById(const int id) { switchboard[id]->updateModelFromView(); }
    // static WebElemNode &getCLElementById(const int id) { return *(switchboard[id]); }
    static void runCallbackById(const string &id) { callbackMap[id](); }
  };

}

#endif
