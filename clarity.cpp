

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

class CLElement_CPP
{
public:
  enum CPP_Type
  {
    INT = 0,
    UINT = 1,
    FLOAT = 2,
    DOUBLE = 3,
    STRING = 4
  };
  // static map<CPP_Type, std::function<void(CLElement_CPP&)>> translators;

  CLElement_CPP(string domtype, string id, int ival) : _domtype(domtype),
                                                       _id(id),
                                                       _ival(ival)

  {
    val CLContext = val::global("CLElement");
    _jsVal = CLContext.new_();
    //_jsVal.set("cpptype", val(int(0)));
    _jsVal.set("domtype", val(domtype));
    _jsVal.set("id", val(id));
    _jsVal.set("value", val(ival));
    _jsVal.set("owner", this);
  }

  void intValueUpdated(int newval)
  {
    this->_ival = newval;
    cout << "C++ side: Value Updated! Now: " << this->_ival << endl;
  }

  // void valueUpdated(void* newvalPtr)
  // {
  //   switch(this->_anyvalPtrType) {
  //     case INT:
  //       _anyvalPtr = reinterpret_cast<int*>(newvalPtr);

  //     break;
  //     case FLOAT:

  //     break;
  //   }

  //   // this->_ival = newval;
  //   // cout << "C++ side: Value Updated! Now: " << this->_ival << endl;
  // }

  void valueUpdated(int newInt)
  {
    //*(int*)_anyvalPtr = newInt;
    *reinterpret_cast<int *>(_anyvalPtr) = newInt;
    cout << "C++ side: using reinterpret_cast! New value Now: " << *reinterpret_cast<int *>(_anyvalPtr) << endl;
  }

  // void valueUpdated(float newFloat)
  // {
  //   //*(int*)_anyvalPtr = newInt;
  //   *reinterpret_cast<float *>(_anyvalPtr) = newFloat;
  //   cout << "C++ side: using reinterpret_cast! New value Now: " << *reinterpret_cast<float *>(_anyvalPtr) << endl;
  // }

  static map<string, CLElement_CPP *> globalMap;

  string getDomType() const { return _domtype; }
  void setDomType(string domtype) { _domtype = domtype; }
  string getId() const { return _id; }
  void setId(string id) { _id = id; }
  void *getAnyvalPtr() const { return _anyvalPtr; }
  void setAnyvalPtr(void *valptr) { _anyvalPtr = valptr; }
  int getAnyvalPtrType() const { return _anyvalPtrType; }
  void setAnyvalPtrType(int cpp_type) { _anyvalPtrType = cpp_type; _jsVal.set("cpptype", cpp_type); }
  int getIval() const { return _ival; }
  void setIval(int ival)
  {
    _ival = ival;
    _jsVal.set("value", val(ival));
  }

  CLElement_CPP *getSelf() { return this; }

  static void updateIntVal(int newval, string id) { globalMap[id]->intValueUpdated(newval); }
  static void updateVal(int newval, string id) { globalMap[id]->valueUpdated(newval); }
  //static void updateVal(float newval, string id) { globalMap[id]->valueUpdated(newval); }
  static CLElement_CPP & getCLElementById(string id) { return *globalMap[id]; }

private:
  std::string _domtype;
  std::string _id;
  int _ival;
  int _anyvalPtrType;
  void *_anyvalPtr;
  val _jsVal = val::global("CLElement");
};

EMSCRIPTEN_BINDINGS(CLElement_CPP)
{
  class_<CLElement_CPP>("CLElement_CPP")
      .constructor<std::string, std::string, int>(allow_raw_pointers())
      .property("domtype", &CLElement_CPP::getDomType, &CLElement_CPP::setDomType)
      .property("id", &CLElement_CPP::getId, &CLElement_CPP::setId)
      .property("ival", &CLElement_CPP::getIval, &CLElement_CPP::setIval)
      .property("anyvalPtrType", &CLElement_CPP::getAnyvalPtrType, &CLElement_CPP::setAnyvalPtrType)
      .function("intValueUpdated", &CLElement_CPP::intValueUpdated)      
      .function("valueUpdated", &CLElement_CPP::valueUpdated)
      .function("getSelf", &CLElement_CPP::getSelf, allow_raw_pointers())
      .class_function("updateIntVal", &CLElement_CPP::updateIntVal, allow_raw_pointers())
      .class_function("getCLElementById", &CLElement_CPP::getCLElementById, allow_raw_pointers())
      .class_function("updateVal", &CLElement_CPP::updateVal, allow_raw_pointers());
  ;
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

  CLElement_CPP *testinput = new CLElement_CPP("text", string("testinput"), 39);
  testinput->setAnyvalPtrType(int(1));
  testinput->setId("testinput");
  CLElement_CPP::globalMap["testinput"] = testinput;

  printf("Everything should be set!\n");

  return 0;
}
