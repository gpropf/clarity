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
 * "mirror" class CLElement is currently used to store a JS Dom element and
 * retain other state on the JS side. This is also the base class for all other
 * types. ClarityNode is now an abstract class. The HybridNode<T> child class is
 * what we actually use to represent web elements.
 *
 */
class ClarityNode {
   public:
    static map<string, std::function<void()>> callbackMap;
    static val CLElement_;

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
        ClarityNode *nodeA_;
        ClarityNode *nodeB_;
        int intMultiplier = 1;
        double dblMultiplier = 1.0;
        val a2b_xfmr_;
        val b2a_xfmr_;

        // static val CLElement_;
        val get_a2b_xfmr() const;
        val get_b2a_xfmr() const;
        void set_a2b_xfmr(val xfmr);
        void set_b2a_xfmr(val xfmr);

        template <typename T>
        DualLink(ClarityNode *nodeA, ClarityNode *nodeB, const T multiplier = 1)
            : nodeA_(nodeA), nodeB_(nodeB) {
            a2b_xfmr_ = CLElement_.call<val>("generateTransformFn", multiplier);
            b2a_xfmr_ = CLElement_.call<val>("generateTransformFn", 1 / multiplier);
        };

        DualLink(ClarityNode *nodeA, ClarityNode *nodeB, val a2b_xfmr, val b2a_xfmr = val(NULL))
            : nodeA_(nodeA), nodeB_(nodeB), a2b_xfmr_(a2b_xfmr), b2a_xfmr_(b2a_xfmr) {}

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

    virtual string cppValToString() const { return "CN::cppValToString()"; }

    static void nodeAudit() {
        for (auto [id, node] : switchboard) {
            string name = node->getName();
            // string cppValStr;// = "NULL PTR2";
            string cppValStr = node->cppValToString();
            cout << "ID: " << id << ", cppVal_: " << cppValStr << ", <" << node->getTag()
                 << ">, name = '" << name << "'\n";
        }
    }

    static void addEventListenerGenerator(const string &nodeTypeCode, val handlerGenerator) {}

    INLINE string getTag() const { return tag_; }
    INLINE int getId() const { return id_; }
    INLINE void setCLE(val cle) { cle_ = cle; }

    INLINE static void runCallbackById(const string &id) { callbackMap[id](); }

    // void EMSCRIPTEN_KEEPALIVE init();
    INLINE ClarityNode() { init(); }

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
        val domElement = cle_["domElement"];
        if (!domElement.isUndefined()) cle_["domElement"].template call<void>("remove");
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
        val domElement = cle_["domElement"];
        string domText = domElement[boundField_].as<string>();
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
        return val("NO SPECIALIZED TEMPLATE");
    }

    INLINE virtual void setVal(const val &inval) {
        clean_ = false;
        val domElement = cle_["domElement"];
        // cle_.call<void>("printVal", inval);
        if (boundField_ != "") {
            domElement.set(boundField_, inval);
            domElement.call<void>("setAttribute", val(boundField_), inval);
        } else {
            // FIXME: Should be an assert or exception
        }
    }

    inline virtual void finalize() = 0;

    INLINE string getBoundField() const { return boundField_; }

    INLINE void setBoundField(const string &boundField) {
        boundField_ = boundField;
        cle_.set("boundField_", boundField);
        // val domElement = cle_["domElement"];
    }

    /**
     * @brief Get the Node Type Code for use in installing the appropriate eventListeners on node creation.
     * I went with a short code instead of an enum to make expanding the types easier for end users.
     * 
     * Update: using finalize() and CLElement.installEventListeners probably makes this unnecessary except for
     * debugging output.
     *
     * @return string
     */
    virtual string getNodeTypeCode() {return string("--");};

    INLINE val getCLE() const { return cle_; }
    INLINE val getDomElement() { return domElement_; }
    INLINE string getName() const { return name_; }
    INLINE virtual void printState() const { cle_.call<void>("printState"); }
    INLINE static ClarityNode *getCLElementById(const int id) { return switchboard[id]; }
    // INLINE static void markNodeDirtyById(int id) { switchboard[id]->clean_ = false; }
    INLINE void toggleClean() { clean_ = !clean_; }

    template <typename T>
    INLINE static val cpp2js(void *valptr) {
        return val(*reinterpret_cast<T *>(valptr));
    }

    // template <typename T>
    //  val cpp2js(void *valptr) {
    //     return val(*reinterpret_cast<T *>(valptr));
    // }

    // virtual string nodeStats(const string &msg = "") const;

    // const int *getDimensionality() const { return dataDimensionality_; };

    // virtual void pushValToPeer(ActiveLink &al, const string &tabs = "");
    // virtual void pushValToPeer(DualLink &al);

    // virtual void pushValToPeers(ClarityNode *excludedPeer = nullptr);
    // virtual void pushValToPeers(ClarityNode *excludedPeer = nullptr);
    // static void pushValToPeersById(int id);
    // static void pushValToPeersById(int id);
    // void addPeer(ClarityNode::ActiveLink al, bool alreadyAdded = false);
    // void pullValFromPeer(DualLink &dl);
    // void pullValFromPeers(ClarityNode *excludedPeer);
    // static void pullValFromPeersById(int id);

    template <typename T>
    void addPeer(ClarityNode *peer, const T linkMultiplierConstant = 1) {
        auto dl = make_shared<DualLink>(this, peer, linkMultiplierConstant);
        dlpeers_.push_back(dl);
        peer->appendDualLink(dl);
    }
    //   void addPeer2(ClarityNode *peer);

    // void addPeer(ClarityNode *peer, val a2b_xfmr, val b2a_xfmr = val(NULL));
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
        ClarityNode *cnn = getCLElementById(id);
        // cout << cnn->nodeStats("[pushValToPeersById]");
        cnn->pushValToPeers(cnn);
    }

    virtual void updateNodeFromDom() = 0;

    static INLINE void updateNodeFromDomById(int id) {
        ClarityNode *cnn = getCLElementById(id);
        cout << "updateNodeFromDomById for " << cnn->getNodeTypeCode() << "\n";
        cnn->updateNodeFromDom();
        // cout << cnn->nodeStats("[updateNodeFromDomById]");
        cnn->pushValToPeers(cnn);
    }

    INLINE void updateNodeFromModel() {}

    INLINE void init() {
        id_ = tm.getNext();
        ClarityNode::switchboard[id_] = this;
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
            val transformedVal = CLElement_.call<val>("applyTransformFn", xfmr, internalVal);
            peer->setVal(transformedVal);
        } else {
            peer->setVal(internalVal);
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
                peer->pushValToPeers(this);
                pushValToPeer(*dl);
            }
        } else {
            for (auto dl : dlpeers_) {
                auto [peer, xfmr] = dl->getOtherNode(this);
                if (peer != excludedPeer) {
                    peer->pushValToPeers(this);
                    pushValToPeer(*dl);
                }
            }
        }
        clean_ = false;
    }

    virtual void refreshDOMValueFromModel() = 0;
    // virtual void refresh() = 0;

    void pullValFromPeer(DualLink &dl) {
        if (clean_) {
        }

        auto [peer, xfmr] = dl.getOtherNode(this);
        val internalVal = peer->getVal();
        if (internalVal.isNumber()) {
            val transformedVal = CLElement_.call<val>("applyTransformFn", xfmr, internalVal);
            setVal(transformedVal);
        } else {
            setVal(internalVal);
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
        ClarityNode *cnn = getCLElementById(id);
        cnn->pullValFromPeers(cnn);
    }

    void addPeer(ClarityNode *peer, val a2b_xfmr, val b2a_xfmr) {
        auto dl = make_shared<DualLink>(this, peer, a2b_xfmr, b2a_xfmr);
        dlpeers_.push_back(dl);
        peer->appendDualLink(dl);
    }

    INLINE void setAttribute(const string &attr, const val &value) {
        val domElement = cle_["domElement"];
        domElement.call<void>("setAttribute", attr, value);
    }

    INLINE void setAttributes(const map<string, val> &attrs) {
        for (auto [attrName, value] : attrs) {
            setAttribute(attrName, value);
        }
    }    

    INLINE void addEventListenerByName(const string &eventName, const string &callbackName) {
        cle_.call<void>("addEventListenerById", eventName, callbackName);
    }

    INLINE void addJSEventListener(const string &eventName, val eventCallback) {
        cle_.call<void>("addEventListener", eventName, eventCallback);
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

    /** \brief Instance of the CLElement class that acts as a "proxy" in JS
     * space. */
    val cle_ = val::global("CLElement").new_();

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
};

template <typename V>
class HybridNode : public ClarityNode {
   public:
    HybridNode(const string &name, const string &tag, bool useExistingDOMElement,
               ClarityNode::AttachmentMode attachmentMode = ClarityNode::AttachmentMode::NEW,
               const string &attachmentId = "")
        : ClarityNode(name, tag, useExistingDOMElement, attachmentMode, attachmentId) {
        // cout << "FIVE ARG HN constructor called!: " << int(attachmentMode) << "\n";
        //cout << "type code: " << this->getNodeTypeCode() << "\n";
        //cle_.call<void>("generateEventHandlers", cle_);
        
    }

    inline virtual void finalize() {
        //cout << "ID: " << this->id_ << ", HybridNode: virtual void finalize()\n";
        cle_.call<void>("generateEventHandlers", cle_);
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
    void refresh() {
        refreshDOMValueFromModel();
        pushValToPeers(this);
    }

    /**
     * @brief Calls ClarityNode::setVal() first to initiate the push process if there are peer
     * nodes. If the cppVal_ pointer is non-null, we also then update that value in this node.
     *
     * @param inval
     */
    INLINE virtual void setVal(const val &inval) {
        ClarityNode::setVal(inval);
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
        // return clto_str<V>(*(reinterpret_cast<V *>(this->cppVal_)));
        // return clto_str<V>(*this->cppVal_);
        // return to_string(*this->cppVal_);
        return string("FIXME");
    }

    /**
     * @brief Updates the DOM value from the model value.
     *
     */
    virtual void refreshDOMValueFromModel() {
        if (cppVal_ != nullptr) {
            val jsval = val(*cppVal_);
            setVal(jsval);
        }
    }

    virtual void updateNodeFromDom() {
        cout << "************************************ ClarityNode::updateNodeFromDom()\n";
        val jsval = getVal();
        if (cppVal_ != nullptr) {
            cout << "cppVal_ exists!\n";
            *cppVal_ = jsval.as<V>();            
        }
    }

    ~HybridNode() {  // cout << "DESTROYING HybridNode with id: " << id_ << "\n";
    }

   protected:
    V *cppVal_ = nullptr;  //!< The C++ data object that acts as the 'model'
};

}  // namespace clarity

#endif
