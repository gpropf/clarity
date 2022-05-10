

#include <stdio.h>
#include <iostream>
//#include <vector>
#include <map>
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

using namespace std;
using namespace emscripten;

class CLElement_CPP
{
public:
  CLElement_CPP(string type, string id, int ival) : _type(type),
                                                    _id(id),
                                                    _ival(ival)

  {
    val CLContext = val::global("CLElement");
    _jsVal = CLContext.new_();
    _jsVal.set("type", val(type));
    _jsVal.set("id", val(id));
    _jsVal.set("value", val(ival));
    _jsVal.set("owner", this);
  }

  void valueUpdated(int newval)
  {
    this->_ival = newval;
    cout << "C++ side: Value Updated! Now: " << this->_ival << endl;
  }

  
  static map<string, CLElement_CPP *> globalMap;
  
  string getType() const { return _type; }
  void setType(string type) { _type = type; }
  string getId() const { return _id; }
  void setId(string id) { _id = id; }
  int getIval() const { return _ival; }
  void setIval(int ival)
  {
    _ival = ival;
    _jsVal.set("value", val(ival));
  }
  
  CLElement_CPP *getSelf() { return this; }

  static void updateCurrent(int newval, string id) { globalMap[id]->valueUpdated(newval); }
 

private:
  std::string _type;
  std::string _id;
  int _ival;
  val _jsVal = val::global("CLElement");
};

EMSCRIPTEN_BINDINGS(my_class_example)
{  
  class_<CLElement_CPP>("CLElement_CPP")
      .constructor<std::string, std::string, int>(allow_raw_pointers())
      .property("type", &CLElement_CPP::getType, &CLElement_CPP::setType)
      .property("id", &CLElement_CPP::getId, &CLElement_CPP::setId)
      .property("ival", &CLElement_CPP::getIval, &CLElement_CPP::setIval)
      .function("valueUpdated", &CLElement_CPP::valueUpdated)
      .function("getSelf", &CLElement_CPP::getSelf, allow_raw_pointers())
      .class_function("updateCurrent", &CLElement_CPP::updateCurrent, allow_raw_pointers());
}

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

  CLElement_CPP *testinput = new CLElement_CPP("text", string("testinput"), 39);
  CLElement_CPP::globalMap["testinput"] = testinput;

  printf("Everything should be set!\n");

  return 0;
}
