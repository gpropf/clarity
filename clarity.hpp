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
    inline const int getNext() { return ++id_; }
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

   public:
    Datum(CppType cppType, CppT *datum, int *dataDimensionality)
        : cppType_(cppType),
          datum_(datum),
          dataDimensionality_(dataDimensionality) {}
};

class TranslatorBase {};

template <class CppT>
class Translator : public TranslatorBase {
   protected:
    val jsval_;          //!< The actual data in JS form.
    val domElement_;     //!< The element in the web page to store/retreive the
                         //!< data.
    string boundField_;  //!< Tells us what field in the domElement_ to use to
                         //!< get the jsval_ from;

   public:
   protected:
    Datum<CppT> *datum_;  //!< The datum we interact with and translate to/from

   public:
    virtual val datum2js(DatumBase &d) {}

    virtual DatumBase js2Datum(const val &jsval) {}

    Translator(Datum<CppT> *datum, val domElement, string boundField_)
        : datum_(datum), domElement_(domElement), boundField_(boundField_) {}

    val text2jsval() const {
        string valueText = domElement_[boundField_].as<string>();
        // cout << "ClarityNode::getVal() valueText = " << valueText << "\n";
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
};

template <class CppT>
class TranslatorInput : public Translator<CppT> {
    // boundField_ = "value";

   public:
    TranslatorInput(Datum<CppT> *datum, val domElement)
        : Translator<CppT>(datum, domElement, "boundField") {}
};

}  // namespace clarity

using namespace clarity;

#endif
