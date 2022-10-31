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
// #include <vector>
// #include <string>

using namespace std;
using namespace emscripten;

// Run with 'python3 -m http.server 8000'

#define INLINE inline
//#define INLINE /* inline */

namespace clarity {

template <typename T>
string interpolateTypeIntoString(string &inStr) {
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

}  // namespace clarity

using namespace clarity;

#endif
