
#include <stdio.h>
#include <iostream>
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
    enum class Tag : int
    {
      Div,
      Button,
      Input
    };

    enum class CppType : int
    {
      Int,
      Float,
      String,
      NoData
    };

    //===========
    //ValueElement() {}

    WebElement(string id, string tag) {}

    WebElement(const string tag, string id, const CppType anyvalPtrType) : tag_(tag),
                                                                                     id_(id),
                                                                                     anyvalPtrType_(anyvalPtrType)

    {
      val CLContext = val::global("CLElement");
      jsval_ = CLContext.new_();
      jsval_.set("cpptype", val(anyvalPtrType));
      jsval_.set("tag", val(tag));
      
      jsval_.set("id", val(id));

      
      WebElement::globalMap[id] = this;
      //_jsval.set("anyval", val(_anyvalPtr));
      //_jsval.set("owner", val(this));
    }

    void valueUpdated()
    {
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
      case CppType::String:
        //*reinterpret_cast<float *>(anyvalPtr_) = this->jsval_["anyval"].as<float>();
        cout << "C++ side: New String Value: " << endl;
        break;
      case CppType::NoData:
        //*reinterpret_cast<float *>(anyvalPtr_) = this->jsval_["anyval"].as<float>();
        cout << "C++ side: New NoData Value: " << endl;
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

    static map<string, WebElement *> globalMap;

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
    class_<WebElement>("CLElement_CPP")
        .constructor<string, string, const WebElement::CppType>(allow_raw_pointers())
        .property("tag", &WebElement::getTag, &WebElement::setTag)
        .property("id", &WebElement::getId, &WebElement::setId)
        .property("anyvalPtrType", &WebElement::getAnyvalPtrType, &WebElement::setAnyvalPtrType)
        //.property("anyvalPtr", &CLElement_CPP::getAnyvalPtr, &CLElement_CPP::setAnyvalPtr)
        .function("valueUpdated", &WebElement::valueUpdated)
        .function("splicePtrs", &WebElement::splicePtrs, allow_raw_pointers())
        .class_function("getCLElementById", &WebElement::getCLElementById, allow_raw_pointers())
        .class_function("updateVal", &WebElement::updateVal, allow_raw_pointers());
    enum_<WebElement::Tag>("CLElement_CPPTag")
        .value("Div", WebElement::Tag::Div)
        .value("Button", WebElement::Tag::Button)
        .value("Input", WebElement::Tag::Input);
    enum_<WebElement::CppType>("CLElement_CPPCppType")
        .value("Int", WebElement::CppType::Int)
        .value("Float", WebElement::CppType::Float)
        .value("String", WebElement::CppType::String)
        .value("NoData", WebElement::CppType::NoData);
  }
}

class ToyModel
{
public:
  void iterate()
  {
    s_ += delta_;
  }
  ToyModel(float s, float delta) : s_(s), delta_(delta) {}

private:
  float s_, delta_;
};

class ToyControl : public clarity::WebElement
{

public:
  ToyControl(string id, string tag): WebElement(id, tag)
  {

    clarity::WebElement *mainDiv_ = new clarity::WebElement("div", "mainDiv_", CppType::NoData);
    clarity::WebElement *inputA_ = new clarity::WebElement("input", "inputA_", CppType::Float);
    clarity::WebElement *inputB_ = new clarity::WebElement("input", "inputB_", CppType::Float);
    clarity::WebElement *applyButton_ = new clarity::WebElement("button", "applyButton_", CppType::NoData);
    inputA_->setAnyvalPtrType(CppType::Float);
    inputA_->setTag("div");
    inputB_->setAnyvalPtrType(CppType::Float);
    inputB_->setTag("div");
    // mainDiv_->setAnyvalPtrType(CppType::NoData);
    mainDiv_->appendChild(*inputA_);
    mainDiv_->appendChild(*inputB_);
    mainDiv_->appendChild(*applyButton_);

    // testinput->setId("tc_delta");

    // CLElement_CPP::globalMap["tc_delta"] = testinput;
  }
};

int main()
{
  // CLElement_CPP::translators[CLElement_CPP::CPP_Type::INT] = [](CLElement_CPP& cle) { cle.anyvalPtr_};
  val CLContext = val::global("CLElement");
  if (CLContext.as<bool>())
  {
    printf("Got CLElement!\n");
  }
  else
  {
    return -1;
  }
  // testinput->splicePtrs((int *)&(delta));

  // World * worldPtr = new ToyClass();
  ToyModel tm = ToyModel(0, 1.5);
  ToyControl tc = ToyControl("tc1", "div");

  printf("Everything should be set!\n");

  return 0;
}
