
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
  // class UpdateAble
  // {
  // public:
  //   virtual void update() = 0; /// Perform a single update
  // };

  /**
   * @brief Hands out (presumably) unique int ids with a simply incrementing counter.
   *
   */
  class TicketMachine
  {
  private:
    int id_ = 0;

  public:
    const int getNext() { return ++id_; }
  };

  /**
   * @brief Base class for all nodes.
   *
   */
  class ControlNetworkNode
  {
  public:
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

    virtual void updateViewFromModel() = 0;
    virtual void updatePeers() = 0;

  protected:
    static TicketMachine tm;

    ControlNetworkNode(const CppType anyvalPtrType) : anyvalPtrType_(anyvalPtrType)
    {
      id_ = tm.getNext();
    }
    CppType anyvalPtrType_; // C++ Data type
    void *anyvalPtr_;       // pointer to actual data
    string boundField_;
    int id_;
    vector<ControlNetworkNode *> peers_;
  };

  class WebNode : public ControlNetworkNode
  {
    // WebNode(const CppType anyvalPtrType) : ControlNetworkNode(anyvalPtrType)
    // {
    //   // id_ = tm.getNext();
    protected:
    WebNode(const CppType anyvalPtrType): ControlNetworkNode(anyvalPtrType) {}

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
    void updatePeers() {} // FIXME

  private:
    vector<WebElemNode *> children_;
    WebElemNode *parent_;
    // string tag_, name_;
    string tag_, name_;

    val jsval_ = val::global("CLElement");

protected:
WebElemNode(const CppType anyvalPtrType): WebNode(anyvalPtrType) {}


  public:
    /**
     * @brief Construct a new Web Element object
     *
     * @param id Unique identifier
     * @param tag HTML tag of this element
     * @param anyvalPtrType C++ type of data contained within
     * @param isAttributeOfParent Some WEs represent actual DOM children of their parent and others are simply attributes
     */
    WebElemNode(const string &name, const string &tag,
                const CppType anyvalPtrType) : tag_(tag),
                                               name_(name),
                                               WebNode(anyvalPtrType)

    {
      val CLContext = val::global("CLElement");

      jsval_ = CLContext.new_();
      jsval_.set("cpptype", val(anyvalPtrType));
      jsval_.set("tag", val(tag));
      jsval_.set("id", val(id_));
      jsval_.set("name", val(name_));
      anyvalPtr_ = nullptr;
      boundField_ = "value";

      WebElemNode::switchboard[id_] = this;
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
        domElement.set(boundField_, val(*reinterpret_cast<int *>(anyvalPtr_)));
        break;
      case CppType::Float:
        domElement.set(boundField_, val(*reinterpret_cast<float *>(anyvalPtr_)));
        break;
      case CppType::Double:
        // domElement.set(boundField, val(*reinterpret_cast<double *>(anyvalPtr_)));
        domElement.set(boundField_, val(*reinterpret_cast<double *>(anyvalPtr_)));
        // jsval_.set("anyval", val(*reinterpret_cast<double *>(anyvalPtr_)));
        break;
      case CppType::String:
        domElement.set(boundField_, val(*reinterpret_cast<string *>(anyvalPtr_)));
        break;
      case CppType::NoData:
      default:
        break;
      }
      cout << "ENDING: updateViewFromModel for " << this->name_ << "\n";
    }

    void updateModelFromView()
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
        *reinterpret_cast<int *>(anyvalPtr_) = this->jsval_.call<int>("jsToCPPVal", getDomElementVal());
        cout << "C++ side: New Int Value: " << *reinterpret_cast<int *>(anyvalPtr_) << endl;
        break;
      case CppType::Float:
        *reinterpret_cast<float *>(anyvalPtr_) = this->jsval_.call<int>("jsToCPPVal", getDomElementVal());
        cout << "C++ side: New Float Value: " << *reinterpret_cast<float *>(anyvalPtr_) << endl;
        break;
      case CppType::Double:
        //*reinterpret_cast<double *>(anyvalPtr_) = this->jsval_["anyval"].as<double>();
        *reinterpret_cast<double *>(anyvalPtr_) = this->jsval_.call<double>("jsToCPPVal", getDomElementVal());
        cout << "C++ side: New Double Value: " << *reinterpret_cast<double *>(anyvalPtr_) << endl;
        break;
      case CppType::String:
        *reinterpret_cast<string *>(anyvalPtr_) = this->jsval_.call<string>("jsToCPPVal", getDomElementVal());
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

    val getDomElementVal() const
    {
      val domElement = jsval_["domElement"];
      return domElement[boundField_];
    }

    bool appendChild(WebElemNode *child)
    {
      children_.push_back(child);
      jsval_.call<void>("appendChild", child->getJSval());
      return true; // FIXME: need to check for duplicate ids.
    }

    void addEventListenerById(const string &eventName, const string &callbackId)
    {
      jsval_.call<void>("addEventListenerById", eventName, callbackId);
    }
    //=====================
    static map<const int, WebElemNode *> switchboard;
    static map<string, std::function<void()>> callbackMap;

    string getTag() const { return tag_; }
    void setParent(WebElemNode *parent) { this->parent_ = parent; }
    WebElemNode *getParent() const { return this->parent_; }
    string getId() const { return name_; }
    // void setId(string id) { id_ = id; }
    val getJSval() const { return jsval_; }
    void setJsval(val jsval) { jsval_ = jsval; }
    void *getAnyvalPtr() const { return anyvalPtr_; }
    void setAnyvalPtr(void *valptr) { anyvalPtr_ = valptr; }
    CppType getAnyvalPtrType() const { return anyvalPtrType_; }
    void setAnyvalPtrType(CppType cppType)
    {
      anyvalPtrType_ = cppType;
      jsval_.set("cpptype", cppType);
    }
    void splicePtrs(void *worldValuePtr) { anyvalPtr_ = worldValuePtr; }

    static void updateModelFromViewById(const int id) { switchboard[id]->updateModelFromView(); }
    static WebElemNode &getCLElementById(const int id) { return *(switchboard[id]); }
    static void runCallbackById(const string &id) { callbackMap[id](); }
  };

  // class WebElemNode : public WebElemNode
  // {

  //   void updatePeers() {} // FIXME

  //   void updateViewFromModel()
  //   {
  //     // FIXME: fill in method
  //   }
  // };

  /**
   * @brief Represents an attribute of another element such as bgcolor.
   * This is not necessarily a dom element that we control though it may be.
   *
   */
  class WebAttrNode : public WebElemNode
  {

    void updatePeers() {} // FIXME

    void updateViewFromModel()
    {
      // FIXME: fill in method
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
    class_<WebElemNode>("WebElement")
        .constructor<string, string, const ControlNetworkNode::CppType>(allow_raw_pointers())
        .property("tag", &WebElemNode::getTag)
        .property("id", &WebElemNode::getId)
        .property("anyvalPtrType", &WebElemNode::getAnyvalPtrType, &WebElemNode::setAnyvalPtrType)
        .function("updateModelFromView", &WebElemNode::updateModelFromView)
        .function("splicePtrs", &WebElemNode::splicePtrs, allow_raw_pointers())
        .class_function("getCLElementById", &WebElemNode::getCLElementById, allow_raw_pointers())
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
    // cout << "addr(s) = " << &s_ << endl;
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
map<const int, clarity::WebElemNode *> clarity::WebElemNode::switchboard;

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

  tc->inputA_->splicePtrs(&tm->delta_);
  tc->inputB_->splicePtrs(&tm->s_);
  tc->sliderA_->splicePtrs(&tm->s_);
  tc->applyButton_->splicePtrs(buttonText);
  tc->sliderA_->addEventListenerById("change", "syncModelView");
  tc->applyButton_->addEventListenerById("click", "iterateModel");

  tc->updateViewFromModel();
  printf("Setup complete!\n");
  return 0;
}
