/**
 * @file Util.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief Utility code and classes for the CL3 revision of Clarity.
 * @version 0.3
 * @date 2023-10-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef Util_hpp
#define Util_hpp

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <iostream>
#include <string>

using emscripten::val;
using std::cout;
using std::endl;
using std::map;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;

namespace cl3 {

/**
 * @brief I'm attempting to overload 'cout' in hopes of making a debug function that prints
 * conditionally. I does't work yet.
 *
 */
struct Debug : public std::ostream {
    Debug() = default;
    static const int INFO = 1;
    static const int WARNING = 2;
    static const int ERROR = 4;

    static int LEVEL;

    Debug& operator<<(int input) {
        // do something with input
        cout << (input + 1);
        return *this;
    }

    Debug& operator<<(char e) {
        // do something with input
        cout << e;
        return *this;
    }

    // static void dout
};

/**
 * @brief Basic wrapper around the JS function of this name since I anticipate needing this a lot.
 * Doesn't offer the optional extra params like the actual JS one.
 *
 * @param jsFunc The function to call.
 * @param delay Interval in ms to wait between calls.
 * @return int The interval id.
 */
int setInterval(val jsFunc, int delay) {
    val setInterval = val::global("setInterval");
    return setInterval(jsFunc, val(delay)).as<int>();
}

/**
 * @brief Used as a base class for things that need a unique identifier.
 *
 */
class Identifiable {
    int uid_;

   public:
    Identifiable(int uid = -1) : uid_(uid) {}

    int getUid() const { return uid_; }

    virtual int setUid(int id) {
        int oldId = uid_;
        uid_ = id;
        return oldId;
    }
};

/**
 * @brief Hands out (presumably) unique int ids with a simply incrementing
 * counter. Tickets start with 1.
 *
 */
class TicketMachine {
   private:
    int id_ = 0;
    static int sid__;

   public:
    TicketMachine(int startVal = 0) { id_ = startVal; }

    int getNext() { return ++id_; }
    static int getNextSid() { return ++sid__; }
    std::string getNextStrId() { return std::to_string(getNext()); }
    static std::string getNextStrSid() { return std::to_string(getNextSid()); }
};

int TicketMachine::sid__ = 0;

/**
 * @brief An old fashioned C-style function with a static int that stores state. Produces the same
 * kind of incrementing count the `TicketMachine` above does. This should actually be all you need
 * unless the code is parallel.
 *
 * @param initVal
 * @param reset Set to true this causes the `initVal` to be substituted for whatever is in `id_`.
 * @return int
 */
int getId(int initVal = 0, bool reset = false) {
    static int id_ = initVal;
    if (reset) id_ = initVal;
    // else
    return ++id_;
}

/**
 * @brief Uses the `getId()` function above but returns the string value of the number.
 *
 * @param initVal Passed through to `getId()`
 * @param reset Passed through to `getId()`
 * @return std::string
 */
std::string getStrId(int initVal = 0, bool reset = false) {
    return std::to_string(getId(initVal, reset));
}
}  // namespace cl3
#endif