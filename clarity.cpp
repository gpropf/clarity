
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

class CLElement_CPP
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
  CLElement_CPP() {}

  CLElement_CPP(const Tag tag, string type, string id, const CppType anyvalPtrType) : _tag(tag),
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
    CLElement_CPP::globalMap[id] = this;
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

  bool appendChild(CLElement_CPP &child)
  {

    _children.push_back(child);
    _jsval.call<void>("appendChild", child.getJsval());
    return true; // FIXME: need to check for duplicate ids.
  }

  static map<string, CLElement_CPP *> globalMap;

  Tag getTag() const { return _tag; }
  void setTag(Tag tag)
  {
    _tag = tag;
    _jsval.set("tag", tag);
  }
  void setParent(CLElement_CPP *parent) { this->_parent = parent; }
  CLElement_CPP *getParent() { return this->_parent; }
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
  static CLElement_CPP &getCLElementById(string id) { return *(globalMap[id]); }

private:
  vector<CLElement_CPP> _children;
  CLElement_CPP *_parent;
  Tag _tag, _default_tag; // This is the HTML tag

  string _id, _default_id;
  string _type, _default_type; // This is the 'type' attribute in the HTML tag, NOT the data type.

  CppType _anyvalPtrType; // c++ Data type
  void *_anyvalPtr;       // pointer to actual data
  val _jsval = val::global("CLElement");
};

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

class ToyControl : public CLElement_CPP
{

public:
  ToyControl()
  {

    CLElement_CPP *mainDiv_ = new CLElement_CPP(CLElement_CPP::Tag::Div, "", "mainDiv_", CppType::NoData);
    CLElement_CPP *inputA_ = new CLElement_CPP(CLElement_CPP::Tag::Input, "text", "inputA_", CppType::Float);
    CLElement_CPP *inputB_ = new CLElement_CPP(CLElement_CPP::Tag::Input, "text", "inputB_", CppType::Float);
    CLElement_CPP *applyButton_ = new CLElement_CPP(CLElement_CPP::Tag::Button, "", "applyButton_", CppType::NoData);
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

EMSCRIPTEN_BINDINGS(CLElement_CPP)
{
  class_<CLElement_CPP>("CLElement_CPP")
      .constructor<CLElement_CPP::Tag, string, string, const CLElement_CPP::CppType>(allow_raw_pointers())
      .property("tag", &CLElement_CPP::getTag, &CLElement_CPP::setTag)
      .property("id", &CLElement_CPP::getId, &CLElement_CPP::setId)
      .property("anyvalPtrType", &CLElement_CPP::getAnyvalPtrType, &CLElement_CPP::setAnyvalPtrType)
      //.property("anyvalPtr", &CLElement_CPP::getAnyvalPtr, &CLElement_CPP::setAnyvalPtr)
      .function("valueUpdated", &CLElement_CPP::valueUpdated)
      .function("splicePtrs", &CLElement_CPP::splicePtrs, allow_raw_pointers())
      .class_function("getCLElementById", &CLElement_CPP::getCLElementById, allow_raw_pointers())
      .class_function("updateVal", &CLElement_CPP::updateVal, allow_raw_pointers());
  enum_<CLElement_CPP::Tag>("CLElement_CPPTag")
      .value("Div", CLElement_CPP::Tag::Div)
      .value("Button", CLElement_CPP::Tag::Button)
      .value("Input", CLElement_CPP::Tag::Input);
  enum_<CLElement_CPP::CppType>("CLElement_CPPCppType")
      .value("Int", CLElement_CPP::CppType::Int)
      .value("Float", CLElement_CPP::CppType::Float)
      .value("String", CLElement_CPP::CppType::String)
      .value("NoData", CLElement_CPP::CppType::NoData);
}

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
  ToyControl tc = ToyControl();

  printf("Everything should be set!\n");

  return 0;
}
