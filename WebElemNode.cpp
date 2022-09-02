#include "WebElemNode.hpp"

#include "clarity.hpp"

using namespace clarity;

WebElemNode::WebElemNode(const string &name, const string &tag,
                         const CppType storedValueType)
    : WebNode(name, storedValueType) {
    cle_.call<void>("createDOMElement", id_, tag, storedValueType, name);
    cle_.set("name", val(name));
    tag_ = tag;
    name_ = name;
    boundField_ = "value";
    ControlNetworkNode::switchboard[id_] = this;
}

inline void WebElemNode::setStoredValueType(CppType cppType) {
    storedValueType_ = cppType;
    cle_.set("cpptype", cppType);
}

inline void WebElemNode::setAttribute(const string &attr, const val &value) {
    val domElement = cle_["domElement"];
    domElement.call<void>("setAttribute", attr, value);
}

inline void WebElemNode::addEventListenerByName(const string &eventName,
                                                const string &callbackName) {
    cle_.call<void>("addEventListenerById", eventName, callbackName);
}

inline void WebElemNode::addJSEventListener(const string &eventName,
                                            val eventCallback) {
    cle_.call<void>("addEventListener", eventName, eventCallback);
}

static map<string, std::function<void()>> callbackMap;
