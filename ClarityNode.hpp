#ifndef ClarityNode_hpp
#define ClarityNode_hpp

#include "clarity.hpp"
//#include "ModelNode.hpp"

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
        DualLink(ClarityNode *nodeA, ClarityNode *nodeB, const T multiplier = 1)
            : nodeA_(nodeA), nodeB_(nodeB) {
            a2b_xfmr_ = CLElement_.call<val>("generateTransformFn", val(multiplier));
            b2a_xfmr_ =
                CLElement_.call<val>("generateTransformFn", val(1 / multiplier));
        };

        DualLink(ClarityNode *nodeA, ClarityNode *nodeB, val a2b_xfmr,
                 val b2a_xfmr = val(NULL))
            : nodeA_(nodeA),
              nodeB_(nodeB),
              a2b_xfmr_(a2b_xfmr),
              b2a_xfmr_(b2a_xfmr) {}

        inline void printDL() {
            cout << "DL peer IDs: A = " << nodeA_->getId()
                 << ", B = " << nodeB_->getId() << "\n";
        }

        inline pair<ClarityNode *, val> getOtherNode(ClarityNode *thisNode) {
            if (nodeA_ == thisNode) {
                return pair(nodeB_, b2a_xfmr_);
            }
            return pair(nodeA_, a2b_xfmr_);
            // FIXME: probably should check to see if nodeA_ is actually the
            // same as thisNode and throw an exception if not.
        }
    };

    EMSCRIPTEN_KEEPALIVE void addEventListenerByName(
        const string &eventName, const string &callbackName);
    EMSCRIPTEN_KEEPALIVE void addJSEventListener(const string &eventName,
                                                 val eventCallback);

    inline string getTag() const { return tag_; }
    inline int getId() const { return id_; }
    inline void setCLE(val cle) { cle_ = cle; }

    inline static void runCallbackById(const string &id) { callbackMap[id](); }

    void EMSCRIPTEN_KEEPALIVE init();
    inline ClarityNode() { init(); }
    EMSCRIPTEN_KEEPALIVE ClarityNode(const string &name);
    // EMSCRIPTEN_KEEPALIVE ClarityNode(const CppType storedValueType);

    /**
     * @brief Construct a new Web Element object
     *
     * @param name String name of element, may be empty.
     * @param tag HTML tag of this element
     * @param storedValueType C++ type of data contained within
     *
     */
    ClarityNode(const string &name, const string &tag,
                bool useExistingDOMElement_ = false);

    ~ClarityNode() {
        cout << "DESTROYING CLARITYNODE " << id_ << "\n";

        for (auto dl : dlpeers_) {
            dl.reset();
        }
        dlpeers_.clear();
        val domElement = cle_["domElement"];
        if (!domElement.isUndefined()) cle_["domElement"].call<void>("remove");
    }

    EMSCRIPTEN_KEEPALIVE void setAttribute(const string &attr,
                                           const val &value);

    EMSCRIPTEN_KEEPALIVE void setAttributes(const map<string, val> &attrs);

    bool appendChild(ClarityNode *child);

    inline ClarityNode *getParent() const { return this->parent_; }
    inline void setParent(ClarityNode *parent) { this->parent_ = parent; }

    inline void setDatum(DatumBase *datum) { datum_ = datum; }

    inline void setTranslator(TranslatorBase *translator) {
        translator_ = translator;
    }

    inline virtual void setVal(val inval) {
        assert(boundField_ != "");
        clean_ = false;
        val domElement = cle_["domElement"];
        cle_.call<void>("printVal", inval);
        domElement.set(boundField_, inval);
        domElement.call<void>("setAttribute", val(boundField_), inval);
        if (translator_ != nullptr) {
            translator_->js2datum();
        }
    }

    inline virtual val getJSVal() const {
        assert(boundField_ != "");
        // clean_ = false;
        val domElement = cle_["domElement"];
        // cle_.call<void>("printVal", inval);
        // domElement.get(boundField_, inval);
        // val internalJSVal =
        //     domElement.call<val>("getAttribute", val(boundField_));

        val internalJSVal = domElement[boundField_];
        cle_.call<void>("printVal", internalJSVal,
                        val("getJSVal: id_ = " + to_string(id_)));
        return internalJSVal;
    }

    inline void setBoundField(const string &boundField) {
        boundField_ = boundField;
    }

    inline val getCLE() const { return cle_; }
    inline val getDomElement() const { return cle_["domElement"]; }
    inline string getName() const { return name_; }
    inline virtual void printState() const { cle_.call<void>("printState"); }
    inline static ClarityNode *getCLElementById(const int id) {
        return switchboard[id];
    }
    inline static void markNodeDirtyById(int id) {
        switchboard[id]->clean_ = false;
    }
    inline void toggleClean() { clean_ = !clean_; }

    template <typename T>
    inline static val cpp2js(void *valptr) {
        return val(*reinterpret_cast<T *>(valptr));
    }

    // template <typename T>
    //  val cpp2js(void *valptr) {
    //     return val(*reinterpret_cast<T *>(valptr));
    // }

    virtual string nodeStats(const string &msg = "") const;

    // virtual void pushValToPeer(ActiveLink &al, const string &tabs = "");
    virtual void pushValToPeer(DualLink &al);

    // virtual void pushValToPeers(ClarityNode *excludedPeer = nullptr);
    virtual void pushValToPeers(ClarityNode *excludedPeer = nullptr);
    // static void pushValToPeersById(int id);
    static void pushValToPeersById(int id);
    void js2datumWithPushToPeers();
    static void js2datumWithPushToPeersById(int id);
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

    void addPeer(ClarityNode *peer, val a2b_xfmr, val b2a_xfmr = val(NULL));
    inline void appendDualLink(shared_ptr<DualLink> dl) {
        dlpeers_.push_back(dl);
    }
    inline int countPeers() const { return dlpeers_.size(); }

   protected:
    /** \brief Instance of the CLElement class that acts as a "proxy" in JS
     * space. Also stores the DOM element that the translator connects to.*/
    val cle_ = val::global("CLElement").new_();

    string tag_;         //!< HTML tag.
    string boundField_;  //!< The field that holds the data for this node.
    vector<ClarityNode *> children_;  //!< Mostly homolougous with the
                                      //!< associated DOM element tree.

    /** \brief A node is 'clean' when we're done changing it. This
       feature is mainly designed to prevent infinite update loops if the node
       graph is not acyclic. */
    bool clean_ = false;

    /** \brief Hands out the numeric ids for all nodes.*/
    static TicketMachine tm_;

    /** \brief Keeps track of all nodes in the system. If you have the id of a
     * node you can get a pointer to it here. */
    static map<const int, ClarityNode *> switchboard;

    ClarityNode *parent_;  //!< Parent node of this node. Can be null.
    int id_ = -1;  //!< Unique identifier - needs to be immutable once set.
    string name_;  //!< Human readable name of node. Mutable, not required.

    vector<shared_ptr<ClarityNode::DualLink>>
        dlpeers_;  //!< Nodes that this node exchanges data with.

    TranslatorBase *translator_ = nullptr;
    DatumBase *datum_ = nullptr;  //!< The native (C++) data this node controls.
};

}  // namespace clarity

#endif
