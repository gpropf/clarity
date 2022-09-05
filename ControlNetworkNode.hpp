#ifndef ControlNetworkNode_hpp
#define ControlNetworkNode_hpp

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

class ControlNetworkNode {
   public:
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
        ActiveLink(ControlNetworkNode *peer, val scalarConst = val(1));

        template <typename T>
        ActiveLink(ControlNetworkNode *peer, const T scalarConst)
            : peer_(peer), scalarConst_(val(scalarConst)) {
            transformFn_ =
                CLElement_.call<val>("generateTransformFn", scalarConst_);
        }

        void printAL() {
            cout << "AL peer ID: " << peer_->getId() << "\n";
        }

        ControlNetworkNode *peer_;
        val scalarConst_;
        val transformFn_;
    };

    EMSCRIPTEN_KEEPALIVE void addEventListenerByName(
        const string &eventName, const string &callbackName);
    EMSCRIPTEN_KEEPALIVE void addJSEventListener(const string &eventName,
                                                 val eventCallback);

    inline string getTag() const { return tag_; }
    inline int getId() const { return id_; }
    inline void setCLE(val jsval) { cle_ = jsval; }

    inline static void runCallbackById(const string &id) { callbackMap[id](); }

    void EMSCRIPTEN_KEEPALIVE init();
    inline ControlNetworkNode() { init(); }
    EMSCRIPTEN_KEEPALIVE ControlNetworkNode(const string &name,
                                            const CppType storedValueType);
    EMSCRIPTEN_KEEPALIVE ControlNetworkNode(const CppType storedValueType);

    /**
     * @brief Construct a new Web Element object
     *
     * @param name String name of element, may be empty.
     * @param tag HTML tag of this element
     * @param storedValueType C++ type of data contained within
     *
     */
    ControlNetworkNode(const string &name, const string &tag,
                       const CppType storedValueType);

    EMSCRIPTEN_KEEPALIVE void setAttribute(const string &attr,
                                           const val &value);

    EMSCRIPTEN_KEEPALIVE void setAttributes(const map<string, val> &attrs);

    inline CppType getStoredValueType() const { return storedValueType_; }
    EMSCRIPTEN_KEEPALIVE void setStoredValueType(CppType cppType);

    bool appendChild(ControlNetworkNode *child);

    inline ControlNetworkNode *getParent() const { return this->parent_; }
    inline void setParent(ControlNetworkNode *parent) {
        this->parent_ = parent;
    }
    
    val getVal() const {
        val domElement = cle_["domElement"];
        string valueText = domElement[boundField_].as<string>();

        switch (this->storedValueType_) {
            case CppType::Int:
                return val(stoi(valueText));
                break;
            case CppType::Float:
                return val(stof(valueText));
                break;
            case CppType::Double:
                return val(stod(valueText));
                break;
            case CppType::String:
                return val(valueText);
                break;
            case CppType::NoData:
            default:
                return val(NULL);
                break;
        }
    }
    // inline virtual void setVal(const val &inval) { clean_ = false; }

    inline virtual void setVal(const val &inval) {
        cout << "CNN::setVal(const val &inval)\n";
        clean_ = false;
        val domElement = cle_["domElement"];
        domElement.set(boundField_, inval);
    }

    inline val getCLE() const { return cle_; }
    inline string getName() const { return name_; }
    inline virtual void printState() const { cle_.call<void>("printState"); }
    inline static ControlNetworkNode *getCLElementById(const int id) {
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

    virtual string nodeStats() const;

    virtual void pushValToPeer(ActiveLink &al);
    virtual void pushValToPeers(ControlNetworkNode *excludedPeer = nullptr);
    static void pushValToPeersById(int id);
    void addPeer(ControlNetworkNode::ActiveLink al, bool alreadyAdded = false);

   protected:
    
    string tag_;
    string boundField_;

    vector<ControlNetworkNode *> children_;
    /** \brief The node is clean if it has not been recently changed. This
       feature is mainly designed to prevent infinite update loops if the node
       graph is not acyclic. It doesn't do anything yet.*/
    bool clean_ = true;
    /** \brief Hands out the numeric ids for all nodes.*/
    static TicketMachine tm;
    /** \brief Keeps track of all nodes in the system. If you have the id of a
     * node you can get a pointer to it here. */
    static map<const int, ControlNetworkNode *> switchboard;

    static map<string, std::function<void()>> callbackMap;

    /** \brief Instance of the CLElement class that acts as a "proxy" in JS
     * space. */
    val cle_ = val::global("CLElement").new_();

    CppType storedValueType_;  //!< C++ Data type
    /** \brief A node's parent is the DOM element that contains it. In the case
     * of the WebAttrNode this is the WebElemNode for which this is an
     * attribute. */
    ControlNetworkNode *parent_;
    int id_ = -1;  //!< Unique identifier - immutable.
    string name_;  //!< Human readable name of node. Mutable, not required.
    /** \brief List of peers linked through JS functions which are applied when
     * data is moved between nodes. */
    vector<ControlNetworkNode::ActiveLink> peers_;
};
}  // namespace clarity
#endif
