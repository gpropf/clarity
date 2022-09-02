#ifndef WebElemNode_hpp
#define WebElemNode_hpp

#include "WebNode.hpp"
#include "clarity.hpp"

namespace clarity {
class WebElemNode : public WebNode {
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
    WebElemNode(const string &name, const string &tag,
                const CppType storedValueType);

    static map<string, std::function<void()>> callbackMap;

    EMSCRIPTEN_KEEPALIVE inline void setAttribute(const string &attr,
                                                  const val &value);

    EMSCRIPTEN_KEEPALIVE void addEventListenerByName(
        const string &eventName, const string &callbackName);
    EMSCRIPTEN_KEEPALIVE void addJSEventListener(const string &eventName,
                                                 val eventCallback);

    inline string getTag() const { return tag_; }
    inline int getId() const { return id_; }
    inline void setCLE(val jsval) { cle_ = jsval; }

    inline CppType getStoredValueType() const { return storedValueType_; }
    EMSCRIPTEN_KEEPALIVE void setStoredValueType(CppType cppType);

    inline static void runCallbackById(const string &id) { callbackMap[id](); }
};

}  // namespace clarity

#endif
