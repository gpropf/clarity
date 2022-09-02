#include "WebAttrNode.hpp"

#include "clarity.hpp"

using namespace clarity;

WebAttrNode::WebAttrNode(const string &attributeName,
                         const CppType storedValueType,
                         ControlNetworkNode *parent)
    : WebNode(attributeName, storedValueType) {
    parent_ = parent;
    boundField_ = attributeName;
    val parentDomelement = parent_->getCLE()["domElement"];
    cle_.set("domElement", parentDomelement);
}

void WebAttrNode::setVal(const val &inval) {
    ControlNetworkNode::setVal(inval);
    val domElement = cle_["domElement"];
    domElement.call<void>("setAttribute", boundField_, inval);
}
