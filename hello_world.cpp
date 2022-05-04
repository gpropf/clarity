/*
 * Copyright 2011 The Emscripten Authors.  All rights reserved.
 * Emscripten is available under two separate licenses, the MIT license and the
 * University of Illinois/NCSA Open Source License.  Both these licenses can be
 * found in the LICENSE file.
 */

#include <stdio.h>
#include <iostream>
#include <emscripten/val.h>


using namespace std;
using namespace emscripten;

int main() {
  val AudioContext = val::global("CLElement");
  if (AudioContext.as<bool>()) {
    printf("Got CLElement!\n");
    
  }
  else {
    return -1;
  }

  printf("Got an AudioContext\n");
  val context = AudioContext.new_();
  val oscillator = context.call<val>("createCLElement");

  
  

  printf("Configuring oscillator\n");
  oscillator.set("_type", val("triangle"));
  oscillator.set("_id", val("tf1"));
  
  //return 0;
  
  //oscillator["frequency"].set("value", val(261.63)); // Middle C

  printf("Playing\n");
  string tstr = oscillator.call<string>("get_type");
  //oscillator.call<void>("start", 0);

  printf("tstr: %s, All done!\n", tstr.c_str());

  return 0;
}