
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

    //===========
    // ValueElement() {}

    WebElement(string id, string tag) {}

    WebElement(const string tag, const string id, const CppType anyvalPtrType) : tag_(tag),
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

    void setAttribute(const string attr, const string value)
    {
      val domElement = jsval_["domElement"];
      domElement.call<void>("setAttribute", attr, value);
    }    

    void valueUpdated()
    {
      if (anyvalPtr_ == nullptr) return;
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
    }

    bool appendChild(WebElement &child)
    {
      children_.push_back(child);
      jsval_.call<void>("appendChild", child.getJsval());
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
    void setTag(string tag)
    {
      tag_ = tag;
      jsval_.set("tag", tag);
    }
    void setParent(WebElement *parent) { this->parent_ = parent; }
    WebElement *getParent() { return this->parent_; }
    string getId() const { return id_; }
    void setId(string id) { id_ = id; }
    val getJsval() const { return jsval_; }
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
    static void updateVal(string id) { globalMap[id]->valueUpdated(); }
    static WebElement &getCLElementById(string id) { return *(globalMap[id]); }
    static void runCallbackById(string id) { callbackMap[id](); }

  private:
    vector<WebElement> children_;
    WebElement *parent_;
    string tag_, id_;
    CppType anyvalPtrType_; // c++ Data type
    void *anyvalPtr_;       // pointer to actual data
    val jsval_ = val::global("CLElement");
  };

  EMSCRIPTEN_BINDINGS(WebElement)
  {
    class_<WebElement>("WebElement")
        .constructor<string, string, const WebElement::CppType>(allow_raw_pointers())
        .property("tag", &WebElement::getTag, &WebElement::setTag)
        .property("id", &WebElement::getId, &WebElement::setId)
        .property("anyvalPtrType", &WebElement::getAnyvalPtrType, &WebElement::setAnyvalPtrType)        
        .function("valueUpdated", &WebElement::valueUpdated)
        .function("splicePtrs", &WebElement::splicePtrs, allow_raw_pointers())
        .class_function("getCLElementById", &WebElement::getCLElementById, allow_raw_pointers())
        .class_function("updateVal", &WebElement::updateVal, allow_raw_pointers())
        .class_function("runCallbackById", &WebElement::runCallbackById, allow_raw_pointers());
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
  }

   void iterate()  {
    s_ += delta_;
    printState();
  }
  ToyModel(double s, double delta) : s_(s), delta_(delta)
  {    
    printState();
  }

//private:
  double s_, delta_;
  //int i_;
};

class ToyControl : public clarity::WebElement
{

public:
  ToyControl(string id, string tag) : WebElement(id, tag)
  {
    mainDiv_ = new clarity::WebElement("div", "mainDiv_", CppType::NoData);
    inputA_ = new clarity::WebElement("input", "inputA_", CppType::Double);
    inputB_ = new clarity::WebElement("input", "inputB_", CppType::Double);
    applyButton_ = new clarity::WebElement("button", "applyButton_", CppType::NoData);
    inputA_->setAnyvalPtrType(CppType::Double);
    inputA_->setAttribute("type", "text");
    inputB_->setAnyvalPtrType(CppType::Double);
    inputB_->setAttribute("type", "text");
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
  
  ToyControl * tc = new ToyControl("tc1", "div");
  ToyModel * tmptr = new ToyModel(0,1);
  
  clarity::WebElement::callbackMap["updateModel"] = [=]
  {
    cout << "BUTTTON PRESSED!\n";
    //cout << "a = " << a <<"\n";
    tmptr->iterate(); };

  tc->inputA_->splicePtrs(&tmptr->delta_);
  tc->inputB_->splicePtrs(&tmptr->s_);
  tc->applyButton_->addEventListenerById("click", "updateModel");
  printf("Setup complete!\n");
  // while(true) {
  //   usleep(1000);
  // }
  return 0;
}
