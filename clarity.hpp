#ifndef clarity_hpp
#define clarity_hpp

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <stdio.h>
#include <unistd.h>

#include <functional>
#include <iostream>
#include <map>

using namespace std;
using namespace emscripten;

// Run with 'python3 -m http.server 8000'

namespace clarity {
/**
 * @brief Supported C++ types for WebElements.
 *
 */
enum class CppType : int {
    Int,
    Float,
    Double,
    String,
    JSFunc,
    NoData  /// Used for things like div that hold no data.
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
class Updateable {
    virtual void update() = 0;
};

/**
 * @brief Hands out (presumably) unique int ids with a simply incrementing
 * counter.
 *
 */
class TicketMachine {
   private:
    int id_ = 0;

   public:
    inline const int getNext() {
        cout << "Issuing ticket #: " << id_ + 1 << "\n";
        return ++id_;
    }
};

class Invertable {
    virtual Invertable *inverted() = 0;
};

class DatumBase {};

/**
 * @brief I created this class when I realized that I needed a way to
 * encapsulate data that took into account dimensionality (i.e. is this an
 * array, a matrix?, etc...) It's also probably for the best that the CppType
 * and the pointer to the datum are together now.
 *
 * @tparam CppT
 */
template <class CppT>
class Datum : public DatumBase {
   protected:
   public:
    /**
     * @brief There is a digit for each dimension and the dimension list is
     * terminated with a 0. Single valued datums thus have[1, 0]. A 2D grid that
     * is 6 wide and 5 high will have[6, 5, 0] and so on. This design was chosen
     * so that a simple test for a 1 as the first value would differentiate all
     * singular from all multiple values.
     *
     */
    int *dataDimensionality_ = new int[2];
    CppType cppType_ = CppType::NoData;
    CppT *datum_;  //!< The tparam is the type of each C++ value;

    // inline CppType getCppType() const { return cppType_; }
    // inline void setVal(CppT v) { *datum_ = v; }

    Datum(CppT *datum) : datum_(datum) {}

    Datum(CppType cppType, CppT *datum, int *dataDimensionality)
        : cppType_(cppType),
          datum_(datum),
          dataDimensionality_(dataDimensionality) {}
};

class TranslatorBase {
   public:
    inline virtual val text2jsval() { return val(NULL); }
    inline virtual val js2datum() { return val(NULL); }
    inline virtual void datum2js() {}

    inline virtual void setVal(const val &inval) {}
};

template <class CppT>
class Translator : public TranslatorBase {
   protected:
    val jsval_;          //!< The actual data in JS form.
    val domElement_;     //!< The element in the web page to store/retreive the
                         //!< data.
    string boundField_;  //!< Tells us what field in the domElement_ to use to
                         //!< get the jsval_ from;

   protected:
    Datum<CppT> *datum_;  //!< The datum we interact with and translate to/from

   public:
    // virtual val datum2js(Datum<CppT> *datum) { return val(NULL); }  // FIXME

    // virtual Datum<CppT> *js2Datum(const val &jsval) {
    //     return nullptr;
    // }  // FIXME

    Translator(Datum<CppT> *datum, val domElement, string boundField_)
        : datum_(datum), domElement_(domElement), boundField_(boundField_) {}

    virtual val text2jsval() {
        string valueText = domElement_[boundField_].as<string>();
        // cout << "ClarityNode::getVal() valueText = " << valueText <<
        // "\n";
        switch (datum_->cppType_) {
            case CppType::Int:
                // cout << "ClarityNode::getVal() Int\n";
                return val(stoi(valueText));
                break;
            case CppType::Float:
                // cout << "ClarityNode::getVal() Float\n";
                return val(stof(valueText));
                break;
            case CppType::Double:
                // cout << "ClarityNode::getVal() Double\n";
                return val(stod(valueText));
                break;
            case CppType::String:
                // cout << "ClarityNode::getVal() String\n";
                return val(valueText);
                break;
            case CppType::NoData:
            default:
                // cout << "ClarityNode::getVal() NoData\n";
                return val(NULL);
                break;
        }
    }

    inline virtual val js2datum() {
        val jsval = text2jsval();
        CppT cppVal = jsval.as<CppT>();
        cout << "js2datum():  Setting C++ val to: " << cppVal << "\n";
        *reinterpret_cast<CppT *>(datum_->datum_) =
            cppVal;  // jsval.as<CppT>();
        return jsval;
        //   this->cle_.template call<T>("jsToCPPVal", inval);
        // pushValToPeers(this);
    }

    inline virtual void setVal(const val &inval) {
        assert(boundField_ != "");
        // clean_ = false;

        // cle_.call<void>("printVal", inval);
        domElement_.set(boundField_, inval);
        domElement_.call<void>("setAttribute", val(boundField_), inval);
    }

    inline virtual void datum2js() {
        val jsval = val(*reinterpret_cast<CppT *>(datum_->datum_));
        setVal(jsval);
    }
};

template <class CppT>
class TranslatorInput : public Translator<CppT> {
    // boundField_ = "value";

   public:
    TranslatorInput(Datum<CppT> *datum, val domElement)
        : Translator<CppT>(datum, domElement, "value") {}
};

}  // namespace clarity

using namespace clarity;

#endif
