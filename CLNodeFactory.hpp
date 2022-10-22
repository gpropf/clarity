#ifndef CLNodeFactory_hpp
#define CLNodeFactory_hpp

#include "CanvasElement.hpp"
#include "ClarityNode.hpp"
#include "Selectables.hpp"

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
template <template <typename V> class Nc, typename V, typename N>
// template <class Nc, typename V, typename N>
class CLNodeFactory {
   public:
    string tag_;   //!< Tag to be used with elements this factory builds.
    string name_;  //!< Name to be used with elements this factory builds.
    bool nameIsForSingleUse_ = true;

    V *cppVal_ = nullptr;

    string innerHTML_ = "";

    string boundField_;  //!< Different types of elements need different fields
                         //!< to be modified when the node value changes.

    ClarityNode *parent_ = nullptr;  //!< If we have this set, we are creating
                                     //!< any new nodes as its children.

    Nc<V> *modelNode_ = nullptr;  //!< If we create a new MN or attach one, we set this. Note
                                  //!< that we can create ClarityNodes with no MN. These would store
                                  //!< their data entirely in their DOM elements.

    N linkMultiplierConstant_ = 1;  //!< By default we just transfer numeric
                                    //!< values from node to node unchanged.

    val a2b_xfmr_ = val(NULL);
    val b2a_xfmr_ = val(NULL);
    bool useExistingDOMElement_ = false;  //!< Primarily this is intended for creating attribute
                                          //!< nodes. The idea is that some nodes control attributes
                                          //!< of other nodes. The canonical example of this would
                                          //!< be color of an SVG object. In these cases we don't
                                          //!< want to create a new DOM element.

    map<string, val> attrs_;  //!< Sometimes a web element should be created with a bunch of
                              //!< attributes already set. That's what this is for. Even
                              //!< simple things like text input fields need to set the type
                              //!< attribute and doing just about anything with SVG requires
                              //!< a lot of attributes.

    /** \brief Tells how to attach the new node. Nodes can replace or attach to an existing DOM
     * node. We can also select the attachment point by either name or id.*/
    ClarityNode::AttachmentMode attachmentMode_ = ClarityNode::AttachmentMode::NEW;

    /** \brief String identifier (can be either the DOM name or Id) that will be used to place the
     * new node in the DOM.*/
    string attachmentId_ = "";

    /**
     * @brief Construct a new CLNodeFactory object from another of possibly different template
     * parameters. We cannot rely on the implicit copy constructor if we want to build a new CLNF
     * from one that may have different tparams.
     *
     * @tparam Nc_from
     * @tparam V_from
     * @tparam N_from
     * @param clnf_from
     */
    template <template <typename V_from> class Nc_from, typename V_from, typename N_from>
    CLNodeFactory(const CLNodeFactory<Nc_from, V_from, N_from> &clnf_from) {
        tag_ = clnf_from.tag_;
        name_ = clnf_from.name_;
        nameIsForSingleUse_ = clnf_from.nameIsForSingleUse_;
        innerHTML_ = clnf_from.innerHTML_;
        boundField_ = clnf_from.boundField_;
        parent_ = clnf_from.parent_;
        linkMultiplierConstant_ = clnf_from.linkMultiplierConstant_;
        // transformFn_ = clnf_from.transformFn_;
        a2b_xfmr_ = clnf_from.a2b_xfmr_;
        b2a_xfmr_ = clnf_from.b2a_xfmr_;
        useExistingDOMElement_ = clnf_from.useExistingDOMElement_;
        attrs_ = clnf_from.attrs_;

        modelNode_ = nullptr;
        cppVal_ = nullptr;
    }

    /**
     * @brief Construct a new CLNodeFactory object
     *
     */
    INLINE CLNodeFactory() {}

    /**
     * @brief Construct a new CLNodeFactory object
     *
     * @param tag valid HTML tag, e.g. input, div, button, etc...
     * @param name the string name to give the element
     *
     */
    INLINE CLNodeFactory(const string &tag, const string &name) : tag_(tag), name_(name) {}

    /**
     * @brief Construct a new CLNodeFactory object
     *
     * @param tag
     * @param name
     * @param storedValue If we use this, we are creating a corresponding MN to
     * hold the value.
     */
    INLINE CLNodeFactory(const string &tag, const string &name, V *storedValue)
        : tag_(tag), name_(name) {
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
    INLINE CLNodeFactory extractModelNode(Nc<V> *&modelNode) {
        modelNode = modelNode_;
        return *this;
    }

    /**
     * @brief Simple BR tag.
     *
     * @return Nc<V>*
     */
    INLINE Nc<V> *br() { return withTag("br").build(); }

    /**
     * @brief Simple HR tag.
     *
     * @return Nc<V>*
     */
    INLINE Nc<V> *hr() { return withTag("hr").build(); }

    /**
     * @brief The method that makes it all possible. Virtually all the other
     * methods use this at least once. The build method uses all the current
     * settings to construct a control element.
     *
     * @return Nc*
     */
    Nc<V> *build(Nc<V> *existingNode = nullptr) {
        Nc<V> *newNode;
        if (existingNode != nullptr) {
            newNode = existingNode;
        } else {
            newNode =
                new Nc<V>(name_, tag_, useExistingDOMElement_, attachmentMode_, attachmentId_);
        }

        if (innerHTML_ != "") {
            (newNode->getDomElement()).set("innerHTML", val(innerHTML_));
        }
        newNode->setBoundField(boundField_);
        newNode->setAttributes(attrs_);
        if (parent_) {
            parent_->appendChild(newNode);
        }
        if (modelNode_) {
            if (a2b_xfmr_ != val(NULL)) {
                modelNode_->addPeer(newNode, a2b_xfmr_, b2a_xfmr_);
            } else {
                modelNode_->addPeer(newNode, linkMultiplierConstant_);
            }
            if (!useExistingDOMElement_) modelNode_->pushValToPeers(modelNode_);
        }

        if (cppVal_) {
            newNode->setCppVal(cppVal_);
        }

        // newNode->refresh();
        if (nameIsForSingleUse_) name_ = "";
        return newNode;
    }

    INLINE CLNodeFactory withAttachmentId(const string &attachmentId_) const & {
        CLNodeFactory cpy(*this);
        cpy.attachmentId_ = attachmentId_;
        return cpy;
    }

    INLINE CLNodeFactory withAttachmentId(const string &attachmentId_) && {
        CLNodeFactory cpy(std::move(*this));
        cpy.attachmentId_ = attachmentId_;
        return cpy;
    }

    INLINE CLNodeFactory withAttachmentMode(ClarityNode::AttachmentMode attachmentMode) const & {
        CLNodeFactory cpy(*this);
        cpy.attachmentMode_ = attachmentMode;
        return cpy;
    }

    INLINE CLNodeFactory withAttachmentMode(ClarityNode::AttachmentMode attachmentMode) && {
        CLNodeFactory cpy(std::move(*this));
        cpy.attachmentMode_ = attachmentMode;
        return cpy;
    }

    /**
     * @brief Create the element with the listed attrs.
     *
     * @param attrs
     * @return CLNodeFactory
     */
    INLINE CLNodeFactory withAttributes(const map<string, val> &attrs) {
        CLNodeFactory cpy(*this);
        cpy.attrs_ = attrs;
        return cpy;
    }

    /**
     * @brief The "bound" field is the one that stores the data and reflects the
     * current state of the associated ModelNode.
     *
     * @param boundField
     * @return CLNodeFactory
     */
    INLINE CLNodeFactory withBoundField(const string &boundField) {
        assert(boundField != "");
        CLNodeFactory cpy(*this);
        cpy.boundField_ = boundField;
        // cout << "withBoundField:: boundField = " << boundField << "\n";
        return cpy;
    }

    /**
     * @brief Sets the factory to create all future nodes as children of the one
     * you give it. Very handy when fleshing out a piece of the GUI that is all
     * stored inside one div.
     *
     * @param parent
     * @return CLNodeFactory
     */
    INLINE CLNodeFactory createChildrenOf(Nc<V> *parent) {
        assert(parent != nullptr);
        CLNodeFactory cpy(*this);
        cpy.parent_ = parent;
        return cpy;
    }

    /**
     * @brief Sets the HTML tag to use when creating the node.
     *
     * @param tag
     * @return CLNodeFactory
     */
    INLINE CLNodeFactory withTag(const string &tag) const & {
        CLNodeFactory cpy(*this);
        cpy.tag_ = tag;
        return cpy;
    }

    INLINE CLNodeFactory withTag(const string &tag) && {
        CLNodeFactory cpy(std::move(*this));
        cpy.tag_ = tag;
        return cpy;
    }

    /**
     * @brief Sets the name of the node to create.
     *
     * @param name
     * @return CLNodeFactory
     */
    INLINE CLNodeFactory withName(const string &name) const & {
        CLNodeFactory cpy(*this);
        cpy.name_ = name;
        return cpy;
    }

    INLINE CLNodeFactory withName(const string &name) && {
        CLNodeFactory cpy(std::move(*this));
        cpy.name_ = name;
        return cpy;
    }

    /**
     * @brief Set the node's parent.
     *
     * @param parent Cannot be null.
     * @return CLNodeFactory
     */
    INLINE CLNodeFactory withParent(Nc<V> *parent) const & {
        assert(parent != nullptr);
        CLNodeFactory cpy(*this);
        cpy.parent_ = parent;
        return cpy;
    }

    INLINE CLNodeFactory withParent(Nc<V> *parent) && {
        assert(parent != nullptr);
        CLNodeFactory cpy(std::move(*this));
        cpy.parent_ = parent;
        return cpy;
    }

    INLINE CLNodeFactory withCppVal(V *cppVal) const & {
        // assert(cppVal != nullptr);
        CLNodeFactory cpy(*this);
        cpy.cppVal_ = cppVal;
        return cpy;
    }

    INLINE CLNodeFactory withCppVal(V *cppVal) const && {
        // assert(cppVal != nullptr);
        CLNodeFactory cpy(std::move(*this));
        cpy.cppVal_ = cppVal;
        return cpy;
    }

    /**
     * @brief In the case where a ModelNode already exists we can store it in
     * the factory and it will be linked to the built node as a peer in the
     * build() method. Note that this method and withStoredValue() are
     * incompatible.
     *
     * @param modelNode
     * @return CLNodeFactory
     */
    INLINE CLNodeFactory withModelNode(Nc<V> *modelNode) const & {
        assert(modelNode != nullptr);
        CLNodeFactory cpy(*this);
        cpy.modelNode_ = modelNode;
        return cpy;
    }

    INLINE CLNodeFactory withModelNode(Nc<V> *modelNode) && {
        assert(modelNode != nullptr);
        CLNodeFactory cpy(std::move(*this));
        cpy.modelNode_ = modelNode;
        return cpy;
    }

    /**
     * @brief A numeric constant that will be used as a multiplier when
     * transferring the value in this node to its associated ModelNode. The
     * inverse of this factor is applied for reverse data transfers.
     *
     * @param linkMultiplierConstant Cannot be 0.
     * @return CLNodeFactory
     */
    INLINE CLNodeFactory withLinkMultiplierConstant(N linkMultiplierConstant) const & {
        assert(linkMultiplierConstant != 0);
        CLNodeFactory cpy(*this);
        cpy.linkMultiplierConstant_ = linkMultiplierConstant;
        return cpy;
    }

    INLINE CLNodeFactory withLinkMultiplierConstant(N linkMultiplierConstant) && {
        assert(linkMultiplierConstant != 0);
        CLNodeFactory cpy(std::move(*this));
        cpy.linkMultiplierConstant_ = linkMultiplierConstant;
        return cpy;
    }

    /**
     * @brief It is sometimes necessary to apply a complex transform rather than
     * just a constant multiplier to a value when transferring it between nodes.
     * An example would be when translating a numeric value into an RGB color
     * value to make an object become redder as its temperature rises. These
     * JavaSCript transform functions can be stored to apply these
     * transformations. There are two to represent the possible transfer of
     * values in both directions. The reverse (b2a) transform can be null since
     * many uses of this feature will involve setting attributes from values and
     * attributes are generally not capable of accepting input.
     *
     * @param a2b_xfmr
     * @param b2a_xfmr
     * @return CLNodeFactory
     */
    INLINE CLNodeFactory withTransformFns(val a2b_xfmr, val b2a_xfmr = val(NULL)) const & {
        CLNodeFactory cpy(*this);
        cpy.a2b_xfmr_ = a2b_xfmr;
        cpy.b2a_xfmr_ = b2a_xfmr;
        return cpy;
    }

    INLINE CLNodeFactory withTransformFns(val a2b_xfmr, val b2a_xfmr = val(NULL)) && {
        CLNodeFactory cpy(std::move(*this));
        cpy.a2b_xfmr_ = a2b_xfmr;
        cpy.b2a_xfmr_ = b2a_xfmr;
        return cpy;
    }

    /**
     * @brief Primarily meant to be used when creating an attribute node since
     * the DOM element already exists. I also plan to allow attaching nodes to
     * existing DOM elements in a static web page.
     *
     * @return CLNodeFactory
     */
    INLINE CLNodeFactory withExistingDOMElement() const & {
        CLNodeFactory cpy(*this);
        cpy.useExistingDOMElement_ = true;
        return cpy;
    }

    INLINE CLNodeFactory withExistingDOMElement() && {
        CLNodeFactory cpy(std::move(*this));
        cpy.useExistingDOMElement_ = true;
        return cpy;
    }

    /**
     * @brief Put some text or raw html in a node.
     *
     * @param innerHTML
     * @return INLINE
     */
    INLINE CLNodeFactory withInnerHTML(const string &innerHTML) & {
        CLNodeFactory cpy(*this);
        cpy.innerHTML_ = innerHTML;
        return cpy;
    }

    INLINE CLNodeFactory withInnerHTML(const string &innerHTML) && {
        CLNodeFactory cpy(std::move(*this));
        cpy.innerHTML_ = innerHTML;
        return cpy;
    }

    /**
     * @brief A button.
     *
     * @param name
     * @param text
     * @param onPressCallback JS function to run when button is pressed.
     * @return Nc*
     */
    INLINE Nc<V> *button(const string &name, const string &text, val onPressCallback = val(NULL)) {
        Nc<V> *button = withTag("button").build();
        button->setBoundField("textContent");
        button->setVal(val(text));
        val buttonDOMElement = button->getCLE()["domElement"];
        buttonDOMElement.call<void>("addEventListener", val("click"), onPressCallback);
        return button;
    }

    INLINE Nc<V> *group(const vector<ClarityNode *> &nodes) {
        Nc<V> *group = withTag("div").build();
        for (auto node : nodes) {
            group->appendChild(node);
        }
        return group;
    }

    /**
     * @brief
     *
     * @tparam Nc_any This template param exists because the label value
     * (probably a string) does not need to be the same as the thing being
     * labelled.
     * @param forNode
     * @param text
     * @return Nc*
     */
    template <class Nc_any>
    INLINE Nc<V> *label(Nc_any *forNode, const string &text) {
        Nc<V> *label = withTag("label").build();
        label->setBoundField("innerHTML");
        label->setVal(val(text));
        label->setAttribute("for", val(forNode->getId()));
        return label;
    }

    /**
     * @brief A simple text input field.
     *
     * @return Nc*
     */
    INLINE Nc<V> *textInput() {
        map<string, val> inputFieldAttrs = {{"type", val("text")}};
        attrs_.merge(inputFieldAttrs);

        Nc<V> *inp = withTag("input").withBoundField("value").build();
        // inp->refreshDOMValueFromModel();
        // inp->pushValToPeers(inp);
        inp->refresh();
        return inp;
    }

    /**
     * @brief A range (slider) control.
     *
     * @return Nc*
     */
    INLINE Nc<V> *rangeInput(int min = 0, int max = 100) {
        map<string, val> inputFieldAttrs = {
            {"type", val("range")}, {"min", val(min)}, {"max", val(max)}};
        attrs_.merge(inputFieldAttrs);
        Nc<V> *inp = withTag("input").withBoundField("value").build();
        // inp->refreshDOMValueFromModel();
        // inp->pushValToPeers(inp);
        inp->refresh();
        return inp;
    }

    /**
     * @brief Text/Range input control. Creates a text field  and corresponding
     * range (slider) input that both control the same value.
     *
     * @return Nc*
     */
    INLINE Nc<V> *trInput() {
        Nc<V> *tinp = withName("tinp_" + name_).textInput();
        Nc<V> *rinp = withCppVal(nullptr).withName("rinp_" + name_).rangeInput();
        tinp->addPeer(rinp, linkMultiplierConstant_);
        Nc<V> *outerDiv = withTag("div").withName("wrapper_" + name_).build();
        outerDiv->appendChild(tinp);
        outerDiv->appendChild(rinp);
        tinp->refresh();
        return outerDiv;
    }

    INLINE Nc<V> *textarea(string *txt, const int rows = 4, const int cols = 50) {
        map<string, val> attrs = {{"rows", val(rows)}, {"cols", val(cols)}};
        Nc<V> *textArea = withTag("textarea")
                              .withBoundField("value")
                              .withAttributes(attrs)
                              .withCppVal(txt)
                              .build();
        textArea->refresh();
        return textArea;
    }

    /**
     * @brief Creates a label for any node you give it and encapsulates the
     * given node and the label in an enclosing div.
     *
     * @param nodeToBeLabelled
     * @param labelText
     * @return Nc*
     */
    INLINE Nc<V> *labelGivenNode(Nc<V> *nodeToBeLabelled, const string &labelText) {
        Nc<V> *outerDiv =
            withTag("div").withName("labeldiv_" + nodeToBeLabelled->getName()).build();
        Nc<V> *labelNode =
            withName("labelfor_" + nodeToBeLabelled->getName()).label(nodeToBeLabelled, labelText);
        outerDiv->appendChild(nodeToBeLabelled);
        outerDiv->appendChild(labelNode);
        return outerDiv;
    }

    /**
     * @brief Creates a JS canvas element with a simple test pattern.
     *
     * @return Nc*
     */
    INLINE CanvasElement<V> *canvas() {
        CanvasElement<V> *cel = static_cast<CanvasElement<V> *>(withTag("canvas").build());
        cel->setDrawFuntionName("canvasTestPattern");
        cel->refreshView();
        return cel;
    }

    INLINE CanvasGrid<V> *canvasGrid(int gridWidth, int gridHeight, int pixelWidth, int pixelHeight,
                                     const string &existingDOMId = "") {
        CanvasGrid<V> *cg =
            new CanvasGrid<V>(name_, "canvas", useExistingDOMElement_, attachmentMode_, gridWidth,
                              gridHeight, pixelWidth, pixelHeight, existingDOMId);
        cg = static_cast<CanvasGrid<V> *>(build(cg));
        cg->initcg();
        return cg;
    }

    INLINE Select<V> *select() {
        return withTag("select").build();
    }



    /**
     * @brief Attribute nodes are a special case. They represent a single
     * attribute of another node and thus do not have their own DOM element.
     *
     * @param attributeName
     * @return Nc*
     */
    INLINE Nc<V> *attributeNode(const string &attributeName) {
        Nc<V> *attributeNode = withExistingDOMElement().withBoundField(attributeName).build();
        val parentDomelement = parent_->getCLE()["domElement"];
        attributeNode->getCLE().set("domElement", parentDomelement);
        modelNode_->pushValToPeers(modelNode_);
        return attributeNode;
    }

    /**
     * @brief A variant of attributeNode method that sets the parent node and
     * then calls the single argument form.
     *
     * @param attributeName
     * @param parent
     * @return Nc*
     */
    INLINE Nc<V> *attributeNode(const string &attributeName, Nc<V> *parent) {
        Nc<V> *attributeNode = withParent(parent).attributeNode(attributeName);
        return attributeNode;
    }
};

}  // namespace clarity

#endif
