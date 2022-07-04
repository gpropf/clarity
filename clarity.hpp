
#include <stdio.h>
#include <iostream>
//#include <cstdlib>
#include <unistd.h>

//#include <vector>
#include <map>
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <functional>

using namespace std;
using namespace emscripten;

// Run with 'python3 -m http.server 8000'

namespace clarity
{

  /**
   * @brief Interface class that should be implemented by all models and
   * views. Has only the update method. Typically a model or view being
   * updated will trigger the update of it corresponding element. Models
   * are always paired with views and vice versa.
   *
   */

  /**
   * @brief Hands out (presumably) unique int ids with a simply incrementing counter.
   *
   */
  class TicketMachine
  {
  private:
    int id_ = 0;

  public:
    inline const int getNext() { return ++id_; }
  };

  class WebElemNode;

  class WebNode;

  /**
   * @brief Base class for all nodes.
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

  class ModelNode : public ControlNetworkNode
  {
  public:
    ModelNode(void *anyvalPtr) : ControlNetworkNode(anyvalPtr) {}
    ModelNode(CppType anyvalPtrType) : ControlNetworkNode(anyvalPtrType) {}
    // void updateViewFromModel() {}
    virtual void updatePeers() {}
    // virtual void updateModelFromView() {}

    virtual val getVal() const
    {
      ControlNetworkNode::getVal();
      // cout << "GETVAL called for ModelNode!\n\n";
      if (anyvalPtr_ == nullptr)
      {
        return val(NULL);
      }
      switch (this->anyvalPtrType_)
      {
      case CppType::Int:
        return val(cpp2js<int>(anyvalPtr_));
        break;
      case CppType::Float:
        return val(cpp2js<float>(anyvalPtr_));
        break;
      case CppType::Double:
        return val(cpp2js<double>(anyvalPtr_));
        break;
      case CppType::String:
        return val(cpp2js<string>(anyvalPtr_));
        break;
      case CppType::NoData:
      default:
        return val(NULL);
        break;
      }
    }

    virtual void setVal(const val &inval)
    {
      ControlNetworkNode::setVal(inval);
      nodeVal = inval;

      if (anyvalPtr_ == nullptr)
      {
        return;
      }

      switch (this->anyvalPtrType_)
      {
      case CppType::Int:
        *reinterpret_cast<int *>(anyvalPtr_) = this->jsval_.call<int>("jsToCPPVal", inval);
        cout << "C++ side: New Int Value: " << *reinterpret_cast<int *>(anyvalPtr_) << endl;
        break;
      case CppType::Float:
        *reinterpret_cast<float *>(anyvalPtr_) = this->jsval_.call<int>("jsToCPPVal", inval);
        cout << "C++ side: New Float Value: " << *reinterpret_cast<float *>(anyvalPtr_) << endl;
        break;
      case CppType::Double:
        //*reinterpret_cast<double *>(anyvalPtr_) = this->jsval_["anyval"].as<double>();
        *reinterpret_cast<double *>(anyvalPtr_) = this->jsval_.call<double>("jsToCPPVal", inval);
        cout << "C++ side: New Double Value: " << *reinterpret_cast<double *>(anyvalPtr_) << endl;
        break;
      case CppType::String:
        *reinterpret_cast<string *>(anyvalPtr_) = this->jsval_.call<string>("jsToCPPVal", inval);
        cout << "C++ side: New String Value: " << *reinterpret_cast<string *>(anyvalPtr_) << endl;
        break;
      case CppType::NoData:
        cout << "C++ side: This element contains no data." << endl;
        break;
      default:
        break;
      }

      return; // this->jsval_["anyval"];
    }
  };

  class WebNode : public ControlNetworkNode
  {
  public:
    WebNode(const CppType anyvalPtrType) : ControlNetworkNode(anyvalPtrType) {}
    virtual val getVal() const
    {
      ControlNetworkNode::getVal();
      // cout << "GETVAL called for WebNode!\n\n";
      val domElement = jsval_["domElement"];
      string valueText = domElement[boundField_].as<string>();

      switch (this->anyvalPtrType_)
      {
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

  protected:
    string boundField_;

    void setVal(const val &inval)
    {
      ControlNetworkNode::setVal(inval);
      val domElement = jsval_["domElement"];
      domElement.set(boundField_, inval);
    }

    // }
  };

  // WebNode(const CppType anyvalPtrType) : anyvalPtrType_(anyvalPtrType)

  // };

  /**
   * @brief The project's central class. Describes an element with push/pull behavior to syncronize a data model and
   * a web view. WebElements can contain others and complex web controls can be built up in a hierarchal fashion. The class is
   * not necessarily homologous to JavaScript Dom elements but the "mirror" class CLElement is currently used to store
   * a JS Dom element and retain other state on the JS side.
   *
   */
  class WebElemNode : public WebNode
  {
  public:
    WebElemNode(const CppType anyvalPtrType) : WebNode(anyvalPtrType) {}

  protected:
    vector<ControlNetworkNode *> children_;

    // string tag_, name_;
    string tag_, name_;
    string stringId_;

  public:
    /**
     * @brief Construct a new Web Element object
     *
     * @param name String name of element, may be empty.
     * @param tag HTML tag of this element
     * @param anyvalPtrType C++ type of data contained within
     *
     */
    WebElemNode(const string &name, const string &tag,
                const CppType anyvalPtrType) : tag_(tag),
                                               name_(name),
                                               WebNode(anyvalPtrType)

    {

      jsval_.set("cpptype", val(anyvalPtrType));
      jsval_.set("tag", val(tag));
      jsval_.set("name", val(name_));
      jsval_.set("id", val(id_));

      anyvalPtr_ = nullptr;
      boundField_ = "value";

      ControlNetworkNode::switchboard[id_] = this;
    }

    void setAttribute(const string &attr, const val &value)
    {
      val domElement = jsval_["domElement"];
      domElement.call<void>("setAttribute", attr, value);
    }

    /**
     * @brief Update the view from the model
     *
     */
    /*
    void updateViewFromModel()
    {
      cout << "STARTING: updateViewFromModel for " << this->name_ << "\n";
      val domElement = this->jsval_["domElement"];
      for (auto child : children_)
      {
        child->updateViewFromModel();
      }

      if (anyvalPtr_ == nullptr)
      {
        cout << "ENDING: updateViewFromModel for " << this->name_ << " because anyvalPtr_ == nullptr\n";
        return;
      }

      switch (this->anyvalPtrType_)
      {
      case CppType::Int:
        domElement.set(boundField_, val(cpp2js<int>(anyvalPtr_)));
        break;
      case CppType::Float:
        domElement.set(boundField_, val(cpp2js<float>(anyvalPtr_)));
        break;
      case CppType::Double:
        // domElement.set(boundField, val(*reinterpret_cast<double *>(anyvalPtr_)));
        domElement.set(boundField_, val(cpp2js<double>(anyvalPtr_)));
        // jsval_.set("anyval", val(*reinterpret_cast<double *>(anyvalPtr_)));
        break;
      case CppType::String:
        domElement.set(boundField_, val(cpp2js<string>(anyvalPtr_)));
        break;
      case CppType::NoData:
      default:
        break;
      }
      cout << "ENDING: updateViewFromModel for " << this->name_ << "\n";
    } */
    /*
    virtual void updateModelFromView()
    {
      cout << "STARTING: updateModelFromView for " << this->name_ << "\n";
      for (auto child : children_)
      {
        child->updateModelFromView();
      }

      if (anyvalPtr_ == nullptr)
      {
        cout << "ENDING: updateModelFromView for " << this->name_ << " because anyvalPtr_ == nullptr\n";
        return; // this->jsval_["anyval"];
      }

      switch (this->anyvalPtrType_)
      {
      case CppType::Int:
        *reinterpret_cast<int *>(anyvalPtr_) = this->jsval_.call<int>("jsToCPPVal", getVal());
        cout << "C++ side: New Int Value: " << *reinterpret_cast<int *>(anyvalPtr_) << endl;
        break;
      case CppType::Float:
        *reinterpret_cast<float *>(anyvalPtr_) = this->jsval_.call<int>("jsToCPPVal", getVal());
        cout << "C++ side: New Float Value: " << *reinterpret_cast<float *>(anyvalPtr_) << endl;
        break;
      case CppType::Double:
        *reinterpret_cast<double *>(anyvalPtr_) = this->jsval_["anyval"].as<double>();
        *reinterpret_cast<double *>(anyvalPtr_) = this->jsval_.call<double>("jsToCPPVal", getVal());
        cout << "C++ side: New Double Value: " << *reinterpret_cast<double *>(anyvalPtr_) << endl;
        break;
      case CppType::String:
        *reinterpret_cast<string *>(anyvalPtr_) = this->jsval_.call<string>("jsToCPPVal", getVal());
        cout << "C++ side: New String Value: " << *reinterpret_cast<string *>(anyvalPtr_) << endl;
        break;
      case CppType::NoData:
        cout << "C++ side: This element contains no data." << endl;
        break;
      default:
        break;
      }
      cout << "ENDING: updateModelFromView for " << this->name_ << "\n";
      return; // this->jsval_["anyval"];
    } */

    bool appendChild(WebNode *child)
    {
      children_.push_back(child);
      jsval_.call<void>("appendChild", child->getJSval());
      return true; // FIXME: need to check for duplicate ids.
    }

    void addEventListenerByName(const string &eventName, const string &callbackName)
    {
      jsval_.call<void>("addEventListenerById", eventName, callbackName);
    }
    //=====================
    // static map<const int, WebElemNode *> switchboard;
    static map<string, std::function<void()>> callbackMap;

    string getTag() const { return tag_; }

    string getId() const { return name_; }
    // void setId(string id) { id_ = id; }

    void setJsval(val jsval) { jsval_ = jsval; }
    void *getAnyvalPtr() const { return anyvalPtr_; }
    void setAnyvalPtr(void *valptr) { anyvalPtr_ = valptr; }
    CppType getAnyvalPtrType() const { return anyvalPtrType_; }
    void setAnyvalPtrType(CppType cppType)
    {
      anyvalPtrType_ = cppType;
      jsval_.set("cpptype", cppType);
    }

    // static void updateModelFromViewById(const int id) { switchboard[id]->updateModelFromView(); }
    // static WebElemNode &getCLElementById(const int id) { return *(switchboard[id]); }
    static void runCallbackById(const string &id) { callbackMap[id](); }
  };

  /**
   * @brief Represents an attribute of another element such as bgcolor.
   * This is not necessarily a dom element that we control though it may be.
   *
   */
  class WebAttrNode : public WebNode
  {
  protected:
    string attributeName_;

  public:
    void updatePeers() {} // FIXME

    WebAttrNode(const string &attributeName, const CppType anyvalPtrType,
                ControlNetworkNode *parent) : attributeName_(attributeName),
                                              WebNode(anyvalPtrType)
    {
      parent_ = parent;
      boundField_ = attributeName;
      val parentDomelement = parent_->getJSval()["domElement"];
      // bool cln = parent->clean_;
      jsval_.set("domElement", parentDomelement);
    }

    // void updateViewFromModel()
    // {
    //   // FIXME: fill in method
    // }

    virtual void setVal(const val &inval)
    {
      ControlNetworkNode::setVal(inval);
      val domElement = jsval_["domElement"];
      domElement.call<void>("setAttribute", boundField_, inval);
    }
  };

  class ButtonElement : public WebElemNode
  {
  public:
    ButtonElement(const string &name, const string &tag, const CppType anyvalPtrType) : WebElemNode(name, tag, anyvalPtrType)
    {
      boundField_ = "textContent";
    }
  };

  EMSCRIPTEN_BINDINGS(WebElemNode)
  {
    class_<ControlNetworkNode>("ControlNetworkNode")
        .function("toggleClean", &ControlNetworkNode::toggleClean, allow_raw_pointers())
        .function("pushValToPeers", &ControlNetworkNode::pushValToPeers, allow_raw_pointers())
        .function("getVal", &ControlNetworkNode::getVal, allow_raw_pointers())
        .class_function("pushValToPeersById", &ControlNetworkNode::pushValToPeersById, allow_raw_pointers())
        .class_function("getCLElementById", &ControlNetworkNode::getCLElementById, allow_raw_pointers())
        .class_function("markNodeDirtyById", &ControlNetworkNode::markNodeDirtyById, allow_raw_pointers());

    class_<WebElemNode>("WebElement")
        .constructor<string, string, const ControlNetworkNode::CppType>(allow_raw_pointers())
        .property("tag", &WebElemNode::getTag)
        .property("id", &WebElemNode::getId)
        .property("anyvalPtrType", &WebElemNode::getAnyvalPtrType, &WebElemNode::setAnyvalPtrType)
        //.function("updateModelFromView", &WebElemNode::updateModelFromView)
        .function("splicePtrs", &WebElemNode::splicePtrs, allow_raw_pointers())

        //.class_function("updateModelFromViewById", &WebElemNode::updateModelFromViewById, allow_raw_pointers())
        .class_function("runCallbackById", &WebElemNode::runCallbackById, allow_raw_pointers());

    enum_<ControlNetworkNode::CppType>("WebElementCppType")
        .value("Int", ControlNetworkNode::CppType::Int)
        .value("Float", ControlNetworkNode::CppType::Float)
        .value("Double", ControlNetworkNode::CppType::Double)
        .value("String", ControlNetworkNode::CppType::String)
        .value("NoData", ControlNetworkNode::CppType::NoData);
  }
}

/**
 * @brief switchboard is where a map of all the WebElements is stored so that
 * they can be found by their id numbers.
 *
 */
// map<const int, clarity::WebElemNode *> clarity::WebElemNode::switchboard;
map<const int, clarity::ControlNetworkNode *> clarity::ControlNetworkNode::switchboard;

/**
 * @brief callbackMap holds C++ functions that can be triggered from JS
 * when events like a button press or timer tick require modification of
 * the C++ model state.
 *
 */
map<string, std::function<void()>> clarity::WebElemNode::callbackMap;
clarity::TicketMachine clarity::ControlNetworkNode::tm;