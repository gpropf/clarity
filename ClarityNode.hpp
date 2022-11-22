#ifndef ClarityNode_hpp
#define ClarityNode_hpp

#include <sstream>

#include "clarity.hpp"

namespace clarity {

/**
 * @brief My own stringification function once I saw that to_string() wasn't always working.
 *
 * @tparam T
 * @param v
 * @return std::string
 */
template <typename T>
std::string clto_str(const T &v) {
    std::ostringstream os;
    os << v;
    return os.str();
}

/**
 * @brief The project's central class. Describes an element with push/pull
 * behavior to synchronize a data model and a web view. WebElements can contain
 * others and complex web controls can be built up in a hierarchal fashion. The
 * class is not necessarily homologous to JavaScript Dom elements but the
 * "mirror" class JSProxyNode is currently used to store a JS Dom element and
 * retain other state on the JS side. This is also the base class for all other
 * types. ClarityNode is now an abstract class. The HybridNode<T> child class is
 * what we actually use to represent web elements.
 *
 */
class ClarityNode {
   public:
    static vector<string> jsAuxScripts__;
    // static vector<string> preloadJsAuxScripts__;

    static void addJSAuxScript(const string &jsFilename) { jsAuxScripts__.push_back(jsFilename); }

    static void runJSAuxScripts() {
        val loadAuxScript = val::global("loadAuxScript");
        for (string scriptName : jsAuxScripts__) {
            loadAuxScript(scriptName);
            // resultString = emscripten_run_script_string(scriptName.c_str());
        }
    }

    // static void addPreloadJSAuxScript(const string &jsFilename) {
    //     preloadJsAuxScripts__.push_back(jsFilename);
    // }

    // static void runPreloadJSAuxScripts() {
    //     val preloadAuxScript = val::global("preloadAuxScript");
    //     for (string scriptName : jsAuxScripts__) {
    //         preloadAuxScript(scriptName);
    //         // resultString = emscripten_run_script_string(scriptName.c_str());
    //     }
    // }

    static bool clogSilent;

    static void setClogSilent(bool silent = true) { clogSilent = silent; }

    static map<string, std::function<void()>> callbackMap;
    static val JSProxyNode_;

    enum class AttachmentMode : unsigned char {
        NEW = 0,
        REPLACE_ID = 1,
        REPLACE_NAME = 2,
        ATTACH_ID = 3,
        ATTACH_NAME = 4
    };

    /**
     * @brief Represents the 'edges' in our control graph. These edges can be
     * active and contain a JS value that can act as a transformation on the
     * values moving between nodes.
     *
     */
    class DualLink {
       public:
        enum class Directionality : char { L2R = 0, R2L = 1, UNIDIR = 2 };

        Directionality directionality_;
        ClarityNode *nodeA_;
        ClarityNode *nodeB_;
        // int intMultiplier = 1;
        // double dblMultiplier = 1.0;
        val a2b_xfmr_;
        val b2a_xfmr_;

        // static val JSProxyNode_;
        val get_a2b_xfmr() const;
        val get_b2a_xfmr() const;
        void set_a2b_xfmr(val xfmr);
        void set_b2a_xfmr(val xfmr);

        template <typename T>
        DualLink(ClarityNode *nodeA, ClarityNode *nodeB, const T multiplier = 1,
                 Directionality directionality = Directionality::UNIDIR)
            : nodeA_(nodeA), nodeB_(nodeB), directionality_(directionality) {
            a2b_xfmr_ = JSProxyNode_.call<val>("generateTransformFn", multiplier);
            b2a_xfmr_ = JSProxyNode_.call<val>("generateTransformFn", 1 / multiplier);
        };

        DualLink(ClarityNode *nodeA, ClarityNode *nodeB, val a2b_xfmr, val b2a_xfmr = val(NULL),
                 Directionality directionality = Directionality::UNIDIR)
            : nodeA_(nodeA),
              nodeB_(nodeB),
              a2b_xfmr_(a2b_xfmr),
              b2a_xfmr_(b2a_xfmr),
              directionality_(directionality) {}

        INLINE void printDL() {
            cout << "DL peer IDs: A = " << nodeA_->getId() << ", B = " << nodeB_->getId() << "\n";
        }

        /**
         * @brief A node will have a shared pointer to a DualLink if it is one of the ends of the
         * link. This method allows a node to quickly get a reference to the other end of the link.
         *
         * @param thisNode
         * @return INLINE
         */
        INLINE pair<ClarityNode *, val> getOtherNode(ClarityNode *thisNode) {
            if (nodeA_ == thisNode) {
                return pair(nodeB_, b2a_xfmr_);
            }
            return pair(nodeA_, a2b_xfmr_);
            // FIXME: probably should check to see if nodeA_ is actually the
            // same as thisNode and throw an exception if not.
        }
    };

    /**
     * @brief Version of DualLink that stores the multiplier in case intend to make the multiplier
     * itself a value that can be controlled in the page.
     *
     * @tparam T
     */
    template <typename T>
    class MultiplierLink : public DualLink {
        T *multiplier_;

       public:
        MultiplierLink(ClarityNode *nodeA, ClarityNode *nodeB, T *multiplier,
                       Directionality directionality = Directionality::UNIDIR)
            : DualLink(nodeA, nodeB, *multiplier, directionality) {
            multiplier_ = multiplier;
        }
    };

    virtual string cppValToString() const = 0;  //{ return "CN::cppValToString()"; }

    static void listNodes() {
        for (auto [id, node] : switchboard) {
            string name = node->getName();
            // string cppValStr;// = "NULL PTR2";
            string cppValStr = node->cppValToString();
            cout << "ID: " << id << ", cppVal_: " << cppValStr << ", <" << node->getTag()
                 << ">, name = '" << name << "'\n";
        }
    }

    void nodelog(const string &msg, ClogType clt = ClogType::INFO) const {
        if (ClarityNode::clogSilent) return;
        string msgout("node " + clto_str(id_) + ": " + msg);
        clog(msgout, clt);
    }

    static void nodelogStatic(const string &msg, ClogType clt = ClogType::INFO) {
        if (ClarityNode::clogSilent) return;
        // string msgout("node " + clto_str(id_)+ ":" + msg);
        clog(msg, clt);
    }

    static void addEventListenerGenerator(const string &nodeTypeCode, val handlerGenerator) {}

    /**
     * @brief Adds a special EL to this node. I don't think this form is actually in use.
     *
     * @param fn
     * @param eventName
     */
    void addEventListener(std::function<void(val ev)> fn, const string &eventName) {
        domElement_.call<void>("addEventListener", eventName, val(fn));
    }

    /**
     * @brief Adds a special EL to this node. Used in CLNF in particular to attach the click EL to
     * the buttons.
     *
     * @param elfn
     * @param eventName
     */
    void addEventListener(val elfn, const string &eventName) {
        domElement_.call<void>("addEventListener", eventName, elfn);
    }

    inline string getTag() const { return tag_; }
    inline int getId() const { return id_; }
    inline void setCLE(val cle) { cle_ = cle; }

    /**
     * @brief An early hack to allow users to attach their C++ code to buttons and things. Not sure
     * this actually saves work over just using embindings in your program.
     *
     * @param id
     */
    inline static void runCallbackById(const string &id) { callbackMap[id](); }

    // void EMSCRIPTEN_KEEPALIVE init();
    inline ClarityNode() { init(); }

    virtual ~ClarityNode() {
        // cout << "DESTROYING ClarityNode " << id_ << "\n";
        switchboard.erase(id_);
        for (auto dl : dlpeers_) {
            dl.reset();
        }
        dlpeers_.clear();
        for (auto child : children_) {
            delete child;
        }
        children_.clear();

        if (!domElement_.isUndefined()) domElement_.template call<void>("remove");
    }

    // INLINE ClarityNode(const CppType storedValueType)
    //     : storedValueType_(storedValueType) {
    //     init();
    //     cle_.set("cpptype", val(storedValueType));
    // }
    INLINE ClarityNode(const string &name) : name_(name) {
        init();
        // cle_.set("cpptype", val(storedValueType));
    }

    /**
     * @brief Construct a new Web Element object
     *
     * @param name String name of element, may be empty.
     * @param tag HTML tag of this element
     * @param storedValueType C++ type of data contained within
     *
     */
    ClarityNode(const string &name, const string &tag, bool useExistingDOMElement,
                AttachmentMode attachmentMode, const string &attachmentId = "")
        : name_(name), tag_(tag) {
        init();

        if (!useExistingDOMElement)
            cle_.call<void>("createDOMElement", id_, attachmentMode, attachmentId);

        domElement_ = cle_["domElement"];
        boundField_ = "value";
        ClarityNode::switchboard[id_] = this;
    }

    INLINE ClarityNode *getParent() const { return this->parent_; }
    INLINE void setParent(ClarityNode *parent) { this->parent_ = parent; }

    INLINE string getDOMText() const {
        string domText = domElement_[boundField_].as<string>();
        return domText;
    }

    /**
     * @brief Read the DOM element text and convert it to a JS variable using the cppVal_ type as a
     * type hint. Even if this particular node does not contain a value there it will still be able
     * to use the pointer as a type hint. The template specializations allow us to use functions
     * like stof, stod, and stoi to convert the text to the appropriate value type.
     *
     * @return val
     */
    virtual val getVal() const {
        string valueText = getDOMText();
        return val("Warning! No specialization to interpret meaning of DOM value!");
    }

    INLINE virtual void setDOMVal(const val &inval) {
        clean_ = false;
        if (boundField_ != "") {
            domElement_.set(boundField_, inval);
            domElement_.call<void>("setAttribute", val(boundField_), inval);
        } else {
            nodelog("Attempt to set DOM value of element with no bound field!", ClogType::WARNING);
        }
    }

    /**
     * @brief This method is called by the factory to complete the construction of a node. This is
     * where complex controls will create sub-nodes and carry out other initialization tasks. We
     * also use this method to install specialized event listener generators for certain classes. An
     * example of this use would be the SimpleSelect class. This is also where we install the
     * `clarityNode` member of the `JSProxyNode` contained in each node. It is this JavaScript
     * member that allows the JS code in the event listeners to "see" the C++ node and manipulate
     * it.
     *
     */
    inline virtual void finalize() = 0;

    virtual void doNothing() = 0;

    INLINE string getBoundField() const { return boundField_; }

    INLINE void setBoundField(const string &boundField) {
        boundField_ = boundField;
        cle_.set("boundField_", boundField);
    }

    /**
     * @brief Get the Node Type Code for use in installing the appropriate eventListeners on node
     * creation. I went with a short code instead of an enum to make expanding the types easier for
     * end users.
     *
     * Update: using finalize() and JSProxyNode.installEventListeners probably makes this
     * unnecessary except for debugging output.
     *
     * @return string
     */
    virtual string getNodeTypeCode() { return string("--"); };

    INLINE val getCLE() const { return cle_; }
    INLINE val getDomElement() { return domElement_; }
    INLINE void setDomElement(val domElement) { domElement_ = domElement; }
    INLINE string getName() const { return name_; }
    INLINE virtual void printState() const { cle_.call<void>("printState"); }
    INLINE static ClarityNode *getClarityNodeById(const int id) { return switchboard[id]; }
    // INLINE static void markNodeDirtyById(int id) { switchboard[id]->clean_ = false; }
    INLINE void toggleClean() { clean_ = !clean_; }

    template <typename T>
    INLINE static val cpp2js(void *valptr) {
        return val(*reinterpret_cast<T *>(valptr));
    }

    void addPeer(ClarityNode *peer, val a2b_xfmr, val b2a_xfmr) {
        auto dl = make_shared<DualLink>(this, peer, a2b_xfmr, b2a_xfmr);
        dlpeers_.push_back(dl);
        peer->appendDualLink(dl);
    }

    template <typename T>
    void addPeer(ClarityNode *peer, const T linkMultiplierConstant = 1) {
        auto dl = make_shared<DualLink>(this, peer, linkMultiplierConstant);
        dlpeers_.push_back(dl);
        peer->appendDualLink(dl);
    }

    template <typename T>
    void addPeer(ClarityNode *peer, T *linkMultiplierVariable) {
        auto dl = make_shared<MultiplierLink<T>>(this, peer, linkMultiplierVariable);
        dlpeers_.push_back(dl);
        peer->appendDualLink(dl);
    }

    INLINE void appendDualLink(shared_ptr<DualLink> dl) { dlpeers_.push_back(dl); }
    INLINE int countPeers() const { return dlpeers_.size(); }

    bool appendChild(ClarityNode *child) {
        children_.push_back(child);
        child->setParent(this);
        cle_.call<void>("appendChild", child->getCLE());
        return true;  // FIXME: need to check for duplicate ids.
    }

    INLINE string nodeStats(const string &msg) const {
        string s = "NS: Node name: " + name_ + ", Node id: " + to_string(id_) +
                   ", Node type: " + typeid(*this).name() +
                   ", peer count = " + to_string(countPeers()) + ". " + msg + "\n";
        return s;
    }

    static void pushValToPeersById(int id) {
        ClarityNode *cnn = getClarityNodeById(id);
        cnn->pushValToPeers(cnn);
    }

    /**
     * @brief Very central method in the data flow. When a user changes a GUI element, the
     * JavaScript listeners call this method on the node. The specific behavior depends on the
     * subclass of ClarityNode we are dealing with but the central feature of all overloads of this
     * method must be that the underlying C++ value is updated and the peers of the node receive the
     * updated value as well.
     *
     */
    virtual void updateNodeFromDom() = 0;

    static INLINE void updateNodeFromDomById(int id) {
        ClarityNode *cnn = getClarityNodeById(id);
        cnn->updateNodeFromDom();
        // cnn->pushValToPeers(cnn);
    }

    INLINE void updateNodeFromModel() {}

    INLINE void init() {
        id_ = tm.getNext();
        ClarityNode::switchboard[id_] = this;
    }

    /**
     * @brief Allows the API user to install his own LGs with varying specificity as to tag, type,
     * event name, etc...
     *
     * @param jsModuleName JSProxyNode, Selectables, etc...
     * @param nodeClassName HybridNode, SimpleSelect, CanvasGrid, etc...
     * @param newListenerGenerators can be a single LG as a "val" object or an entire set of LGs,
     * the presence of null values in the parameters determines how this param is used.
     * @param tag HTML tag
     * @param tagType "type" attribute for the HTML tag.
     * @param eventName JS event name: "mousedown", "click", etc...
     * @return void
     */
    static INLINE void installListenerGenerators(const string &jsModuleName,
                                                 const string &nodeClassName,
                                                 val newListenerGenerators, const string &tag = "",
                                                 string tagType = string(""),
                                                 const string &eventName = "") {
        assert(nodeClassName != "");
        if (tag == "") {
            val::global(jsModuleName.c_str()).set("listenerGenerators", newListenerGenerators);
            return;
        }

        // Since the "type" attribute seems to be just for the <input> tag, we use the explicit
        // value "NOTYPE" as the type for anything where type is left blank.
        if (tagType == "") tagType = string("NOTYPE");

        val lgmap = val::global(
            jsModuleName.c_str())["listenerGenerators"][nodeClassName.c_str()][tag.c_str()];

        // If there's no key for the tagType we create a placeholder value. This is mainly so
        // that listeners for "NOTYPE" can be installed.
        if (lgmap[tagType.c_str()] == val::null())
            val::global(
                jsModuleName.c_str())["listenerGenerators"][nodeClassName.c_str()][tag.c_str()]
                .set(tagType.c_str(), val(1));

        // If there's no eventName we assume that we are replacing the entire set of ELGs for a
        // given tag.
        if (eventName == "") {
            val::global(
                jsModuleName.c_str())["listenerGenerators"][nodeClassName.c_str()][tag.c_str()]
                .set(tagType.c_str(), newListenerGenerators);
        } else {
            val::global(jsModuleName.c_str())["listenerGenerators"][nodeClassName.c_str()]
                                             [tag.c_str()][tagType.c_str()]
                                                 .set(eventName.c_str(), newListenerGenerators);
        }
    }

    /**
     * @brief Use the provided DualLink to send data to the peers of this node.
     *
     * @param dl
     */
    virtual void pushValToPeer(DualLink &dl) {
        val internalVal = getVal();

        auto [peer, xfmr] = dl.getOtherNode(this);
        if (internalVal.isNumber()) {
            val transformedVal = JSProxyNode_.call<val>("applyTransformFn", xfmr, internalVal);
            peer->setDOMVal(transformedVal);
        } else {
            peer->setDOMVal(internalVal);
        }
    }

    virtual void pushValToPeers(ClarityNode *excludedPeer) {
        if (clean_) {
            return;
        }

        clean_ = true;
        if (excludedPeer == nullptr) {
            for (auto dl : dlpeers_) {
                auto [peer, xfmr] = dl->getOtherNode(this);
                pushValToPeer(*dl);
                peer->pushValToPeers(this);
            }
        } else {
            for (auto dl : dlpeers_) {
                auto [peer, xfmr] = dl->getOtherNode(this);
                if (peer != excludedPeer) {
                    pushValToPeer(*dl);
                    peer->pushValToPeers(this);
                }
            }
        }
        clean_ = false;
    }

    virtual void refresh() = 0;
    virtual void refreshDOMValueFromModel() = 0;

    INLINE void setAttribute(const string &attr, const val &value) {
        domElement_.call<void>("setAttribute", attr, value);
    }

    INLINE void setAttributes(const map<string, val> &attrs) {
        for (auto [attrName, value] : attrs) {
            setAttribute(attrName, value);
        }
    }

   protected:
    string tag_;
    string boundField_;

    vector<ClarityNode *> children_;

    /** \brief The node is clean if it has not been recently changed. This
       feature is mainly designed to prevent infinite update loops if the node
       graph is not acyclic. It doesn't do anything yet.*/
    bool clean_ = false;

    /** \brief Hands out the numeric ids for all nodes.*/
    static TicketMachine tm;

    /** \brief Keeps track of all nodes in the system. If you have the id of a
     * node you can get a pointer to it here. */
    static map<const int, ClarityNode *> switchboard;

    /** \brief Instance of the JSProxyNode class that acts as a "proxy" in JS
     * space. */
    val cle_ = val::global("JSProxyNode").new_();

    val domElement_;  //!< This will be initialized if the node has its own DOM
                      //!< element.

    /** \brief A node's parent is the DOM element that contains it. In the
     * case of the WebAttrNode this is the WebElemNode for which this is an
     * attribute. */
    ClarityNode *parent_;

    int id_ = -1;  //!< Unique identifier - needs to be immutable once set.

    string name_;  //!< Human readable name of node. Mutable, not required.

    /** \brief List of peers linked through JS functions which are applied when
     * data is moved between nodes. */
    // vector<ClarityNode::ActiveLink> peers_;
    vector<shared_ptr<ClarityNode::DualLink>> dlpeers_;

    // DEPRECATED BELOW THIS LINE ------------------------------
    /*

    The 'pull' methods are likely to never be needed now.

        void pullValFromPeer(DualLink &dl) {
            if (clean_) {
            }

            auto [peer, xfmr] = dl.getOtherNode(this);
            val internalVal = peer->getVal();
            if (internalVal.isNumber()) {
                val transformedVal = JSProxyNode_.call<val>("applyTransformFn", xfmr, internalVal);
                setDOMVal(transformedVal);
            } else {
                setDOMVal(internalVal);
            }

            clean_ = true;
        }

        void pullValFromPeers(ClarityNode *excludedPeer) {
            if (excludedPeer == nullptr) {
                for (auto dl : dlpeers_) {
                    pullValFromPeer(*dl);
                }
            } else {
                for (auto dl : dlpeers_) {
                    auto [peer, xfmr] = dl->getOtherNode(this);
                    if (peer != excludedPeer) {
                        pullValFromPeer(*dl);
                    }
                }
            }
        }

        static void pullValFromPeersById(int id) {
            ClarityNode *cnn = getClarityNodeById(id);
            cnn->pullValFromPeers(cnn);
        }
    */
};

/**
 * @brief Most nodes in the system are of this type. Pretty much any simple input control can be
 * created with this class and the various 'with' methods in CLNodeFactory.
 *
 * @tparam V
 */
template <typename V>
class HybridNode : public ClarityNode {
   public:
    HybridNode(const string &name, const string &tag, bool useExistingDOMElement,
               ClarityNode::AttachmentMode attachmentMode = ClarityNode::AttachmentMode::NEW,
               const string &attachmentId = "")
        : ClarityNode(name, tag, useExistingDOMElement, attachmentMode, attachmentId) {}

    inline virtual void doNothing() {
        cout << "This method exists so that Embind will create a type when this method in bound."
             << endl;
    }

    INLINE virtual void finalize() {
        // cout << "HybridNode::finalize()\n";
        this->cle_.set("clarityNode", this);
        // val Selectables = val::global("Selectables");
        val listenerGenerators = JSProxyNode_["listenerGenerators"];
        JSProxyNode_.call<void>("installEventListenersByTagAndType2", val("HybridNode"), this->cle_,
                                listenerGenerators);
    }

    INLINE void setCppVal(V *cppVal) { cppVal_ = cppVal; }
    INLINE V *getCppVal() const { return cppVal_; }

    INLINE virtual string getNodeTypeCode() { return string("HN"); }

    /**
     * @brief Set the Model value from the provided val object.
     *
     * @param jsval
     */
    void setCppValFromJSVal(const val &jsval) {
        V newCppVal = jsval.as<V>();
        *reinterpret_cast<V *>(cppVal_) = newCppVal;
        pushValToPeers(this);
    }

    /**
     * @brief Runs refreshDOMValueFromModel() and then syncs the peers with the updated value.
     * Called by CLNF as the final step in creating many of the nodes so that they are initialized
     * with data from the model when they first appear.
     *
     */
    virtual void refresh() {
        refreshDOMValueFromModel();
        pushValToPeers(this);
    }

    /**
     * @brief Calls ClarityNode::setDOMVal() first to initiate the push process if there are peer
     * nodes. If the cppVal_ pointer is non-null, we also then update that value in this node.
     *
     * @param inval
     */
    INLINE virtual void setDOMVal(const val &inval) {
        ClarityNode::setDOMVal(inval);
        if (cppVal_ != nullptr) {
            setCppValFromJSVal(inval);
        }
    }

    /**
     * @brief If this node has a non-null cppVal_ pointer we return the JS form of the value
     * therein. Otherwise we call the CN method that pulls the value from the DOM element.
     *
     * @return val
     */
    virtual val getVal() const {
        if (cppVal_ != nullptr) {
            return val(cpp2js<V>(cppVal_));
        }
        val domVal = ClarityNode::getVal();
        return domVal;
    }

    virtual string cppValToString() const {
        if (cppVal_ == nullptr) return "###";
        return string("HybridNode<V>::cppValToString() UNSPECIALIZED");
    }

    /**
     * @brief Updates the DOM value from the model value.
     *
     */
    virtual void refreshDOMValueFromModel() {
        if (cppVal_ != nullptr) {
            val jsval = val(*cppVal_);
            setDOMVal(jsval);
        }
    }

    virtual void updateNodeFromDom() {
        string methodStr = "HybridNode<V>::updateNodeFromDom()\n";
        methodStr = clarity::interpolateTypeIntoString<V>(methodStr);
        nodelog(methodStr);
        cout << methodStr << endl;
        val jsval = getVal();
        if (cppVal_ != nullptr) {
            cout << "cppVal_ exists!\n";
            *cppVal_ = jsval.as<V>();
        }
        pushValToPeers(this);
    }

    ~HybridNode() {  // cout << "DESTROYING HybridNode with id: " << id_ << "\n";
    }

   protected:
    V *cppVal_ = nullptr;  //!< The C++ data object that acts as the 'model'
};

}  // namespace clarity

#endif
