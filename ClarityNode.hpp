#ifndef ClarityNode_hpp
#define ClarityNode_hpp

#include "clarity.hpp"

namespace clarity {

/**
 * @brief The project's central class. Describes an element with push/pull
 * behavior to syncronize a data model and a web view. WebElements can contain
 * others and complex web controls can be built up in a hierarchal fashion. The
 * class is not necessarily homologous to JavaScript Dom elements but the
 * "mirror" class CLElement is currently used to store a JS Dom element and
 * retain other state on the JS side. This is also the base class for all other
 * types. The CNN type should never be instantiated as it has no specific
 * behaviors and doesn't bind to anything on the webpage. In future it may
 * contain pure virtual methods and trying to instantiate it will be an error.
 *
 */

class ClarityNode {
   public:
    static map<string, std::function<void()>> callbackMap;

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

        static val CLElement_;
        val get_a2b_xfmr() const;
        val get_b2a_xfmr() const;
        void set_a2b_xfmr(val xfmr);
        void set_b2a_xfmr(val xfmr);

        template <typename T>
        DualLink(ClarityNode *nodeA, ClarityNode *nodeB, const T multiplier = 1) : nodeA_(nodeA), nodeB_(nodeB) {
            a2b_xfmr_ = CLElement_.call<val>("generateTransformFn", multiplier);
            b2a_xfmr_ = CLElement_.call<val>("generateTransformFn", 1 / multiplier);
        };

        DualLink(ClarityNode *nodeA, ClarityNode *nodeB, val a2b_xfmr, val b2a_xfmr = val(NULL))
            : nodeA_(nodeA), nodeB_(nodeB), a2b_xfmr_(a2b_xfmr), b2a_xfmr_(b2a_xfmr) {}

        inline void printDL() { cout << "DL peer IDs: A = " << nodeA_->getId() << ", B = " << nodeB_->getId() << "\n"; }

        inline pair<ClarityNode *, val> getOtherNode(ClarityNode *thisNode) {
            if (nodeA_ == thisNode) {
                return pair(nodeB_, b2a_xfmr_);
            }
            return pair(nodeA_, a2b_xfmr_);
            // FIXME: probably should check to see if nodeA_ is actually the
            // same as thisNode and throw an exception if not.
        }
    };

    // EMSCRIPTEN_KEEPALIVE void addEventListenerByName(
    //     const string &eventName, const string &callbackName);
    // EMSCRIPTEN_KEEPALIVE void addJSEventListener(const string &eventName,
    //                                              val eventCallback);

    // virtual val mn_getVal() const = 0;

    inline string getTag() const { return tag_; }
    inline int getId() const { return id_; }
    inline void setCLE(val cle) { cle_ = cle; }

    inline static void runCallbackById(const string &id) { callbackMap[id](); }

    // void EMSCRIPTEN_KEEPALIVE init();
    inline ClarityNode() { init(); }

    virtual ~ClarityNode() {
        cout << "DESTROYING ClarityNode " << id_ << "\n";
        for (auto dl : dlpeers_) {
            dl.reset();
        }
        dlpeers_.clear();
        val domElement = cle_["domElement"];
        if (!domElement.isUndefined()) cle_["domElement"].template call<void>("remove");
    }

    // inline ClarityNode(const CppType storedValueType)
    //     : storedValueType_(storedValueType) {
    //     init();
    //     cle_.set("cpptype", val(storedValueType));
    // }
    inline ClarityNode(const string &name) : name_(name) {
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
    ClarityNode(const string &name, const string &tag, bool useExistingDOMElement) : name_(name), tag_(tag) {
        init();
        if (!useExistingDOMElement) cle_.call<void>("createDOMElement", id_, tag, name);
        // cle_.set("name", val(name));
        //  For some reason the code that sets the name in clarity.js doesn't
        //  "take" so we re-set it here.

        domElement_ = cle_["domElement"];
        setBoundField("value");
        // boundField_ = "value";  // FIXME: Not true for all elements
        ClarityNode::switchboard[id_] = this;
    }

    // EMSCRIPTEN_KEEPALIVE void setAttribute(const string &attr,
    //                                        const val &value);

    // EMSCRIPTEN_KEEPALIVE void setAttributes(const map<string, val> &attrs);

    // inline CppType getStoredValueType() const { return storedValueType_; }
    //  EMSCRIPTEN_KEEPALIVE void setStoredValueType(CppType cppType);

    // bool appendChild(ClarityNode *child);

    inline ClarityNode *getParent() const { return this->parent_; }
    inline void setParent(ClarityNode *parent) { this->parent_ = parent; }

    template <typename V, typename C>
    inline V &getRawVal(const vector<C> loc) const {}

    // val stringToJSVal(string s) {
    //     return val(s);
    // }

    inline string getDOMText() const {
        val domElement = cle_["domElement"];
        string domText = domElement[boundField_].as<string>();
        return domText;
    }

    /**
     * @brief Read the DOM element text and convert it to a JS variable using the cppVal_ type as a type hint. Even if
     * this particular node does not contain a value there it will still be able to use the pointer as a type hint. The
     * template specializations allow us to use functions like stof, stod, and stoi to convert the text to the
     * appropriate value type.
     *
     * @return val
     */
    virtual val getVal() const {
        string valueText = getDOMText();
        return val("NO SPECIALIZED TEMPLATE");
    }

    template <typename V, typename C>
    inline void setRawVal(V &v, vector<C> loc) {}

    // inline virtual void setVal(const val &inval) { clean_ = false; }

    inline virtual void setVal(const val &inval) {
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

    inline void setBoundField(const string &boundField) {
        boundField_ = boundField;
        cle_.set("boundField_", boundField);
        // val domElement = cle_["domElement"];
    }

    inline val getCLE() const { return cle_; }
    inline string getName() const { return name_; }
    inline virtual void printState() const { cle_.call<void>("printState"); }
    inline static ClarityNode *getCLElementById(const int id) { return switchboard[id]; }
    inline static void markNodeDirtyById(int id) { switchboard[id]->clean_ = false; }
    inline void toggleClean() { clean_ = !clean_; }

    template <typename T>
    inline static val cpp2js(void *valptr) {
        return val(*reinterpret_cast<T *>(valptr));
    }

    // template <typename T>
    //  val cpp2js(void *valptr) {
    //     return val(*reinterpret_cast<T *>(valptr));
    // }

    // virtual string nodeStats(const string &msg = "") const;

    const int *getDimensionality() const { return dataDimensionality_; };

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
    inline void appendDualLink(shared_ptr<DualLink> dl) { dlpeers_.push_back(dl); }
    inline int countPeers() const { return dlpeers_.size(); }

    //=========== From cpp file

    bool appendChild(ClarityNode *child) {
        children_.push_back(child);
        child->setParent(this);
        cle_.call<void>("appendChild", child->getCLE());
        return true;  // FIXME: need to check for duplicate ids.
    }

    inline string nodeStats(const string &msg) const {
        string s = "NS: Node name: " + name_ + ", Node id: " + to_string(id_) + ", Node type: " + typeid(*this).name() +
                   ", peer count = " + to_string(countPeers()) + ". " + msg + "\n";
        return s;
    }

    static void pushValToPeersById(int id) {
        ClarityNode *cnn = getCLElementById(id);
        // cout << cnn->nodeStats("[pushValToPeersById]");
        cnn->pushValToPeers(cnn);
    }

    virtual void updateNodeFromDom() = 0;

    static inline void updateNodeFromDomById(int id) {
        ClarityNode *cnn = getCLElementById(id);
        cnn->updateNodeFromDom();
        // cout << cnn->nodeStats("[updateNodeFromDomById]");
        cnn->pushValToPeers(cnn);
    }

    inline void updateNodeFromModel() {}

    inline void init() {
        id_ = tm.getNext();
        // Set up all nodes as single valued by default.
        dataDimensionality_[0] = 1;
        dataDimensionality_[1] = 0;
        ClarityNode::switchboard[id_] = this;
    }

    /**
     * @brief The most critical way this is used is to move data to/from
     * ModelNodes. In moving data from a ModelNode the initial getVal call
     * occurs in a ModelNode and the setVal calls are in the corresponding
     * ClarityNodes.
     *
     * @param dl
     */
    virtual void pushValToPeer(DualLink &dl) {
        val internalVal = getVal();

        auto [peer, xfmr] = dl.getOtherNode(this);
        if (internalVal.isNumber()) {
            val transformedVal = ClarityNode::DualLink::CLElement_.call<val>("applyTransformFn", xfmr, internalVal);
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

    void pullValFromPeer(DualLink &dl) {
        if (clean_) {
        }

        auto [peer, xfmr] = dl.getOtherNode(this);
        val internalVal = peer->getVal();
        if (internalVal.isNumber()) {
            val transformedVal = ClarityNode::DualLink::CLElement_.call<val>("applyTransformFn", xfmr, internalVal);
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

    inline void setAttribute(const string &attr, const val &value) {
        val domElement = cle_["domElement"];
        domElement.call<void>("setAttribute", attr, value);
    }

    inline void setAttributes(const map<string, val> &attrs) {
        for (auto [attrName, value] : attrs) {
            setAttribute(attrName, value);
        }
    }

    // inline void setStoredValueType(CppType cppType) {
    //     storedValueType_ = cppType;
    //     cle_.set("cpptype", cppType);
    // }

    inline void addEventListenerByName(const string &eventName, const string &callbackName) {
        cle_.call<void>("addEventListenerById", eventName, callbackName);
    }

    inline void addJSEventListener(const string &eventName, val eventCallback) {
        cle_.call<void>("addEventListener", eventName, eventCallback);
    }
    // ========= End from Cpp file

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

    // CppType storedValueType_;  //!< C++ Data type
    int *dataDimensionality_ = new int[2];  //!< There is a digit for each dimension and the dimension
                                            //!< list is terminated with a 0. Single valued datums thus
                                            //!< have[1, 0]. A 2D grid that is 6 wide and 5
                                            //!< high will have[6, 5, 0] and so on.

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
    HybridNode(const string &name, const string &tag, bool useExistingDOMElement)
        : ClarityNode(name, tag, useExistingDOMElement) {}

    inline void setCppVal(V *cppVal) { cppVal_ = cppVal; }

    static void nodeAudit() {
        for (auto [id, node] : switchboard) {
            string name = node->getName();
            HybridNode<V> *hn = dynamic_cast<HybridNode<V> *>(node);
            string cppValStr = to_string(*(hn->cppVal_));
            if (hn->cppVal_ == nullptr) cppValStr = "NULL PTR";
            cout << "ID: " << id << ", cppVal_: " << cppValStr << ", " << name << "\n";
        }
    }

    void setCppValFromJSVal(const val &jsval) {        
        V newCppVal = jsval.as<V>();
        *reinterpret_cast<V *>(cppVal_) = newCppVal;
        pushValToPeers(this);
    }

    /**
     * @brief Calls ClarityNode::setVal() first to initiate the push process if there are peer nodes. If the cppVal_
     * pointer is non-null, we also then update that value in this node.
     *
     * @param inval
     */
    inline virtual void setVal(const val &inval) {
        ClarityNode::setVal(inval);
        if (cppVal_ != nullptr) {
            setCppValFromJSVal(inval);
        }
    }

    /**
     * @brief If this node has a non-null cppVal_ pointer we return the JS form of the value therein. Otherwise we call
     * the CN method that pulls the value from the DOM element.
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

    virtual void refreshDOMValueFromModel() {
        if (cppVal_ != nullptr) {
            val jsval = val(*cppVal_);
            ClarityNode::setVal(jsval);
        }
    }

    virtual void updateNodeFromDom() {
        val jsval = getVal();
        if (cppVal_ != nullptr) {
            cout << "cppVal_ exists!\n";
            *cppVal_ = jsval.as<V>();
        }
    }

    ~HybridNode() { cout << "DESTROYING HybridNode with id: " << id_ << "\n"; }

   protected:
    V *cppVal_ = nullptr;  //!< The C++ data object that acts as the 'model'
};

}  // namespace clarity

#endif
