/*
 * Copyright 2011 The Emscripten Authors.  All rights reserved.
 * Emscripten is available under two separate licenses, the MIT license and the
 * University of Illinois/NCSA Open Source License.  Both these licenses can be
 * found in the LICENSE file.
 */

// Compile with: emcc --bind -o hello_embind.js hello_world.cpp

#include <stdio.h>
#include <iostream>
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

  string getType() const { return _type; }
  void setType(string type) { _type = type; }
  string getId() const { return _id; }
  void setId(string id) { _id = id; }
  int getIval() const { return _ival; }
  void setIval(int ival) { _ival = ival; }
  void syncOwner(CLElement_CPP * owner) {
    //_jsVal.set("owner", this);
    //return ownerPtr;
    _jsVal.set("owner", owner);
  }
  void setOwner(CLElement_CPP * owner) {
    //_jsVal.set("owner", this);
    //return ownerPtr;
    _jsVal.set("owner", owner);
  }
  CLElement_CPP getSelf() { return *this; }
  //void setSelf(CLElement_CPP * newSelf) { _self = newSelf; }
  void valueUpdated() {
    cout << "Value Updated!" << endl;
  }

  

private:
  std::string _type;
  std::string _id;
  int _ival;
  //CLElement_CPP * _self;
  val _jsVal = val::global("CLElement");
};


EMSCRIPTEN_BINDINGS(my_class_example)
{
  

  class_<CLElement_CPP>("CLElement_CPP")
      .constructor<std::string, std::string, int>(allow_raw_pointers())
      //.function("incrementX", &MyClass::incrementX)
      .property("type", &CLElement_CPP::getType, &CLElement_CPP::setType)
      .property("id", &CLElement_CPP::getId, &CLElement_CPP::setId)
      .property("ival", &CLElement_CPP::getIval, &CLElement_CPP::setIval)
      //.property("self", &CLElement_CPP::getSelf, &CLElement_CPP::setSelf)
      //.property("clContext", &CLElement_CPP::getCLContext, &CLElement_CPP::setCLContext)
      .function("valueUpdated", &CLElement_CPP::valueUpdated)
      .function("setOwner", &CLElement_CPP::setOwner, allow_raw_pointers())
      .function("getSelf", &CLElement_CPP::getSelf, allow_raw_pointers())
      .function("syncOwner", &CLElement_CPP::syncOwner, allow_raw_pointers());

    
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
  CLElement_CPP testinput = CLElement_CPP("text", "testinput", 3);
  testinput.syncOwner(&testinput);

  // testinput.setIval(106);
  // testinput.value = 444;
  //  printf("Got a CLContext\n");
  //  val context = CLContext.new_();
  //  val test_input = context.call<val>("createCLElement");

  // printf("Configuring testinput\n");
  // test_input.set("type", val("text"));
  // test_input.set("id", val("testinput"));

  // printf("Playing\n");

  // test_input.set("value", val(101));

  // // string tstr = test_input.call<string>("get_type");

  // printf("tstr: %s, All done!\n", tstr.c_str());

  return 0;
}
