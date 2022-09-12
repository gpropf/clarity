#ifndef ClarityNode_hpp
#define ClarityNode_hpp

#include "clarity.hpp"
//#include "CLNodeFactory.hpp"

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
    // friend class CLNodeFactory;

   public:
    static map<string, std::function<void()>> callbackMap;

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
            a2b_xfmr_ = CLElement_.call<val>("generateTransformFn", multiplier);
            b2a_xfmr_ =
                CLElement_.call<val>("generateTransformFn", 1 / multiplier);
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

    /**
     * @brief Represents the 'edges' in our control graph. These edges can be
     * active and contain a JS value that can act as a transformation on the
     * values moving between nodes.
     *
     *
     */
    class ActiveLink {
       public:
        static val CLElement_;
        ActiveLink(ClarityNode *peer, val constantOrFunction = val(1));

        template <typename T>
        ActiveLink(ClarityNode *peer, const T constantOrFunction)
            : peer_(peer), constantOrFunction_(val(constantOrFunction)) {
            transformFn_ = CLElement_.call<val>("generateTransformFn",
                                                constantOrFunction_);
        }

        void printAL() { cout << "AL peer ID: " << peer_->getId() << "\n"; }

        ClarityNode *peer_;
        val constantOrFunction_;
        val transformFn_;
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
    EMSCRIPTEN_KEEPALIVE ClarityNode(const string &name,
                                     const CppType storedValueType);
    EMSCRIPTEN_KEEPALIVE ClarityNode(const CppType storedValueType);

    /**
     * @brief Construct a new Web Element object
     *
     * @param name String name of element, may be empty.
     * @param tag HTML tag of this element
     * @param storedValueType C++ type of data contained within
     *
     */
    ClarityNode(const string &name, const string &tag,
                const CppType storedValueType,
                bool useExistingDOMElement_ = false);

    EMSCRIPTEN_KEEPALIVE void setAttribute(const string &attr,
                                           const val &value);

    EMSCRIPTEN_KEEPALIVE void setAttributes(const map<string, val> &attrs);

    inline CppType getStoredValueType() const { return storedValueType_; }
    EMSCRIPTEN_KEEPALIVE void setStoredValueType(CppType cppType);

    bool appendChild(ClarityNode *child);

    inline ClarityNode *getParent() const { return this->parent_; }
    inline void setParent(ClarityNode *parent) { this->parent_ = parent; }

    virtual val getVal() const {
        val domElement = cle_["domElement"];

        string valueText = domElement[boundField_].as<string>();
        //cout << "ClarityNode::getVal() valueText = " << valueText << "\n";
        switch (this->storedValueType_) {
            case CppType::Int:
                cout << "ClarityNode::getVal() Int\n";
                return val(stoi(valueText));
                break;
            case CppType::Float:
                cout << "ClarityNode::getVal() Float\n";
                return val(stof(valueText));
                break;
            case CppType::Double:
                cout << "ClarityNode::getVal() Double\n";
                return val(stod(valueText));
                break;
            case CppType::String:
                cout << "ClarityNode::getVal() String\n";
                return val(valueText);
                break;
            case CppType::NoData:
            default:
                cout << "ClarityNode::getVal() NoData\n";
                return val(NULL);
                break;
        }
    }
    // inline virtual void setVal(const val &inval) { clean_ = false; }

    inline virtual void setVal(const val &inval) {
        cout << "<<<<<<<<<<<<< CNN::setVal(const val &inval) >>>>>>>>>>>>>\n";
        clean_ = false;
        val domElement = cle_["domElement"];
        cle_.call<void>("printVal", inval);
        cout << "boundField_ = " << boundField_ << "\n";
        domElement.set(boundField_, inval);
        domElement.call<void>("setAttribute", val(boundField_), inval);
    }

    inline void setBoundField(const string &boundField) {
        boundField_ = boundField;
    }

    inline val getCLE() const { return cle_; }
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

    virtual string nodeStats(const string &msg = "") const;

    virtual void pushValToPeer(ActiveLink &al, const string &tabs = "");
    virtual void pushValToPeer2(DualLink &al, const string &tabs = "");

    // virtual void pushValToPeers(ClarityNode *excludedPeer = nullptr);
    virtual void pushValToPeers2(ClarityNode *excludedPeer = nullptr);
    static void pushValToPeersById(int id);
    static void pushValToPeersById2(int id);
    // void addPeer(ClarityNode::ActiveLink al, bool alreadyAdded = false);

    template <typename T>
    void addPeer2(ClarityNode *peer,
                                        const T linkMultiplierConstant = 1) {
        auto dl = make_shared<DualLink>(this, peer, linkMultiplierConstant);
        dlpeers_.push_back(dl);
        peer->appendDualLink(dl);
    }
    //   void addPeer2(ClarityNode *peer);

    void addPeer2(ClarityNode *peer, val a2b_xfmr, val b2a_xfmr = val(NULL));
    inline void appendDualLink(shared_ptr<DualLink> dl) {
        dlpeers_.push_back(dl);
    }
    inline int countPeers() const { return peers_.size(); }

   protected:
    string tag_;
    string boundField_;

    vector<ClarityNode *> children_;
    /** \brief The node is clean if it has not been recently changed. This
       feature is mainly designed to prevent infinite update loops if the node
       graph is not acyclic. It doesn't do anything yet.*/
    bool clean_ = true;
    /** \brief Hands out the numeric ids for all nodes.*/
    static TicketMachine tm;
    /** \brief Keeps track of all nodes in the system. If you have the id of a
     * node you can get a pointer to it here. */
    static map<const int, ClarityNode *> switchboard;

    /** \brief Instance of the CLElement class that acts as a "proxy" in JS
     * space. */
    val cle_ = val::global("CLElement").new_();

    CppType storedValueType_;  //!< C++ Data type
    /** \brief A node's parent is the DOM element that contains it. In the case
     * of the WebAttrNode this is the WebElemNode for which this is an
     * attribute. */
    ClarityNode *parent_;
    int id_ = -1;  //!< Unique identifier - immutable.
    string name_;  //!< Human readable name of node. Mutable, not required.
    /** \brief List of peers linked through JS functions which are applied when
     * data is moved between nodes. */
    vector<ClarityNode::ActiveLink> peers_;
    vector<shared_ptr<ClarityNode::DualLink>> dlpeers_;
};

}  // namespace clarity

#endif