#ifndef CLNodeFactory_hpp
#define CLNodeFactory_hpp

#include "ModelNode.hpp"

namespace clarity {

/**
 * @brief This class offers a set of pure functions that can be chained to
 * create complex web controls. Essentially, instead of being a typical
 * 'factory' class that just emits objects in a single method call, CLNF can act
 * like an assembly line, creating basic web controls and then building more
 * complex ones from those. It replaces a whole complex class heirarchy that
 * worked the same but was an inflexible mess.
 *
 * @tparam Nc Node Class: Tells you what kind of Node this factory builds.
 * @tparam V Value: The type of C++ data the nodes will hold.
 * @tparam N Numeric Type: When the V type is a number of some kind, this tells
 * us what type is used for linkMultiplierConstant values. Technically V and N
 * should be the same thing but we get problems when V is a string. The fact
 * that there are two different tparams for this probably means we need a bit of
 * a refactoring.
 */
template <class Nc, typename V, typename N>
class CLNodeFactory {
   public:
    string tag_;   //!< Tag to be used with elements this factory builds.
    string name_;  //!< Name to be used with elements this factory builds.
    CppType storedValueType_;  //!< storedValueType to be used with elements
                               //!< this factory builds.

    string boundField_;  //!< Different types of elements need different fields
                         //!< to be modified when the node value changes.
    ClarityNode *parent_ = nullptr;  //!< If we have this set, we are creating
                                     //!< any new nodes as its children.
    ModelNode<V> *modelNode_ =
        nullptr;  //!< If we create a new MN or attach one, we set this. Note
                  //!< that we can create ControlNetworkNodes with no MN.

    N linkMultiplierConstant_ = 1;  //!< By default we just transfer numeric
                                    //!< values from node to node unchanged.
    val transformFn_ = val(NULL);   //!< See the docs on the ActiveLink class.
    val a2b_xfmr_ = val(NULL);
    val b2a_xfmr_ = val(NULL);
    bool useExistingDOMElement_ =
        false;  //!< Primarily this is intended for creating attribute nodes.
                //!< The idea is that some nodes control attributes of other
                //!< nodes. The canonical example of this would be color of an
                //!< SVG object. In these cases we don't want to create a new
                //!< DOM element.

    map<string, val>
        attrs_;  //!< Sometimes a web element should be created with a bunch of
                 //!< attributes already set. That's what this is for. Even
                 //!< simple things like text input fields need to set the type
                 //!< attribute and doing just about anything with SVG requires
                 //!< a lot of attributes.

    /**
     * @brief This is a bit of a kludge to allow us to switch the parameter
     * values 'midstream' while building a GUI. The idea is to preserve the work
     * you've already done, for instance in setting up a builder that produces
     * only children of a certain node. You might want to preserve most of the
     * settings in such a builder while changing the type of data or class of
     * nodes it produces. You feed in a 'from' factory and get back your 'to'
     * factory with all the non-tparam values copied over.
     *
     * @tparam Nc_from Node class of the 'from' factory.
     * @tparam V_from V for 'from' factory.
     * @tparam N_from N for 'from' factory.
     * @tparam Nc_to Node class of the 'to' factory.
     * @tparam V_to V for 'to' factory.
     * @tparam N_to N for 'from' factory.
     * @param clnf_from 'from' factory
     * @param clnf_to 'to' factory
     * @return CLNodeFactory
     */
    template <class Nc_from, typename V_from, typename N_from, class Nc_to,
              typename V_to, typename N_to>
    static CLNodeFactory clone(
        const CLNodeFactory<Nc_from, V_from, N_from> &clnf_from,
        CLNodeFactory<Nc_to, V_to, N_to> &clnf_to) {
        clnf_to.tag_ = clnf_from.tag_;
        clnf_to.name_ = clnf_from.name_;
        clnf_to.storedValueType_ = clnf_from.storedValueType_;
        clnf_to.boundField_ = clnf_from.boundField_;
        clnf_to.parent_ = clnf_from.parent_;
        clnf_to.linkMultiplierConstant_ = clnf_from.linkMultiplierConstant_;
        clnf_to.transformFn_ = clnf_from.transformFn_;
        clnf_to.a2b_xfmr_ = clnf_from.a2b_xfmr_;
        clnf_to.b2a_xfmr_ = clnf_from.b2a_xfmr_;
        clnf_to.useExistingDOMElement_ = clnf_from.useExistingDOMElement_;
        clnf_to.attrs_ = clnf_from.attrs_;
        return clnf_to;
    }

    /**
     * @brief Construct a new CLNodeFactory object
     *
     */
    inline CLNodeFactory() {}

    /**
     * @brief Construct a new CLNodeFactory object
     *
     * @param tag valid HTML tag, e.g. input, div, button, etc...
     * @param name the string name to give the element
     * @param storedValueType The type the corresponding stored data has in
     * C++. This is a far smaller set of types than actually exist in C++
     * but is meant to make basic distinctions between, say, a string and a
     * float value.
     */
    inline CLNodeFactory(const string &tag, const string &name,
                         CppType storedValueType)
        : tag_(tag), name_(name), storedValueType_(storedValueType) {}

    // template <class Nc_out, class Nc_in, typename V_out, typename V_in>
    // static CLNodeFactory<Nc_out, V_out> morph(
    //     const CLNodeFactory<Nc_in, V_in> &clnf) {
    //     return CLNodeFactory<Nc_out, V_out>(clnf);
    // }

    /**
     * @brief Construct a new CLNodeFactory object
     *
     * @param tag
     * @param name
     * @param storedValueType
     * @param storedValue If we use this, we are creating a corresponding MN to
     * hold the value.
     */
    inline CLNodeFactory(const string &tag, const string &name,
                         CppType storedValueType, V *storedValue)
        : tag_(tag), name_(name), storedValueType_(storedValueType) {
        withStoredValue(storedValue, true);
    }

    /**
     * @brief This is something I'm trying out that might be a bit novel and
     * that I haven't tested well yet. The idea is that you can use mutable
     * arguments to methods to pull values "out the side" of a chain of pure
     * functions. Here, we use it to pull out the MN we create to store a value
     * for a control. This would be useful to create other controls that can
     * modify or be modified by that same value. An example of this would be
     * where you have a slider control as well as a text input field for the
     * same value.
     *
     * Initially this didn't work and I decided to move on and work on other
     * stuff. When I came back to it I realized that the problem was that I was
     * only changing the **copy** of the modelNode pointer that was passed into
     * the method. As a result the calling function's pointer never changed. So
     * now we do this odd thing where we modify a pointer through a reference
     * and this allows us to have access to the internally created modelNode in
     * the calling scope.
     *
     * In short, having a (non-const) pointer allows you to change the thing the
     * pointer points to - not the value of the pointer itself. You cannot make
     * it point somewhere else for instance. This is what I was initially trying
     * to do.
     *
     * @tparam T
     * @param modelNode
     * @return CLNodeFactory
     */
    template <typename T>
    inline CLNodeFactory extractModelNode(ModelNode<T> *&modelNode) {
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
            if (a2b_xfmr_ != val(NULL)) {
                modelNode_->addPeer2(newNode, a2b_xfmr_, b2a_xfmr_);
            } else {
                modelNode_->addPeer2(newNode, linkMultiplierConstant_);
            }
            if (!useExistingDOMElement_)
                modelNode_->pushValToPeers2(modelNode_);
        }
        return newNode;
    }

    /**
     * @brief Create the element with the listed attrs.
     *
     * @param attrs
     * @return CLNodeFactory
     */
    inline CLNodeFactory withAttributes(const map<string, val> &attrs) {
        CLNodeFactory cpy(*this);
        cpy.attrs_ = attrs;
        return cpy;
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
        ModelNode<V> *mn = new ModelNode<V>(storedValue, storedValueType_,
                                            "modelnode_for_" + this->name_);
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

    inline CLNodeFactory withLinkMultiplierConstant(N linkMultiplierConstant) {
        assert(linkMultiplierConstant != 0);
        CLNodeFactory cpy(*this);
        cpy.linkMultiplierConstant_ = linkMultiplierConstant;
        return cpy;
    }

    inline CLNodeFactory withTransformFns(val a2b_xfmr,
                                          val b2a_xfmr = val(NULL)) {
        CLNodeFactory cpy(*this);
        cpy.a2b_xfmr_ = a2b_xfmr;
        cpy.b2a_xfmr_ = b2a_xfmr;
        return cpy;
    }

    inline CLNodeFactory withExistingDOMElement() {
        CLNodeFactory cpy(*this);
        cpy.useExistingDOMElement_ = true;
        return cpy;
    }

    inline ClarityNode *button(const string &name, const string &text,
                               val onPressCallback = val(NULL)) {
        ClarityNode *button = withTag("button").build();
        button->setBoundField("textContent");
        button->setVal(val(text));
        val buttonDOMElement = button->getCLE()["domElement"];
        buttonDOMElement.call<void>("addEventListener", val("click"),
                                    onPressCallback);
        return button;
    }

    inline ClarityNode *label(ClarityNode *forNode, const string &text) {
        ClarityNode *label = withTag("label").build();
        label->setBoundField("innerHTML");
        label->setVal(val(text));
        label->setAttribute("for", val(forNode->getId()));
        return label;
    }

    inline ClarityNode *textInput() {
        map<string, val> inputFieldAttrs = {{"type", val("text")}};
        ClarityNode *inp = withTag("input")
                               .withBoundField("value")
                               .withAttributes(inputFieldAttrs)
                               .build();
        return inp;
    }

    inline ClarityNode *rangeInput() {
        map<string, val> inputFieldAttrs = {{"type", val("range")}};
        ClarityNode *inp = withTag("input")
                               .withBoundField("value")
                               .withAttributes(inputFieldAttrs)
                               .build();
        return inp;
    }

    inline ClarityNode *trInput() {
        map<string, val> inputFieldAttrs = {{"type", val("text")}};
        ClarityNode *tinp = withTag("input")
                                .withBoundField("value")
                                .withAttributes(inputFieldAttrs)
                                .build();

        inputFieldAttrs["type"] = val("range");
        // inputFieldAttrs.find("type") = "range";
        ClarityNode *rinp = withTag("input")
                                .withBoundField("value")
                                .withAttributes(inputFieldAttrs)
                                .build();
        ClarityNode *outerDiv = withTag("div").withName("tr_" + name_).build();
        outerDiv->appendChild(tinp);
        outerDiv->appendChild(rinp);
        return outerDiv;
    }

    inline ClarityNode *labelGivenNode(ClarityNode *nodeToBeLabelled,
                                       const string &labelText) {
        ClarityNode *outerDiv =
            withTag("div")
                .withName("labeldiv_" + nodeToBeLabelled->getName())
                .build();
        ClarityNode *labelNode =
            withName("labelfor_" + nodeToBeLabelled->getName())
                .label(nodeToBeLabelled, labelText);
        outerDiv->appendChild(nodeToBeLabelled);
        outerDiv->appendChild(labelNode);
        return outerDiv;
    }

    inline ClarityNode *labelledTRInputNode(const string &labelText) {
        ClarityNode *trInputNode = trInput();
        return labelGivenNode(trInputNode, labelText);
    }

    inline ClarityNode *attributeNode(const string &attributeName) {
        ClarityNode *attributeNode =
            withExistingDOMElement().withBoundField(attributeName).build();
        val parentDomelement = parent_->getCLE()["domElement"];
        attributeNode->getCLE().set("domElement", parentDomelement);
        modelNode_->pushValToPeers2(modelNode_);
        return attributeNode;
    }

    inline ClarityNode *attributeNode(const string &attributeName,
                                      ClarityNode *parent) {
        ClarityNode *attributeNode =
            withParent(parent).attributeNode(attributeName);
        return attributeNode;
    }
};

}  // namespace clarity

#endif
