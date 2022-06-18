
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
   * @brief Base class for all viewable elements that can take on a value controlled from a model.
   * WebElement and AttributeElement should descend from this.
   *
   */
  class BoundElement
  {
    virtual void updateViewFromModel() = 0;
    string boundField;
  };

  /**
   * @brief Represents an attribute of another element such as bgcolor.
   * This is not necessarily a dom element that we control though it may be.
   *
   */
  class AttributeElement : public BoundElement
  {
    void updateViewFromModel()
    {
      // FIXME: fill in method
    }
  };

  /**
   * @brief The project's central class. Describes an element with push/pull behavior to syncronize a data model and
   * a web view. WebElements can contain others and complex web controls can be built up in a hierarchal fashion. The class is
   * not necessarily homologous to JavaScript Dom elements but the "mirror" class CLElement is currently used to store
   * a JS Dom element and retain other state on the JS side.
   *
   */
  class WebElement //: public BoundElement
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

  private:
    vector<WebElement> children_;
    WebElement *parent_;
    // string tag_, name_;
    string tag_, name_;
    CppType anyvalPtrType_; // C++ Data type
    void *anyvalPtr_;       // pointer to actual data
    val jsval_ = val::global("CLElement");
    const bool isAttributeOfParent_;
    int id_;

  public:
    /**
     * @brief Construct a new Web Element object
     *
     * @param id Unique identifier
     * @param tag HTML tag of this element
     * @param anyvalPtrType C++ type of data contained within
     * @param isAttributeOfParent Some WEs represent actual DOM children of their parent and others are simply attributes
     */
    WebElement(const string &name, const string &tag, const CppType anyvalPtrType, const bool isAttributeOfParent = false) : tag_(tag),
                                                                                                                             name_(name),
                                                                                                                             anyvalPtrType_(anyvalPtrType),
                                                                                                                             isAttributeOfParent_(isAttributeOfParent)

    {
      val CLContext = val::global("CLElement");
      id_ = tm.getNext();
      jsval_ = CLContext.new_();
      jsval_.set("cpptype", val(anyvalPtrType));
      jsval_.set("tag", val(tag));
      jsval_.set("id", val(id_));
      jsval_.set("name", val(name_));
      anyvalPtr_ = nullptr;

      WebElement::switchboard[id_] = this;
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
      val domElement = this->jsval_["domElement"];
      if (anyvalPtr_ == nullptr)
        return;
      switch (this->anyvalPtrType_)
      {
      case CppType::Int:
        domElement.set("value", val(*reinterpret_cast<int *>(anyvalPtr_)));
        break;
      case CppType::Float:
        domElement.set("value", val(*reinterpret_cast<float *>(anyvalPtr_)));
        break;
      case CppType::Double:
        domElement.set("value", val(*reinterpret_cast<double *>(anyvalPtr_)));
        break;
      case CppType::String:
        domElement.set("value", val(*reinterpret_cast<string *>(anyvalPtr_)));
        break;
      case CppType::NoData:
      default:
        break;
      }
    }

    val updateModelFromView()
    {
      if (anyvalPtr_ == nullptr)
        return this->jsval_["anyval"];

      switch (this->anyvalPtrType_)
      {
      case CppType::Int:
        *reinterpret_cast<int *>(anyvalPtr_) = this->jsval_["anyval"].as<int>();
        cout << "C++ side: New Int Value: " << *reinterpret_cast<int *>(anyvalPtr_) << endl;
        break;
      case CppType::Float:
        *reinterpret_cast<float *>(anyvalPtr_) = this->jsval_["anyval"].as<float>();
        cout << "C++ side: New Float Value: " << *reinterpret_cast<float *>(anyvalPtr_) << endl;
        break;
      case CppType::Double:
        *reinterpret_cast<double *>(anyvalPtr_) = this->jsval_["anyval"].as<double>();
        cout << "C++ side: New Double Value: " << *reinterpret_cast<double *>(anyvalPtr_) << endl;
        break;
      case CppType::String:
        *reinterpret_cast<string *>(anyvalPtr_) = this->jsval_["anyval"].as<string>();
        cout << "C++ side: New String Value: " << *reinterpret_cast<string *>(anyvalPtr_) << endl;
        break;
      case CppType::NoData:
        cout << "C++ side: This element contains no data." << endl;
        break;
      default:
        break;
      }

      return this->jsval_["anyval"];
    }

    val getDomElementVal() const
    {
      val domElement = jsval_["domElement"];
      return domElement["value"];
    }

    bool appendChild(WebElement &child)
    {
      children_.push_back(child);
      jsval_.call<void>("appendChild", child.getJSval());
      return true; // FIXME: need to check for duplicate ids.
    }

    void addEventListenerById(const string &eventName, const string &callbackId)
    {
      jsval_.call<void>("addEventListenerById", eventName, callbackId);
    }
    //=====================
    static map<const int, WebElement *> switchboard;
    static map<string, std::function<void()>> callbackMap;

    string getTag() const { return tag_; }
    void setParent(WebElement *parent) { this->parent_ = parent; }
    WebElement *getParent() const { return this->parent_; }
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
    static TicketMachine tm;
    static val updateModelFromViewById(const int id) { return switchboard[id]->updateModelFromView(); }
    static WebElement &getCLElementById(const int id) { return *(switchboard[id]); }
    static void runCallbackById(const string &id) { callbackMap[id](); }
  };

  class ButtonElement : public WebElement
  {
  public:
    ButtonElement(const string &name, const string &tag, const CppType anyvalPtrType, const bool isAttributeOfParent = false):
     WebElement(name, tag, anyvalPtrType, isAttributeOfParent) {}
  };

  EMSCRIPTEN_BINDINGS(WebElement)
  {
    class_<WebElement>("WebElement")
        .constructor<string, string, const WebElement::CppType>(allow_raw_pointers())
        .property("tag", &WebElement::getTag)
        .property("id", &WebElement::getId)
        .property("anyvalPtrType", &WebElement::getAnyvalPtrType, &WebElement::setAnyvalPtrType)
        .function("updateModelFromView", &WebElement::updateModelFromView)
        .function("splicePtrs", &WebElement::splicePtrs, allow_raw_pointers())
        .class_function("getCLElementById", &WebElement::getCLElementById, allow_raw_pointers())
        .class_function("updateModelFromViewById", &WebElement::updateModelFromViewById, allow_raw_pointers())
        .class_function("runCallbackById", &WebElement::runCallbackById, allow_raw_pointers());
    enum_<WebElement::CppType>("WebElementCppType")
        .value("Int", WebElement::CppType::Int)
        .value("Float", WebElement::CppType::Float)
        .value("Double", WebElement::CppType::Double)
        .value("String", WebElement::CppType::String)
        .value("NoData", WebElement::CppType::NoData);
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
    cout << "s = " << s_ << ", delta = " << delta_ << endl;
    cout << "addr(s) = " << &s_ << endl;
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
class ToyControl : public clarity::WebElement
{

public:
  ToyControl(const string &name, const string &tag, const CppType anyvalPtrType) : WebElement(name, tag, anyvalPtrType)
  {
    // mainDiv_ = new clarity::WebElement("mainDiv_", "div", CppType::NoData);
    inputA_ = new clarity::WebElement("inputA_", "input", CppType::Double);
    inputB_ = new clarity::WebElement("inputB_", "input", CppType::Double);
    sliderA_ = new clarity::WebElement("sliderA_", "input", CppType::Double);
    applyButton_ = new clarity::ButtonElement("applyButton_", "button", CppType::String);
    inputA_->setAttribute("type", val("text"));
    inputB_->setAttribute("type", val("text"));
    sliderA_->setAttribute("type", val("range"));
    this->appendChild(*inputA_);
    this->appendChild(*inputB_);
    this->appendChild(*applyButton_);
    this->appendChild(*sliderA_);
    // this->appendChild(*mainDiv_);
  }

  clarity::WebElement *mainDiv_;
  clarity::WebElement *inputA_;
  clarity::WebElement *sliderA_;
  clarity::WebElement *inputB_;
  clarity::WebElement *applyButton_;
};

/**
 * @brief switchboard is where a map of all the WebElements is stored so that
 * they can be found by their id numbers.
 *
 */
map<const int, clarity::WebElement *> clarity::WebElement::switchboard;

/**
 * @brief callbackMap holds C++ functions that can be triggered from JS
 * when events like a button press or timer tick require modification of
 * the C++ model state.
 *
 */
map<string, std::function<void()>> clarity::WebElement::callbackMap;
clarity::TicketMachine clarity::WebElement::tm;

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

  ToyControl *tc = new ToyControl("tc1", "div", clarity::WebElement::CppType::NoData);
  ToyModel *tm = new ToyModel(0, 1);
  string buttonText = string("CLICK ME!");

  clarity::WebElement::callbackMap["updateModelFromView"] = [=]
  {
    cout << "BUTTTON PRESSED!\n";
    tm->iterate();
    cout << "tm->s_ = " << tm->s_ << endl;
    cout << "addr(tc->inputB_->anyvalPtr_) = " << tc->inputB_->getAnyvalPtr() << endl;
    tc->inputB_->updateViewFromModel();
    tc->inputA_->updateViewFromModel();
    tc->sliderA_->updateViewFromModel();
    tc->applyButton_->updateViewFromModel();
  };

  tc->inputA_->splicePtrs(&tm->delta_);
  tc->inputB_->splicePtrs(&tm->s_);
  tc->sliderA_->splicePtrs(&tm->s_);
  tc->applyButton_->splicePtrs(&buttonText);
  tc->sliderA_->addEventListenerById("change", "updateModelFromView");
  tc->applyButton_->addEventListenerById("click", "updateModelFromView");

  printf("Setup complete!\n");

  return 0;
}
