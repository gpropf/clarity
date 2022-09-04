#ifndef CLNodeFactory_hpp
#define CLNodeFactory_hpp

#include "ModelNode.hpp"

namespace clarity {
template <class T, typename V>
class CLNodeFactory {
   public:
    string tag_;   //!< Tag to be used with elements this factory builds.
    string name_;  //!< Name to be used with elements this factory builds.
    CppType storedValueType_;  //!< storedValueType to be used with elements
                               //!< this factory builds.
    V *storedValue_;  //!< Actually only used when creating a model node along
                      //!< with a web control.
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

    // inline T *build() { return new T(name_, tag_, storedValueType_); }

    // inline T *buildWithAttributes() {
    //     T *newNode = new T(name_, tag_,
    //     storedValueType_); newNode->setAttributes(attrs_); return newNode;
    // }

    inline T *build() {
        T *newNode = new T(name_, tag_, storedValueType_);
        newNode->setAttributes(attrs_);
        return newNode;
    }

    inline T *buildInsideNode(ControlNetworkNode *outerNode) {
        ControlNetworkNode *innerNode = build();
        outerNode->appendChild(innerNode);
        return outerNode;
    }

    inline T *buildWithModelNode(const val transformFn = val(1)) {
        T *cnn = build();
        ModelNode<V> *mn = new ModelNode<V>(storedValue_, storedValueType_);
        mn->addPeer(clarity::ControlNetworkNode::ActiveLink(cnn, transformFn));
        mn->pushValToPeers(mn);
        return cnn;
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

    inline T *button(const string &name, const string &text,
                               val onPressCallback = val(NULL)) {
        T *button = withTag("button").build();
        button->boundField_ = "textContent";
        button->setVal(val(text));
        val buttonCLE = button->getCLE();
        return button;
    }

    inline T *label(T *forNode, const string &text) {
        T *label = withTag("label").build();
        label->boundField_ = "innerHTML";
        label->setVal(val(text));
        label->setAttribute("for", val(forNode->getId()));
        return label;
    }

    inline T *labelGivenNode(T *nodeToBeLabelled,
                                       const string &labelText) {
        T *outerDiv =
            withTag("div")
                .withName("labeldiv_" + nodeToBeLabelled->getName())
                .build();
        T *labelNode =
            withName("labelfor_" + nodeToBeLabelled->getName())
                .label(nodeToBeLabelled, labelText);
        outerDiv->appendChild(nodeToBeLabelled);
        outerDiv->appendChild(labelNode);
        return outerDiv;
    }
};
}  // namespace clarity
#endif
