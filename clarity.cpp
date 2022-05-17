
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

class World
{
  virtual void iterate() = 0;
  virtual void printState() = 0;
};

class ToyClass
{
public:
  float total = 0;
  float delta = 0;
  void iterate()
  {
    total += delta;
  }
  void printState()
  {
    cout << "Total: " << total << " , delta: " << delta << endl;
  }
};

class CLElement_CPP
{
public:
  // enum CPP_Type
  // {
  //   INT = int(0),
  //   UINT = 1,
  //   FLOAT = 2,
  //   DOUBLE = 3,
  //   STRING = 4
  // };

  static const int INT = 0;
  static const int FLOAT = 1;

  CLElement_CPP(string domtype, string id, const int &anyvalPtrType, void *world) : _domtype(domtype),
                                                                                    _id(id), _world(world)
  {
    val CLContext = val::global("CLElement");
    _jsVal = CLContext.new_();
    _jsVal.set("cpptype", val(anyvalPtrType));
    _jsVal.set("domtype", val(domtype));
    _jsVal.set("id", val(id));
    //_jsVal.set("anyval", val(_anyvalPtr));
    _jsVal.set("owner", this);
  }

  void valueUpdated()
  {
    //*(int*)_anyvalPtr = newInt;
    //*reinterpret_cast<int *>(_anyvalPtr) = newInt;
    cout << "C++ side: New Value!" << endl;
  }

  static map<string, CLElement_CPP *> globalMap;

  string getDomType() const { return _domtype; }
  void setDomType(string domtype) { _domtype = domtype; }
  string getId() const { return _id; }
  void setId(string id) { _id = id; }
  void *getAnyvalPtr() const { return _anyvalPtr; }
  void setAnyvalPtr(void *valptr) { _anyvalPtr = valptr; }
  int getAnyvalPtrType() const { return _anyvalPtrType; }
  void setAnyvalPtrType(int cpp_type)
  {
    _anyvalPtrType = cpp_type;
    _jsVal.set("cpptype", cpp_type);
  }
  void splicePtrs(void *worldValuePtr) { _anyvalPtr = worldValuePtr; }
  static void updateVal(string id) { globalMap[id]->valueUpdated(); }
  static CLElement_CPP &getCLElementById(string id) { return *globalMap[id]; }

private:
  void (*updateCallback)();
  void *_world;
  std::string _domtype;
  std::string _id;
  int _anyvalPtrType;
  void *_anyvalPtr;
  val _jsVal = val::global("CLElement");
};

EMSCRIPTEN_BINDINGS(CLElement_CPP)
{
  class_<CLElement_CPP>("CLElement_CPP")
      .constructor<std::string, std::string, const int &, void *>(allow_raw_pointers())
      .property("domtype", &CLElement_CPP::getDomType, &CLElement_CPP::setDomType)
      .property("id", &CLElement_CPP::getId, &CLElement_CPP::setId)
      .property("anyvalPtrType", &CLElement_CPP::getAnyvalPtrType, &CLElement_CPP::setAnyvalPtrType)
      //.property("anyvalPtr", &CLElement_CPP::getAnyvalPtr, &CLElement_CPP::setAnyvalPtr)
      .function("valueUpdated", &CLElement_CPP::valueUpdated)
      .function("splicePtrs", &CLElement_CPP::splicePtrs, allow_raw_pointers())
      .class_function("getCLElementById", &CLElement_CPP::getCLElementById, allow_raw_pointers())
      .class_function("updateVal", &CLElement_CPP::updateVal, allow_raw_pointers());
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

  

  // World * worldPtr = new ToyClass();
  ToyClass T;

  void cb() {
    
  };

  int i = 39;
  const int testinputType = CLElement_CPP::FLOAT;
  CLElement_CPP *testinput = new CLElement_CPP("text", string("testinput"), testinputType, &T);
  testinput->setAnyvalPtrType(testinputType);
  testinput->setId("testinput");
  testinput->splicePtrs((float *)&(T.delta));
  CLElement_CPP::globalMap["testinput"] = testinput;

  printf("Everything should be set!\n");

  return 0;
}

// ============================================================

// void intValueUpdated(int newval)
// {
//   this->_ival = newval;
//   cout << "C++ side: Value Updated! Now: " << this->_ival << endl;
// }

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

// void valueUpdated(float newFloat)
// {
//   //*(int*)_anyvalPtr = newInt;
//   *reinterpret_cast<float *>(_anyvalPtr) = newFloat;
//   cout << "C++ side: using reinterpret_cast! New value Now: " << *reinterpret_cast<float *>(_anyvalPtr) << endl;
// }