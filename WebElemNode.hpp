#ifndef WebElemNode_hpp
#define WebElemNode_hpp

#include "clarity.hpp"
#include "WebNode.hpp"

namespace clarity
{

  class WebElemNode : public WebNode
  {
  public:
  protected:
    string tag_;
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
    WebElemNode(const string &name,
                const string &tag,
                const CppType storedValueType);

    static map<string, std::function<void()>> callbackMap;

    inline void setAttribute(const string &attr, const val &value)
    {
      val domElement = cle_["domElement"];
      domElement.call<void>("setAttribute", attr, value);
    };

    inline void addEventListenerByName(const string &eventName, const string &callbackName)
    {
      cle_.call<void>("addEventListenerById", eventName, callbackName);
    }

    inline void addJSEventListener(const string &eventName, val eventCallback)
    {
      cle_.call<void>("addEventListener", eventName, eventCallback);
    }

    inline string getTag() const { return tag_; }

    inline int getId() const { return id_; }

    inline void setJsval(val jsval) { cle_ = jsval; }

    inline CppType getStoredValueType() const { return storedValueType_; }

    inline void setStoredValueType(CppType cppType)
    {
      storedValueType_ = cppType;
      cle_.set("cpptype", cppType);
    }

    inline static void runCallbackById(const string &id) { callbackMap[id](); }
  };

}

#endif
