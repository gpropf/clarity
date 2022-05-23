
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

class CLElement_CPP
{
public:
  enum class Tag: int { div, button, input };
  static const int INT = 0;
  static const int FLOAT = 1;

  CLElement_CPP() {}

  CLElement_CPP(Tag tag, string type, string id, const int anyvalPtrType) : _tag(tag),
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

    CLElement_CPP::globalMap[id] = this;
    //_jsval.set("anyval", val(_anyvalPtr));
    //_jsval.set("owner", val(this));
  }


  void valueUpdated()
  {
    switch (this->_anyvalPtrType)
    {
    case INT:
      *reinterpret_cast<int *>(_anyvalPtr) = this->_jsval["anyval"].as<int>();
      cout << "C++ side: New Int Value: " << *reinterpret_cast<int *>(_anyvalPtr) << endl;
      break;
    case FLOAT:
      *reinterpret_cast<float *>(_anyvalPtr) = this->_jsval["anyval"].as<float>();
      cout << "C++ side: New Float Value: " << *reinterpret_cast<float *>(_anyvalPtr) << endl;
      break;
    default:
      break;
    }
  }

  bool appendChild(CLElement_CPP & child) {
  
    _children.push_back(child);
    _jsval.call<void>("appendChild", child.getJsval());
    return true; // FIXME: need to check for duplicate ids.
  }

  static map<string, CLElement_CPP *> globalMap;

  Tag getTag() const { return _tag; }
  void setTag(Tag tag) { _tag = tag; }
  void setParent(CLElement_CPP * parent) { this->_parent = parent; }
  CLElement_CPP * getParent() { return this->_parent; }
  string getId() const { return _id; }
  void setId(string id) { _id = id; }

  val getJsval() const { return _jsval; }
  void setJsval(val jsval) { _jsval = jsval; }
  void *getAnyvalPtr() const { return _anyvalPtr; }
  void setAnyvalPtr(void *valptr) { _anyvalPtr = valptr; }
  int getAnyvalPtrType() const { return _anyvalPtrType; }
  void setAnyvalPtrType(int cpp_type)
  {
    _anyvalPtrType = cpp_type;
    _jsval.set("cpptype", cpp_type);
  }
  void splicePtrs(void *worldValuePtr) { _anyvalPtr = worldValuePtr; }
  static void updateVal(string id) { globalMap[id]->valueUpdated(); }
  static CLElement_CPP &getCLElementById(string id) { return *(globalMap[id]); }

private:
  vector<CLElement_CPP> _children;
  CLElement_CPP * _parent;
  Tag _tag;
  string _type;
  string _id;
  int _anyvalPtrType;
  void *_anyvalPtr;
  val _jsval = val::global("CLElement");
};

class ToyModel {
  float s_, delta_;
  void iterate() {
    s_ += delta_;
  }
  ToyModel(float s, float delta): s_(s), delta_(delta) {}
};


class ToyControl: public CLElement_CPP
{

public:
  

  ToyControl() {
    
    
    CLElement_CPP *mainDiv_ = new CLElement_CPP(CLElement_CPP::Tag::div, "", "mainDiv_", CLElement_CPP::INT);    
    CLElement_CPP *inputA_ = new CLElement_CPP(CLElement_CPP::Tag::input, "text", "inputA_", CLElement_CPP::FLOAT);
    CLElement_CPP *inputB_ = new CLElement_CPP(CLElement_CPP::Tag::input, "text", "inputB_", CLElement_CPP::FLOAT);    
    inputA_->setAnyvalPtrType(CLElement_CPP::FLOAT);
    inputB_->setAnyvalPtrType(CLElement_CPP::FLOAT);
    mainDiv_->appendChild(*inputA_);
    mainDiv_->appendChild(*inputB_);
    
    //testinput->setId("tc_delta");
    
    //CLElement_CPP::globalMap["tc_delta"] = testinput;
  } 
  
};

EMSCRIPTEN_BINDINGS(CLElement_CPP)
{
  class_<CLElement_CPP>("CLElement_CPP")
      .constructor<CLElement_CPP::Tag, string, string, const int>(allow_raw_pointers())
      .property("tag", &CLElement_CPP::getTag, &CLElement_CPP::setTag)
      .property("id", &CLElement_CPP::getId, &CLElement_CPP::setId)
      .property("anyvalPtrType", &CLElement_CPP::getAnyvalPtrType, &CLElement_CPP::setAnyvalPtrType)
      //.property("anyvalPtr", &CLElement_CPP::getAnyvalPtr, &CLElement_CPP::setAnyvalPtr)
      .function("valueUpdated", &CLElement_CPP::valueUpdated)
      .function("splicePtrs", &CLElement_CPP::splicePtrs, allow_raw_pointers())
      .class_function("getCLElementById", &CLElement_CPP::getCLElementById, allow_raw_pointers())
      .class_function("updateVal", &CLElement_CPP::updateVal, allow_raw_pointers());
  enum_<CLElement_CPP::Tag>("Tag")
      .value("div", CLElement_CPP::Tag::div)
      .value("button", CLElement_CPP::Tag::button)
      .value("input", CLElement_CPP::Tag::input);
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
  //testinput->splicePtrs((int *)&(delta));

  // World * worldPtr = new ToyClass();
  ToyControl T;

  printf("Everything should be set!\n");

  return 0;
}
