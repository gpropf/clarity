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
#include <sstream>

using namespace std;
using namespace emscripten;

/**
 * @brief This macro is here so I can test varying amounts of inlining to see if performance
 * changes. Basically, I'll use my INLINE macro if I'm not sure something should really be inlined.
 * Then you can change it to second definition to effectively de-inline those things declared with
 * it.
 *
 */
#define INLINE inline
//#define INLINE /* inline */

namespace clarity {

/**
 * @brief Given a string with something like '<T>' in the middle of it, this will use the typeid
 * operator to interpolate in the actual type. C++20 supposedly has simpler string interpolation
 * features but we're still using C++17.
 *
 * @tparam T
 * @param inStr
 * @return string
 */
template <typename T>
string interpolateTypeIntoString(string& inStr) {
    string tid = typeid(T).name();
    string beforeAngleBracket;
    string afterAngleBracket;
    std::stringstream streamData(inStr);
    char separator = '<';
    getline(streamData, beforeAngleBracket, separator);
    separator = '>';
    getline(streamData, afterAngleBracket, separator);
    separator = '\n';
    getline(streamData, afterAngleBracket, separator);
    return string(beforeAngleBracket + "<" + tid + ">" + afterAngleBracket);
}

enum class ClogType : unsigned char { WARNING = 0, ERROR = 1, INFO = 2 };
void clog(const string& msg, ClogType clt = ClogType::INFO);

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

template <typename V>
struct NumWrapper {
    NumWrapper(V* v, V modulus = 0) : v_(v), modulus_(modulus) {}
    inline void addWrappedValTo(NumWrapper<V> i) { _addTo(*i.v_); }
    inline void addTo(V i) { _addTo(i); }
    inline V getVal() { return *v_; }
    V* v_;
    V modulus_;

   private:
    inline void _addTo(V i) {
        *v_ += i;
        if (modulus_ != 0) {
            *v_ = *v_ % modulus_;
        }
    }
};

}  // namespace clarity

using namespace clarity;

#endif
