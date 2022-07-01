
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

// class World
// {
//   virtual void iterate() = 0;
//   virtual void printState() = 0;
// };
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
    virtual val getVal() const
    {
      printNodeStats("getVal()");
      return val(NULL);
    }

    val getJSval() const { return jsval_; }

    void splicePtrs(void *worldValuePtr) { anyvalPtr_ = worldValuePtr; }
    virtual void printState() const { jsval_.call<void>("printState"); }
    static ControlNetworkNode *getCLElementById(const int id) { return switchboard[id]; }
    virtual void updateModelFromView() {}
    static void updateModelFromViewById(const int id)
    { // switchboard[id]->updateModelFromView();
    }

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

    ControlNetworkNode(WebElemNode *parent) : parent_(parent) {}

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
    static val cpp2js(void *valptr)
    {
      return val(*reinterpret_cast<T *>(valptr));
    }

    virtual void updateViewFromModel() {}
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
    WebElemNode *parent_;
    int id_;
    vector<ControlNetworkNode *> peers_;
  };

  class ModelNode : public ControlNetworkNode
  {
  public:
    ModelNode(void *anyvalPtr) : ControlNetworkNode(anyvalPtr) {}
    ModelNode(CppType anyvalPtrType) : ControlNetworkNode(anyvalPtrType) {}
    void updateViewFromModel() {}
    virtual void updatePeers() {}
    virtual void updateModelFromView() {}

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
        // cout << "ENDING: updateModelFromView for " << this->name_ << " because anyvalPtr_ == nullptr\n";
        return; // this->jsval_["anyval"];
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

    void setParent(WebElemNode *parent) { this->parent_ = parent; }
    WebElemNode *getParent() const { return this->parent_; }

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
      jsval_.set("id", val(id_));
      jsval_.set("name", val(name_));
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
    }

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
        //*reinterpret_cast<double *>(anyvalPtr_) = this->jsval_["anyval"].as<double>();
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
    }

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

    static void updateModelFromViewById(const int id) { switchboard[id]->updateModelFromView(); }
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

    void updatePeers() {} // FIXME

    WebAttrNode(const CppType anyvalPtrType, WebElemNode *parent) : WebNode(anyvalPtrType)
    {
      parent_ = parent;
    }

    void updateViewFromModel()
    {
      // FIXME: fill in method
    }

    void setVal(const val &inval)
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
        .function("updateModelFromView", &WebElemNode::updateModelFromView)
        .function("splicePtrs", &WebElemNode::splicePtrs, allow_raw_pointers())

        .class_function("updateModelFromViewById", &WebElemNode::updateModelFromViewById, allow_raw_pointers())
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
 * @brief A simple model that just changes its internal state over time to
 * act as a testbed for the Clarity library.
 *
 */
class ToyModel //: public clarity::UpdateAble
{
public:
  void printState() const
  {
    cout << "MODEL STATE: s = " << s_ << ", delta = " << delta_ << endl;
  }

  void update() {}

  void iterate()
  {
    s_ += delta_;
    delta_ *= 0.95;
    printState();
  }

  ToyModel(double s, double delta) : s_(s), delta_(delta)
  {
    printState();
  }

  // private:
  double s_, delta_;
  // int i_;
};

/**
 * @brief A simble example of a complex control made up of simpler ones.
 *
 */
class ToyControl : public clarity::WebElemNode
{

public:
  ToyControl(const string &name, const string &tag, const CppType anyvalPtrType) : WebElemNode(name, tag, anyvalPtrType)
  {
    // mainDiv_ = new clarity::WebElement("mainDiv_", "div", CppType::NoData);
    inputA_ = new clarity::WebElemNode("inputA_", "input", CppType::Double);
    inputB_ = new clarity::WebElemNode("inputB_", "input", CppType::Double);
    sliderA_ = new clarity::WebElemNode("sliderA_", "input", CppType::Double);
    applyButton_ = new clarity::ButtonElement("applyButton_", "button", CppType::String);
    inputA_->setAttribute("type", val("text"));
    inputB_->setAttribute("type", val("text"));
    sliderA_->setAttribute("type", val("range"));
    this->appendChild(inputA_);
    this->appendChild(inputB_);
    this->appendChild(applyButton_);
    this->appendChild(sliderA_);
    // this->appendChild(*mainDiv_);
  }

  clarity::WebElemNode *mainDiv_;
  clarity::WebElemNode *inputA_;
  clarity::WebElemNode *sliderA_;
  clarity::WebElemNode *inputB_;
  clarity::WebElemNode *applyButton_;
};

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

int main()
{
  val CLContext = val::global("CLElement");
  if (CLContext.as<bool>())
  {
    printf("Got CLElement!\n");
  }
  else
  {
    return -1;
  }

  int *i = new int(100);
  double *n = new double(11);
  clarity::ModelNode *nm = new clarity::ModelNode(clarity::ControlNetworkNode::CppType::Double);
  nm->splicePtrs(n);
  clarity::WebElemNode *ratioDiv = new clarity::WebElemNode("ratiofoo", "div",
                                                            clarity::ControlNetworkNode::CppType::NoData);
  clarity::WebElemNode *ncntr = new clarity::WebElemNode("numerator", "input",
                                                         clarity::ControlNetworkNode::CppType::Double);
  clarity::WebElemNode *nslider = new clarity::WebElemNode("numerator", "input",
                                                           clarity::ControlNetworkNode::CppType::Double);
  clarity::WebElemNode *svgarea = new clarity::WebElemNode("svgarea", "svg", clarity::ControlNetworkNode::CppType::NoData);
  clarity::WebElemNode *cir1 = new clarity::WebElemNode("cir1", "circle", clarity::ControlNetworkNode::CppType::Double);

  //<circle cx="50" cy="50" r="40" stroke="green" stroke-width="4" fill="yellow" />
  svgarea->setAttribute("xmlns", val("http://www.w3.org/2000/svg"));
  svgarea->setAttribute("width", val("200px"));
  svgarea->setAttribute("height", val("200px"));
  svgarea->setAttribute("viewBox", val("0 0 200 200"));
  svgarea->setAttribute("xmlns:xlink", val("http://www.w3.org/1999/xlink"));
  svgarea->setAttribute("style", val("border: 1px solid black"));
  cir1->setAttribute("cx", val(100));
  cir1->setAttribute("cy", val(100));
  cir1->setAttribute("stroke", val("green"));
  cir1->setAttribute("stroke-width", val(4));
  cir1->setAttribute("r", val(80));

  nslider->setAttribute("type", val("range"));
  ratioDiv->appendChild(ncntr);
  ratioDiv->appendChild(nslider);
  
  svgarea->appendChild(cir1);
  ncntr->setAttribute("type", val("text"));
  nm->addPeer(ncntr);
  nm->addPeer(nslider);
  ncntr->addEventListenerByName("change", "printNetworkState");
  nslider->addEventListenerByName("change", "printNetworkState");
//ratioDiv->appendChild(svgarea);
  ToyControl *tc = new ToyControl("tc1", "div", clarity::ControlNetworkNode::CppType::NoData);
  ToyModel *tm = new ToyModel(0, 1);
  string *buttonText = new string("CLICK ME!");

  clarity::WebElemNode::callbackMap["iterateModel"] = [=]
  {
    cout << "callbackMap[\"iterateModel\"]\n";
    tm->iterate();
    tm->printState();
    tc->updateViewFromModel();
    tc->updateModelFromView();
  };

  clarity::WebElemNode::callbackMap["syncModelView"] = [=]
  {
    cout << "callbackMap[\"syncModelView\"]\n";
    tm->printState();
    tc->updateModelFromView();
    tc->updateViewFromModel();
  };

  clarity::WebElemNode::callbackMap["printNetworkState"] = [=]
  {
    cout << "callbackMap[\"printNetworkState\"]\n";
    // ncntr->toggleClean();
    // ncntr->pushValToPeers();
    // nslider->pushValToPeers();
    ncntr->printState();
  };

  tc->inputA_->splicePtrs(&tm->delta_);
  tc->inputB_->splicePtrs(&tm->s_);
  tc->sliderA_->splicePtrs(&tm->s_);
  tc->applyButton_->splicePtrs(buttonText);
  tc->sliderA_->addEventListenerByName("change", "syncModelView");
  tc->applyButton_->addEventListenerByName("click", "iterateModel");

  tc->updateViewFromModel();
  printf("Setup complete!\n");
  return 0;
}
