#ifndef Util_hpp
#define Util_hpp

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <sstream>
#include <iostream>

using emscripten::val;
using std::cout;
using std::endl;
using std::string;
using std::ostringstream;

namespace cl2 {

union MapValUnion {
    int i_;
    double d_;
    shared_ptr<string> s_;
    MapValUnion() : s_{} {};
    ~MapValUnion() {}
};

// MapValUnion::MapValUnion(shared_ptr<string> s) :s_(s) {}

struct MapVal {
    enum selector { INT = 1, DBL = 2, STR = 3 };
    selector type_ = STR;
    MapValUnion mvu_;
    MapVal(shared_ptr<string> s) {
        mvu_.s_ = s;
        type_ = STR;
    }
    MapVal(int i) {
        mvu_.i_ = i;
        type_ = INT;
    }
    MapVal(double d) {
        mvu_.d_ = d;
        type_ = DBL;
    }
    string toJson() {
        ostringstream os;
        switch (type_) {
            case INT:
                os << mvu_.i_;
                return os.str();
            case DBL:
                os << mvu_.d_;
                return os.str();
            case STR:
                os << *(mvu_.s_);
                return os.str();
        }
    }
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
}  // namespace cl2
#endif