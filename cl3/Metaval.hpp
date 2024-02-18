/**
 * @file Channel.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief Base class for channels. The most important methods are `inject()` and `syncFrom()`.
 * Basically, those are the push and pull behaviors respectively.
 * @version 0.3
 * @date 2023-10-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef Metaval_hpp
#define Metaval_hpp

#include <memory>

#include "Util.hpp"
#include "WebElements.hpp"
// #include "WebElementChannel.hpp"

using std::cout;
using std::map;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

namespace cl3 {

class Metaval {
    val val_;

   public:
   virtual ~Metaval() {};
    virtual string toString() = 0;
};

class Metaval2D : public Metaval {
    double x_, y_;

   public:
    Metaval2D(double x, double y) : x_(x), y_(y) {}

    virtual string toString() { return std::to_string(x_) + string(", ") + std::to_string(y_); };
};

class MetavalInt : public Metaval {
    int i_;

   public:
    MetavalInt(int i) : i_(i) {}

    virtual string toString() { return std::to_string(i_); };
};

class MetavalString : public Metaval {
    string s_;

   public:
    MetavalString(string s) : s_(s) {}

    virtual string toString() { return s_; };
};

EMSCRIPTEN_BINDINGS(Metaval) {
    emscripten::class_<Metaval>("Metaval")
        //.function("inject", &Channel::inject)
        .smart_ptr<std::shared_ptr<cl3::Metaval>>("Metaval");
    emscripten::class_<MetavalInt>("MetavalInt")
        //.function("inject", &Channel::inject)
        .smart_ptr<std::shared_ptr<cl3::MetavalInt>>("MetavalInt");
    emscripten::class_<Metaval2D>("Metaval2D")
        //.function("inject", &Channel::inject)
        .smart_ptr<std::shared_ptr<cl3::Metaval2D>>("Metaval2D");
    emscripten::class_<MetavalString>("MetavalString")
        //.function("inject", &Channel::inject)
        .smart_ptr<std::shared_ptr<cl3::MetavalString>>("MetavalString");
}

};  // namespace cl3

#endif