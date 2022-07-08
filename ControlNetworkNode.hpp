#ifndef ControlNetworkNode_hpp
#define ControlNetworkNode_hpp

#include "clarity.hpp"


namespace clarity{

class WebNode;

  /**
   * @brief The project's central class. Describes an element with push/pull behavior to syncronize a data model and
   * a web view. WebElements can contain others and complex web controls can be built up in a hierarchal fashion. The class is
   * not necessarily homologous to JavaScript Dom elements but the "mirror" class CLElement is currently used to store
   * a JS Dom element and retain other state on the JS side. This is also the base class for all other types.
   * The CNN type should never be instantiated as it has no specific behaviors and doesn't bind to anything
   * on the webpage. In future it may contain pure virtual methods and trying to instantiate it will be an error.
   *
   */
  class ControlNetworkNode
  {
  public:
    void setParent(ControlNetworkNode *parent) { this->parent_ = parent; }
    ControlNetworkNode *getParent() const { return this->parent_; }

    virtual val getVal() const
    {
      printNodeStats("getVal()");
      return val(NULL);
    }

    val getJSval() const { return jsval_; }

    void splicePtrs(void *worldValuePtr) { anyvalPtr_ = worldValuePtr; }
    virtual void printState() const { jsval_.call<void>("printState"); }
    static ControlNetworkNode *getCLElementById(const int id) { return switchboard[id]; }
    // virtual void updateModelFromView() {}
    // static void updateModelFromViewById(const int id) {}

    virtual string nodeStats() const
    {
      string s = "Node id: " + to_string(id_) + ", Node type: " + typeid(*this).name() + "\n";
      return s;
    }

    virtual void printNodeStats(const string &prefix = "") const
    {
      cout << prefix << " " << nodeStats();
    }

    static void markNodeDirtyById(int id) { switchboard[id]->clean_ = false; }

    static void pushValToPeersById(int id)
    {
      ControlNetworkNode *cnn = getCLElementById(id);
      cnn->printNodeStats("pushValToPeersById()");
      cnn->pushValToPeers(cnn);
    }

    void toggleClean()
    {
      cout << "TOGGLECLEAN, oldstate: " << clean_ << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ID = " << id_ << " \n\n\n";
      clean_ = !clean_;
    }

    /**
     * @brief Supported C++ types for WebElements.
     *
     */
    enum class CppType : int
    {
      Int,
      Float,
      Double,
      String,
      NoData /// Used for things like div that hold no data.
    };

    ControlNetworkNode(ControlNetworkNode *parent) : parent_(parent) {}

    ControlNetworkNode(const CppType anyvalPtrType) : anyvalPtrType_(anyvalPtrType)
    {
      id_ = tm.getNext();
      jsval_.set("cpptype", val(anyvalPtrType));
      ControlNetworkNode::switchboard[id_] = this;
    }

    ControlNetworkNode(void *anyvalPtr) : anyvalPtr_(anyvalPtr)
    {
      id_ = tm.getNext();
      // jsval_.set("id", val(id_));
      //
      ControlNetworkNode::switchboard[id_] = this;
    }

    template <typename T>
    inline static val cpp2js(void *valptr)
    {
      return val(*reinterpret_cast<T *>(valptr));
    }

    // virtual void updateViewFromModel() {}
    virtual void updatePeers() {}
    virtual void setVal(const val &inval)
    {
      cout << "Marking node " << id_ << " as dirty.\n\n";
      clean_ = false;
    }

    virtual void pushValToPeer(ControlNetworkNode *peer)
    {
      printNodeStats("pushValToPeer()");
      if (clean_)
      {
        cout << "Node " << id_ << " is clean, should return.\n";
        // return;
      }
      val internalVal = getVal();
      cout << "Internal val is ";
      jsval_.call<void>("printToConsole", internalVal);
      cout << "Value is type ";
      jsval_.call<void>("printType", internalVal);
      cout << endl;
      peer->setVal(internalVal);
      peer->pushValToPeers(this);
      clean_ = true;
    }

    virtual void pushValToPeers(ControlNetworkNode *excludedPeer = nullptr)

    {
      printNodeStats("pushValToPeers()");
      if (excludedPeer == nullptr)
      {
        for (auto peer : peers_)
        {
          pushValToPeer(peer);
        }
      }
      else
      {
        for (auto peer : peers_)
        {
          if (peer != excludedPeer)
          {
            pushValToPeer(peer);
          }
        }
      }
    }

    void
    addPeer(ControlNetworkNode *peer, bool alreadyAdded = false)
    {

      peers_.push_back(peer);
      if (alreadyAdded)
      {
        return;
      }
      peer->addPeer(this, true);
    }

  protected:
    bool clean_ = true;

    static TicketMachine tm;
    static map<const int, ControlNetworkNode *> switchboard;
    // val CLContext = val::global("CLElement");
    val jsval_ = val::global("CLElement").new_();
    val nodeVal = val(NULL);
    CppType anyvalPtrType_; // C++ Data type
    void *anyvalPtr_;       // pointer to actual data
    ControlNetworkNode *parent_;
    int id_;
    vector<ControlNetworkNode *> peers_;
  };
}
#endif
