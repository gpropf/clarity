
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

    WebElement(const Tag tag, string type, string id, const CppType anyvalPtrType) : _tag(tag),
                                                                                     _type(type),
                                                                                     _id(id),
                                                                                     _anyvalPtrType(anyvalPtrType)

    {
      val CLContext = val::global("CLElement");
      _jsval = CLContext.new_();
      _jsval.set("cpptype", val(anyvalPtrType));
      _jsval.set("tag", val(tag));
      _jsval.set("type", val(type));
      _jsval.set("id", val(id));

      _default_id = _id;
      WebElement::globalMap[id] = this;
      //_jsval.set("anyval", val(_anyvalPtr));
      //_jsval.set("owner", val(this));
    }

    void valueUpdated()
    {
      switch (this->_anyvalPtrType)
      {
      case CppType::Int:
        *reinterpret_cast<int *>(_anyvalPtr) = this->_jsval["anyval"].as<int>();
        cout << "C++ side: New Int Value: " << *reinterpret_cast<int *>(_anyvalPtr) << endl;
        break;
      case CppType::Float:
        *reinterpret_cast<float *>(_anyvalPtr) = this->_jsval["anyval"].as<float>();
        cout << "C++ side: New Float Value: " << *reinterpret_cast<float *>(_anyvalPtr) << endl;
        break;
      case CppType::String:
        //*reinterpret_cast<float *>(_anyvalPtr) = this->_jsval["anyval"].as<float>();
        cout << "C++ side: New String Value: " << endl;
        break;
      case CppType::NoData:
        //*reinterpret_cast<float *>(_anyvalPtr) = this->_jsval["anyval"].as<float>();
        cout << "C++ side: New NoData Value: " << endl;
        break;
      default:
        break;
      }
    }

    bool appendChild(WebElement &child)
    {
      _children.push_back(child);
      _jsval.call<void>("appendChild", child.getJsval());
      return true; // FIXME: need to check for duplicate ids.
    }

    static map<string, WebElement *> globalMap;

    Tag getTag() const { return _tag; }
    void setTag(Tag tag)
    {
      _tag = tag;
      _jsval.set("tag", tag);
    }
    void setParent(WebElement *parent) { this->_parent = parent; }
    WebElement *getParent() { return this->_parent; }
    string getId() const { return _id; }
    void setId(string id) { _id = id; }

    val getJsval() const { return _jsval; }
    void setJsval(val jsval) { _jsval = jsval; }
    void *getAnyvalPtr() const { return _anyvalPtr; }
    void setAnyvalPtr(void *valptr) { _anyvalPtr = valptr; }
    CppType getAnyvalPtrType() const { return _anyvalPtrType; }
    void setAnyvalPtrType(CppType cppType)
    {
      _anyvalPtrType = cppType;
      _jsval.set("cpptype", cppType);
    }
    void splicePtrs(void *worldValuePtr) { _anyvalPtr = worldValuePtr; }
    static void updateVal(string id) { globalMap[id]->valueUpdated(); }
    static WebElement &getCLElementById(string id) { return *(globalMap[id]); }

  private:
    vector<WebElement> _children;
    WebElement *_parent;
    Tag _tag, _default_tag; // This is the HTML tag

    string _id, _default_id;
    string _type, _default_type; // This is the 'type' attribute in the HTML tag, NOT the data type.

    CppType _anyvalPtrType; // c++ Data type
    void *_anyvalPtr;       // pointer to actual data
    val _jsval = val::global("CLElement");
  };

  EMSCRIPTEN_BINDINGS(WebElement)
  {
    class_<WebElement>("CLElement_CPP")
        .constructor<WebElement::Tag, string, string, const WebElement::CppType>(allow_raw_pointers())
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

    clarity::WebElement *mainDiv_ = new clarity::WebElement(clarity::WebElement::Tag::Div, "", "mainDiv_", CppType::NoData);
    clarity::WebElement *inputA_ = new clarity::WebElement(clarity::WebElement::Tag::Input, "text", "inputA_", CppType::Float);
    clarity::WebElement *inputB_ = new clarity::WebElement(clarity::WebElement::Tag::Input, "text", "inputB_", CppType::Float);
    clarity::WebElement *applyButton_ = new clarity::WebElement(clarity::WebElement::Tag::Button, "", "applyButton_", CppType::NoData);
    inputA_->setAnyvalPtrType(CppType::Float);
    inputA_->setTag(Tag::Div);
    inputB_->setAnyvalPtrType(CppType::Float);
    inputB_->setTag(Tag::Div);
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
  // CLElement_CPP::translators[CLElement_CPP::CPP_Type::INT] = [](CLElement_CPP& cle) { cle._anyvalPtr};
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
