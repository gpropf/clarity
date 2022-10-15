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

#define INLINE inline
//#define INLINE /* inline */

namespace clarity {

/**
 * @brief Abstract convenience class. Overload this class and the content() method
 * and you can easily put your content in an existing template like the testbed.cpp
 * example program.
 *
 */
struct PageContent {
    virtual void content() = 0;
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

}  // namespace clarity

using namespace clarity;

#endif
