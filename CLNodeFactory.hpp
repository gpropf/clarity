#ifndef CLNodeFactory_hpp
#define CLNodeFactory_hpp

#include "ModelNode.hpp"

namespace clarity {
template <class Nc, typename V>
class CLNodeFactory {
   public:
    string tag_;   //!< Tag to be used with elements this factory builds.
    string name_;  //!< Name to be used with elements this factory builds.
    CppType storedValueType_;  //!< storedValueType to be used with elements
                               //!< this factory builds.
    // V *storedValue_ = nullptr;  //!< Actually only used when creating a model
    //                             //!< node along with a web control.

    string boundField_;
    ControlNetworkNode *parent_ = nullptr;
    ModelNode<V> *modelNode_ =
        nullptr;  //!< If we create a new MN or attach one, we set this. Note
                  //!< that we can create ControlNetworkNodes with no MN.

    V linkMultiplierConstant_ = 1;
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
        : tag_(tag), name_(name), storedValueType_(storedValueType) {
        withStoredValue(storedValue, true);
    }

    template <typename T>
    inline CLNodeFactory extractModelNode(ModelNode<T> *modelNode) {
        modelNode = modelNode_;
        return *this;
    }

    inline Nc *build() {
        Nc *newNode =
            new Nc(name_, tag_, storedValueType_, useExistingDOMElement_);
        newNode->setBoundField(boundField_);
        newNode->setAttributes(attrs_);
        if (parent_) {
            parent_->appendChild(newNode);
        }
        if (modelNode_) {
            // val transformFn = val(1);

            modelNode_->addPeer(ControlNetworkNode::ActiveLink(newNode));
            modelNode_->pushValToPeers(modelNode_);
        }
        return newNode;
    }

    inline CLNodeFactory withBoundField(const string &boundField) {
        assert(boundField != "");
        CLNodeFactory cpy(*this);
        cpy.boundField_ = boundField;
        cout << "withBoundField:: boundField = " << boundField << "\n";
        return cpy;
    }

    inline CLNodeFactory createChildrenOf(Nc *parent) {
        assert(parent != nullptr);
        CLNodeFactory cpy(*this);
        cpy.parent_ = parent;
        return cpy;
    }

    inline CLNodeFactory createAttributesOf(Nc *parent) {
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

    inline CLNodeFactory withParent(Nc *parent) {
        assert(parent != nullptr);
        CLNodeFactory cpy(*this);
        cpy.parent_ = parent;
        return cpy;
    }

    inline CLNodeFactory withStoredValueType(clarity::CppType storedValueType) {
        CLNodeFactory cpy(*this);
        cpy.storedValueType_ = storedValueType;
        if (cpy.modelNode_) {
            cpy.modelNode_->setStoredValueType(storedValueType);
        }

        // In case we already created a MN, we need to double back and set the
        // type in it.
        return cpy;
    }

    inline CLNodeFactory withStoredValue(V *storedValue, bool mutate = false) {
        assert(storedValue != nullptr);
        ModelNode<V> *mn = new ModelNode<V>(storedValue, storedValueType_);
        if (mutate) {
            this->modelNode_ = mn;
            return *this;
        }
        CLNodeFactory cpy(*this);
        cpy.modelNode_ = mn;
        return cpy;
    }

    inline CLNodeFactory withModelNode(ModelNode<V> *modelNode) {
        assert(modelNode != nullptr);
        CLNodeFactory cpy(*this);
        cpy.modelNode_ = modelNode;
        return cpy;
    }

    inline CLNodeFactory withLinkMultiplierConstant(V linkMultiplierConstant) {
        assert(linkMultiplierConstant != 0);
        CLNodeFactory cpy(*this);
        cpy.linkMultiplierConstant_ = linkMultiplierConstant;
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
        val buttonDOMElement = button->getCLE()["domElement"];
        buttonDOMElement.call<void>("addEventListener", val("click"),
                                    onPressCallback);
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
        val parentDomelement = parent_->getCLE()["domElement"];
        attributeNode->getCLE().set("domElement", parentDomelement);
        return attributeNode;
    }

    inline ControlNetworkNode *attributeNode(const string &attributeName,
                                             ControlNetworkNode *parent) {
        ControlNetworkNode *attributeNode =
            withParent(parent).attributeNode(attributeName);
        return attributeNode;
    }
};

}  // namespace clarity
#endif
