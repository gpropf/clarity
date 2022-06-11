
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
   * @brief The project's central class. Describes an element with push/pull behavior to syncronize a data model and
   * a web view. WebElements can contain others and complex web controls can be built up in a hierarchal fashion. The class is
   * not necessarily homologous to JavaScript Dom elements but the "mirror" class CLElement is currently used to store
   * a JS Dom element and retain other state on the JS side.
   *
   */
  class WebElement
  {
  public:
    enum class CppType : int
    {
      Int,
      Float,
      Double,
      String,
      NoData
    };

  private:
    vector<WebElement> children_;
    WebElement *parent_;
    const string tag_, id_;
    CppType anyvalPtrType_; // c++ Data type
    void *anyvalPtr_;       // pointer to actual data
    val jsval_ = val::global("CLElement");

  public:
    //===========
    // ValueElement() {}

    WebElement(const string &id, const string &tag) : id_(id), tag_(tag) {}

    WebElement(const string &id, const string &tag, const CppType anyvalPtrType) : tag_(tag),
                                                                                 id_(id),
                                                                                 anyvalPtrType_(anyvalPtrType)

    {
      val CLContext = val::global("CLElement");
      jsval_ = CLContext.new_();
      jsval_.set("cpptype", val(anyvalPtrType));
      jsval_.set("tag", val(tag));
      jsval_.set("id", val(id));
      anyvalPtr_ = nullptr;

      WebElement::globalMap[id] = this;
    }

    void setAttribute(const string &attr, const val &value)
    {
      val domElement = jsval_["domElement"];
      domElement.call<void>("setAttribute", attr, value);
    }

    void updateView()
    {
      val domElement = this->jsval_["domElement"];
      if (anyvalPtr_ == nullptr)
        return;
      switch (this->anyvalPtrType_)
      {
      case CppType::Int:
        domElement.set("value", val(*reinterpret_cast<int *>(anyvalPtr_)));
        // cout << "C++ side: New Int Value: " << *reinterpret_cast<int *>(anyvalPtr_) << endl;
        break;
      case CppType::Float:
        domElement.set("value", val(*reinterpret_cast<float *>(anyvalPtr_)));
        // cout << "updateView: C++ side: New Float Value: " << *reinterpret_cast<float *>(anyvalPtr_) << endl;
        break;
      case CppType::Double:
        domElement.set("value", val(*reinterpret_cast<double *>(anyvalPtr_)));
        // cout << "updateView: C++ side: New Double Value: " << *reinterpret_cast<double *>(anyvalPtr_) << endl;
        break;
      case CppType::String:
        domElement.set("value", val(*reinterpret_cast<string *>(anyvalPtr_)));
        // cout << "updateView: C++ side: New String Value: " << endl;
        break;
      case CppType::NoData:
        // cout << "updateView: C++ side: This element contains no data." << endl;
      default:
        // cout << "updateView: C++ side: Unknown data type!!" << endl;
        break;
      }
    }

    val updateModel()
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
        //*reinterpret_cast<float *>(anyvalPtr_) = this->jsval_["anyval"].as<float>();
        cout << "C++ side: New String Value: " << endl;
        break;
      case CppType::NoData:
        //*reinterpret_cast<float *>(anyvalPtr_) = this->jsval_["anyval"].as<float>();
        cout << "C++ side: This element contains no data." << endl;
        break;
      default:

        break;
      }
      return this->jsval_["anyval"];
    }

    val getTypedJSval() const
    {
      switch (this->anyvalPtrType_)
      {
      case CppType::Int:
        return val(*reinterpret_cast<float *>(anyvalPtr_));
        break;
      case CppType::Float:
        return val(*reinterpret_cast<float *>(anyvalPtr_));
        break;
      case CppType::Double:
        return val(*reinterpret_cast<double *>(anyvalPtr_));
        break;
      case CppType::String:
        return val(*reinterpret_cast<string *>(anyvalPtr_));
        break;
      case CppType::NoData:
        return val(nullptr);
        break;
      default:
        return val(nullptr);
        break;
      }
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
      // jsval_.call<void>("ggg", 6);
      // jsval_.set("k", 100);
      jsval_.call<void>("addEventListenerById", eventName, callbackId);
      // jsval_.call<void>("addEventListenerById", eventName, callbackId);
    }

    static map<string, WebElement *> globalMap;
    static map<string, std::function<void()>> callbackMap;

    string getTag() const { return tag_; }
    void setParent(WebElement *parent) { this->parent_ = parent; }
    WebElement *getParent() const { return this->parent_; }
    string getId() const { return id_; }
    //void setId(string id) { id_ = id; }
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
    static val updateVal(const string &id) { return globalMap[id]->updateModel(); }
    static WebElement &getCLElementById(const string &id) { return *(globalMap[id]); }
    static void runCallbackById(const string &id) { callbackMap[id](); }
    // static void recordCurrentDataValues()
    // {
    //   for (const auto &kv : globalMap)
    //   {
    //     // kv.second->setAttribute("currentVal", *(new val("Foo")));
    //     // cout << "ID: " << kv.first << endl;
    //     WebElement *wel = globalMap[kv.first];
    //     // wel->getDomElementVal();
    //     // wel->getTypedJSval();
    //   }
    // }
  };

  EMSCRIPTEN_BINDINGS(WebElement)
  {
    class_<WebElement>("WebElement")
        .constructor<string, string, const WebElement::CppType>(allow_raw_pointers())
        .property("tag", &WebElement::getTag)
        .property("id", &WebElement::getId)
        .property("anyvalPtrType", &WebElement::getAnyvalPtrType, &WebElement::setAnyvalPtrType)
        .function("valueUpdated", &WebElement::updateModel)
        .function("splicePtrs", &WebElement::splicePtrs, allow_raw_pointers())
        .class_function("getCLElementById", &WebElement::getCLElementById, allow_raw_pointers())
        .class_function("updateVal", &WebElement::updateVal, allow_raw_pointers())
        .class_function("runCallbackById", &WebElement::runCallbackById, allow_raw_pointers());
    //.class_function("recordCurrentDataValues", &WebElement::recordCurrentDataValues, allow_raw_pointers());
    enum_<WebElement::CppType>("WebElementCppType")
        .value("Int", WebElement::CppType::Int)
        .value("Float", WebElement::CppType::Float)
        .value("Double", WebElement::CppType::Double)
        .value("String", WebElement::CppType::String)
        .value("NoData", WebElement::CppType::NoData);
  }
}

class ToyModel
{
public:
  void printState() const
  {
    cout << "s = " << s_ << ", delta = " << delta_ << endl;
    cout << "addr(s) = " << &s_ << endl;
  }

  void iterate()
  {
    s_ += delta_;
    delta_ *= 0.95;
    // clarity::WebElement::recordCurrentDataValues();
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

class ToyControl : public clarity::WebElement
{

public:
  ToyControl(string id, string tag) : WebElement(tag, id)
  {
    mainDiv_ = new clarity::WebElement("mainDiv_", "div", CppType::NoData);
    inputA_ = new clarity::WebElement("inputA_", "input", CppType::Double);
    inputB_ = new clarity::WebElement("inputB_", "input", CppType::Double);
    applyButton_ = new clarity::WebElement("applyButton_", "button", CppType::NoData);
    // inputA_->setAnyvalPtrType(CppType::Double);
    inputA_->setAttribute("type", val("text"));
    // inputB_->setAnyvalPtrType(CppType::Double);
    inputB_->setAttribute("type", val("text"));
    mainDiv_->appendChild(*inputA_);
    mainDiv_->appendChild(*inputB_);
    mainDiv_->appendChild(*applyButton_);
  }
  clarity::WebElement *mainDiv_;
  clarity::WebElement *inputA_;
  clarity::WebElement *inputB_;
  clarity::WebElement *applyButton_;
};

int main()
{
  // WebElement::translators[WebElement::CPP_Type::INT] = [](WebElement& cle) { cle.anyvalPtr_};
  val CLContext = val::global("CLElement");
  if (CLContext.as<bool>())
  {
    printf("Got CLElement!\n");
  }
  else
  {
    return -1;
  }

  ToyControl *tc = new ToyControl("tc1", "div");
  ToyModel *tm = new ToyModel(0, 1);

  clarity::WebElement::callbackMap["updateModel"] = [=]
  {
    cout << "BUTTTON PRESSED!\n";
    // cout << "a = " << a <<"\n";
    // clarity::WebElement::recordCurrentDataValues();
    tm->iterate();
    cout << "tm->s_ = " << tm->s_ << endl;
    cout << "addr(tc->inputB_->anyvalPtr_) = " << tc->inputB_->getAnyvalPtr() << endl;
    // tc->inputB_->jsval_.set("onChangeActive", false);
    tc->inputB_->updateView();
    tc->inputA_->updateView();
  };

  tc->inputA_->splicePtrs(&tm->delta_);
  tc->inputB_->splicePtrs(&tm->s_);
  tc->applyButton_->addEventListenerById("click", "updateModel");
  printf("Setup complete!\n");
  // while(true) {
  //   usleep(1000);
  // }
  return 0;
}
