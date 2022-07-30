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
    JSFunc,
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
    int id_ = 0;

  public:
    inline const int getNext() { return ++id_; }
  };

  class Invertable
  {
    virtual Invertable *inverted() = 0;
  };

  template <typename T>
  class Dynval
  {

    T *valptr_;

  public:
    Dynval(T val)
    {
      valptr_ = new T(val);
    }

    Dynval(T *valptr)
    {
      valptr_ = valptr;
    }

    ~Dynval()
    {
      delete valptr_;
    }
  };

  class DynamicValue
  {

  public:
    void *valptr_ = nullptr;
    CppType cpptype_ = CppType::NoData;

    // DynamicValue operator*(const DynamicValue &y)
    // {
    //   //...
    // }

    DynamicValue *inverted()
    {
      switch (cpptype_)
      {
        {
        case CppType::Int:
          cout << "inverted called for Int, returning 0.\n";
          return new DynamicValue((int)0);
          break;
        case CppType::Float:
          cout << "inverted called for Float\n";
          return nullptr;
          break;
        case CppType::Double:
          cout << "inverted called for Double\n";
          return new DynamicValue(1 / *reinterpret_cast<double *>(valptr_));
          break;
        case CppType::String:
          cout << "inverted called for String\n";
          return nullptr;
          break;
        case CppType::NoData:
          cout << "inverted called for NoData\n";
          return nullptr;
          break;
        default:
          return nullptr;
          break;
        }
      }
    }

    ~DynamicValue()
    {
      switch (cpptype_)
      {
        {
        case CppType::Int:
          delete reinterpret_cast<int *>(valptr_);
          cout << "Destructor called for Int\n";
          break;
        case CppType::Float:
          delete reinterpret_cast<float *>(valptr_);
          cout << "Destructor called for Float\n";
          break;
        case CppType::Double:
          //*reinterpret_cast<double *>(anyvalPtr_) = this->jsval_["anyval"].as<double>();
          delete reinterpret_cast<double *>(valptr_);
          cout << "Destructor called for Double\n";
          break;
        case CppType::String:
          delete reinterpret_cast<string *>(valptr_);
          cout << "Destructor called for String\n";
          break;
        case CppType::NoData:
          cout << "Destructor called for NoData\n";
          break;
        default:
          break;
        }
      }
    }

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

    DynamicValue(int ival)
    {
      valptr_ = new int(ival);
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

    DynamicValue(double dval)
    {
      valptr_ = new double(dval);
      cpptype_ = CppType::Double;
    }

    DynamicValue(string *ival)
    {
      valptr_ = ival;
      cpptype_ = CppType::String;
    }

    // DynamicValue inverted() {

    // }
  };

}

#endif
