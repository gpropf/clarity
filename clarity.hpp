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

class Anytype {};

/**
 * @brief I created this class when I realized that I needed a way to
 * encapsulate data that took into account dimensionality (i.e. is this an
 * array, a matrix?, etc...) It's also probably for the best that the CppType
 * and the pointer to the datum are together now.
 *
 * @tparam CppT
 */
template <class CppT>
class Datum : public Anytype {
    int *dataDimensionality_ =
        new int[2];  //!< There is a digit for each dimension and the dimension
                     //!< list is terminated with a 0. Single valued datums thus
                     //!< have[1, 0]. A 2D grid that is 6 wide and 5
                     //!< high will have[6, 5, 0] and so on. This isn't the most
                     //!< obvious way to encode this kind of information but I
                     //!< wanted something where it was very easy to determine
                     //!< if the value is singular or more complex. Checking to
                     //!< see if the first digit is a 1 will do that.
    CppType cppType_ = CppType::NoData;
    CppT *datum_;  //!< The tparam is the type of each C++ value;

public:
    Datum(CppType cppType, CppT *datum, int *dataDimensionality)
        : cppType_(cppType),
          datum_(datum),
          dataDimensionality_(dataDimensionality) {}
};

class Translator {
    val domElement_;
    val jsval_;
};

}  // namespace clarity

using namespace clarity;

#endif
