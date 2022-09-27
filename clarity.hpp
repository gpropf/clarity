#ifndef clarity_hpp
#define clarity_hpp

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <stdio.h>
#include <unistd.h>

#include <array>
#include <functional>
#include <iostream>
#include <map>

using namespace std;
using namespace emscripten;

// Run with 'python3 -m http.server 8000'

namespace clarity {

static val CLElement_ = val::global("CLElement");
static val Util_ = val::global("Util");
// static val CLElement_;

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

class DatumBase {
   public:
    /**
     * @brief There is a digit for each dimension and the dimension list is
     * terminated with a 0. Single valued datums thus have[1, 0]. A 2D grid that
     * is 6 wide and 5 high will be denoted by [6, 5, 0] and so on. This design
     * was chosen so that a simple test for a 1 as the first value would
     * differentiate all singular from all multiple values.
     *
     */
    const int *const dataDimensionality_;
    CppType cppType_ = CppType::NoData;

    DatumBase(const CppType cppType, const int *const dataDimensionality)
        : dataDimensionality_(dataDimensionality), cppType_(cppType) {}
};

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
    CppT *cptr_ = nullptr;  //!< The tparam is the type of each C++ value;

    // inline CppType getCppType() const { return cppType_; }
    // inline void setVal(CppT v) { *datum_ = v; }

    Datum(CppT *cptr) : cptr_(cptr) {}

    Datum(CppType cppType, CppT *cptr, const int *const dataDimensionality)
        : DatumBase(cppType, dataDimensionality) {
        cptr_ = cptr;
    }

    Datum(Datum *d) {
        cppType_ = d->cppType_;
        dataDimensionality_ = d->dataDimensionality_;
        cptr_ = nullptr;
    }
};

class TranslatorBase {
   public:
    // static val CLElement__;  // = val::global("CLElement");
    inline virtual val text2jsval() { return val(NULL); }
    inline virtual val js2datum() { return val(NULL); }
    inline virtual void datum2js() {}
    inline virtual void setVal(const val &inval) {}

    TranslatorBase(val domElement, string boundField)
        : domElement_(domElement), boundField_(boundField) {}

   protected:
    val jsval_;          //!< The actual data in JS form.
    val domElement_;     //!< The element in the web page to store/retreive the
                         //!< data.
    string boundField_;  //!< Tells us what field in the domElement_ to use to
                         //!< get the jsval_ from;
};

template <class CppT>
class Translator : public TranslatorBase {
   protected:
    Datum<CppT> *datum_;  //!< The datum we interact with and translate to/from

   public:
    Translator(val domElement, string boundField)
        : TranslatorBase(domElement, boundField) {}

    Translator(Datum<CppT> *datum, val domElement, string boundField)
        : Translator(domElement, boundField) {
        datum_ = datum;
    }

    Translator(Translator &t) {
        jsval_ = val(NULL);
        domElement_ = val(NULL);
        boundField_ = t.boundField_;
        datum_ = nullptr;
    }

    inline void setDatum(Datum<CppT> *d) { datum_ = d; }

    // Translator(Datum<CppT> *d) {
    //     jsval_ = val(NULL);
    //     domElement_ = val(NULL);

    //     datum_ = d;
    // }

    virtual val text2jsval() {
        string valueText = domElement_[boundField_].template as<string>();
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

    /**
     * @brief Update the datum with the value from the domElement.
     *
     * @return val
     */
    inline virtual val js2datum() {
        val jsval = text2jsval();
        CppT cppVal = jsval.as<CppT>();
        cout << "js2datum():  Setting C++ val to: " << cppVal << "\n";
        *reinterpret_cast<CppT *>(datum_->cptr_) = cppVal;
        return jsval;
        //   this->cle_.template call<T>("jsToCPPVal", inval);
        // pushValToPeers(this);
    }

    /**
     * @brief Set the appropriate field in the domElement to the incoming val.
     *
     * @param inval
     */
    inline virtual void setVal(const val &inval) {
        assert(boundField_ != "");
        CLElement_.call<void>("printVal_st", inval,
                              val("Translator::setVal()"));
        domElement_.set(boundField_, inval);
        domElement_.call<void>("setAttribute", val(boundField_), inval);
    }

    /**
     * @brief Update the domElement with the value from the datum.
     *
     */
    inline virtual void datum2js() {
        val jsval = val(*reinterpret_cast<CppT *>(datum_->cptr_));
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

template <class CppT>
class TranslatorCanvasGrid : public Translator<CppT> {
    // boundField_ = "value";
   protected:
    int pixelWidth_ = this->domElement_["width"].template as<int>();
    int pixelHeight_ = this->domElement_["height"].template as<int>();
    int gridWidth_ = this->datum_->dataDimensionality_[0];
    int gridHeight_ = this->datum_->dataDimensionality_[1];
    int cellWidth_ = pixelWidth_ / gridWidth_;
    int cellHeight_ = pixelHeight_ / gridHeight_;
    CppT currentCellVal_ = 0;

   public:
    int setValXY(int x, int y) {
        int addr = (y * gridWidth_ + x) * sizeof(CppT);
        *(this->datum_->cptr_ + addr) = currentCellVal_;
        return addr;
    }

    TranslatorCanvasGrid(Datum<CppT> *datum, val domElement)
        : Translator<CppT>(datum, domElement, "SPECIAL") {
        // EM_JS(void, clickAlert, (), { alert('hello world!'); });
        // EM_ASM(function clickAlert() { alert("Hello World!"); });
        val cellDimensions = val::object();
        cellDimensions.set("w", cellWidth_);
        cellDimensions.set("h", cellHeight_);
        val gridDimensions = val::object();
        gridDimensions.set("w", gridWidth_);
        gridDimensions.set("h", gridHeight_);
        // CLElement_.set("cellDimensions", cellDimensions);

        this->domElement_.set("translatorRef", this);
        this->domElement_.set("cellDimensions", cellDimensions);
        this->domElement_.set("gridDimensions", gridDimensions);

        this->domElement_.template call<val>(
            "addEventListener", val("click"),
            CLElement_["setGridLocToCurrentVal"]);
    }

    static const array<string, 8> colors;

    inline void setCurrentCellVal(CppT v) { currentCellVal_ = v; }

    inline virtual void datum2js() {
        val ctx = this->domElement_.template call<val>("getContext", val("2d"));
        ctx.set("fillStyle", "blue");
        int width = this->datum_->dataDimensionality_[0];
        int height = this->datum_->dataDimensionality_[1];
        cout << "pixelWidth_ = " << pixelWidth_ << "\n";
        int cellCount = 0;
        for (int i = 0; i < gridWidth_; i++) {
            for (int j = 0; j < gridHeight_; j++) {
                int addr = gridWidth_ * j + i;
                unsigned char v = reinterpret_cast<unsigned char>(
                    *(this->datum_->cptr_ + addr));

                // ctx.set("fillStyle", "rgba(" + to_string(v % 13 * 20) +
                //                          "," + to_string(v % 17 * 20) +
                //                          "," + to_string(v % 23 * 20) +
                //                          ", 255)");

                ctx.set("fillStyle", colors[v]);

                ctx.call<void>("fillRect", val(i * cellWidth_),
                               val(j * cellHeight_), val(cellWidth_),
                               val(cellHeight_));
            }
            // cout << "\n";
            cellCount++;
        }
    }
};

}  // namespace clarity

using namespace clarity;

#endif
