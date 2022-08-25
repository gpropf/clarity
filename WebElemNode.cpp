#include "clarity.hpp"
#include "WebElemNode.hpp"

using namespace clarity;

WebElemNode::WebElemNode(const string &name, const string &tag,
                         const CppType storedValueType) : WebNode(name, storedValueType)
{    
    cle_.call<void>("createDOMElement", id_, tag, storedValueType, name);
    cle_.set("name", val(name));
    tag_ = tag;
    name_ = name;
    boundField_ = "value";
    ControlNetworkNode::switchboard[id_] = this;
}

static map<string, std::function<void()>> callbackMap;
