#ifndef CLNodeFactory_hpp
#define CLNodeFactory_hpp

#include "ModelNode.hpp"

namespace clarity {
template <typename V>
class CLNodeFactory {
   public:
    string tag_;   //!< Tag to be used with elements this factory builds.
    string name_;  //!< Name to be used with elements this factory builds.
    CppType storedValueType_;  //!< storedValueType to be used with elements
                               //!< this factory builds.
    V *storedValue_;  //!< Actually only used when creating a model node along
                      //!< with a web control.

    string boundField_;
    ControlNetworkNode *parent_;
    bool useExistingDOMElement_ = false;

    map<string, val> attrs_;

    inline CLNodeFactory withAttributes(const map<string, val> &attrs) {
        CLNodeFactory cpy(*this);
        cpy.attrs_ = attrs;
        return cpy;
    }

    inline CLNodeFactory() {}
    inline CLNodeFactory(const string &tag, const string &name,
                         CppType storedValueType)
        : tag_(tag), name_(name), storedValueType_(storedValueType) {}

    inline CLNodeFactory(const string &tag, const string &name,
                         CppType storedValueType, V *storedValue)
        : tag_(tag),
          name_(name),
          storedValueType_(storedValueType),
          storedValue_(storedValue) {}

    // inline ControlNetworkNode *build() { return new T(name_, tag_,
    // storedValueType_); }

    // inline ControlNetworkNode *buildWithAttributes() {
    //     ControlNetworkNode *newNode = new T(name_, tag_,
    //     storedValueType_); newNode->setAttributes(attrs_); return newNode;
    // }

    inline ControlNetworkNode *build() {
        ControlNetworkNode *newNode = new ControlNetworkNode(
            name_, tag_, storedValueType_, useExistingDOMElement_);
        newNode->setAttributes(attrs_);
        if (parent_) {
            parent_->appendChild(newNode);
        }
        return newNode;
    }

    // inline ControlNetworkNode *buildInsideNode(ControlNetworkNode *outerNode)
    // {
    //     ControlNetworkNode *innerNode = build();
    //     outerNode->appendChild(innerNode);
    //     return outerNode;
    // }

    inline ControlNetworkNode *buildWithModelNode(
        const val transformFn = val(1)) {
        ControlNetworkNode *cnn = build();
        ModelNode<V> *mn = new ModelNode<V>(storedValue_, storedValueType_);
        mn->addPeer(clarity::ControlNetworkNode::ActiveLink(cnn, transformFn));
        mn->pushValToPeers(mn);
        return cnn;
    }

    inline CLNodeFactory withBoundField(const string &boundField) {
        CLNodeFactory cpy(*this);
        cpy.boundField_ = boundField;
        return cpy;
    }

    inline CLNodeFactory createChildrenOf(ControlNetworkNode *parent) {
        CLNodeFactory cpy(*this);
        cpy.parent_ = parent;
        return cpy;
    }

    inline CLNodeFactory createAttributesOf(ControlNetworkNode *parent) {
        return createChildrenOf(parent).withExistingDOMElement();
    }

    inline CLNodeFactory createAttributesOf(int parentId) {  // FIXME
        // CLNodeFactory cpy(*this);
        // cpy.parent_ = parent;
        // return cpy;
    }

    inline CLNodeFactory withTag(const string &tag) {
        CLNodeFactory cpy(*this);
        cpy.tag_ = tag;
        return cpy;
    }

    inline CLNodeFactory withName(const string &name) {
        CLNodeFactory cpy(*this);
        cpy.name_ = name;
        return cpy;
    }

    inline CLNodeFactory withParent(ControlNetworkNode *parent) {
        CLNodeFactory cpy(*this);
        cpy.parent_ = parent;
        return cpy;
    }

    inline CLNodeFactory withStoredValueType(clarity::CppType storedValueType) {
        CLNodeFactory cpy(*this);
        cpy.storedValueType_ = storedValueType;
        return cpy;
    }

    inline CLNodeFactory withStoredValue(V *storedValue) {
        CLNodeFactory cpy(*this);
        cpy.storedValue_ = storedValue;
        return cpy;
    }

    inline CLNodeFactory withExistingDOMElement() {
        CLNodeFactory cpy(*this);
        cpy.useExistingDOMElement_ = true;
        return cpy;
    }

    inline ControlNetworkNode *button(const string &name, const string &text,
                                      val onPressCallback = val(NULL)) {
        ControlNetworkNode *button = withTag("button").build();
        button->setBoundField("textContent");
        button->setVal(val(text));
        val buttonCLE = button->getCLE();
        return button;
    }

    inline ControlNetworkNode *label(ControlNetworkNode *forNode,
                                     const string &text) {
        ControlNetworkNode *label = withTag("label").build();
        label->setBoundField("innerHTML");
        label->setVal(val(text));
        label->setAttribute("for", val(forNode->getId()));
        return label;
    }

    inline ControlNetworkNode *labelGivenNode(
        ControlNetworkNode *nodeToBeLabelled, const string &labelText) {
        ControlNetworkNode *outerDiv =
            withTag("div")
                .withName("labeldiv_" + nodeToBeLabelled->getName())
                .build();
        ControlNetworkNode *labelNode =
            withName("labelfor_" + nodeToBeLabelled->getName())
                .label(nodeToBeLabelled, labelText);
        outerDiv->appendChild(nodeToBeLabelled);
        outerDiv->appendChild(labelNode);
        return outerDiv;
    }

    inline ControlNetworkNode *attributeNode(const string &attributeName) {
        ControlNetworkNode *attributeNode =
            withBoundField(attributeName).build();
    }

    inline ControlNetworkNode *attributeNode(
        const string &attributeName, ControlNetworkNode *parent = nullptr) {
        ControlNetworkNode *attributeNode =
            withBoundField(attributeName).build();
    }

    //    parent_ = parent;
    //     boundField_ = attributeName;
    //     val parentDomelement = parent_->getCLE()["domElement"];
    //     cle_.set("domElement", parentDomelement);
};
}  // namespace clarity
#endif
