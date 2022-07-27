#ifndef clarity_hpp
#define clarity_hpp

#include <stdio.h>
#include <iostream>
#include <unistd.h>

#include <map>
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <functional>

using namespace std;
using namespace emscripten;

// Run with 'python3 -m http.server 8000'

namespace clarity
{
  /**
   * @brief Supported C++ types for WebElements.
   *
   */
  enum class CppType : int
  {
    Int,
    Float,
    Double,
    String,
    NoData /// Used for things like div that hold no data.
  };

  /**
   * @brief Interface class that should be implemented by all models and
   * views. Has only the update method. Typically a model or view being
   * updated will trigger the update of it corresponding element. Models
   * are always paired with views and vice versa. Still not sure I actually
   * want to use this but if I do basically everything will be an Updateable.
   * It's possible that this class is just OO cruft.
   *
   */
  class Updateable
  {
    virtual void update() = 0;
  };

  /**
   * @brief Hands out (presumably) unique int ids with a simply incrementing counter.
   *
   */
  class TicketMachine
  {
  private:
    int id_ = 10;

  public:
    inline const int getNext() { return ++id_; }
  };

  class DynamicValue
  {
    
  public:
  
    void *valptr_ = nullptr;
    CppType cpptype_ = CppType::NoData;

  public:
    DynamicValue()
    {
      cpptype_ = CppType::NoData;
    }

    DynamicValue(void *valptr, CppType cpptype) : valptr_(valptr), cpptype_(cpptype)
    {
    }

    DynamicValue(int *ival)
    {
      valptr_ = ival;
      cpptype_ = CppType::Int;
    }

    DynamicValue(float *ival)
    {
      valptr_ = ival;
      cpptype_ = CppType::Float;
    }

    DynamicValue(double *ival)
    {
      valptr_ = ival;
      cpptype_ = CppType::Double;
    }

    DynamicValue(string *ival)
    {
      valptr_ = ival;
      cpptype_ = CppType::String;
    }
  };

}

#endif
