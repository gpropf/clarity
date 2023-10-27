#ifndef Util_hpp
#define Util_hpp

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <iostream>
#include <sstream>
#include <string>

using emscripten::val;
using std::cout;
using std::endl;
using std::map;
using std::ostringstream;
using std::string;

namespace cl2 {

union MapValUnion {
    int i_;
    double d_;
    string* s_;
    MapValUnion() : s_{} {};
    ~MapValUnion(){};
};

// MapValUnion::MapValUnion(shared_ptr<string> s) :s_(s) {}

struct MapVal {
    enum selector { INT = 1, DBL = 2, STR = 3 };
    selector type_ = STR;
    MapValUnion mvu_;
    MapVal(string* s) {
        mvu_.s_ = s;
        type_ = STR;
    }

    /**
     * @brief Construct a new Map Val object from a char ptr. If you use this to create values in a
     * map though, the temporary MapVal string will be disposed of before you can use it, for
     * instance in the mapToJson methods here.
     *
     * @param c
     */
    // MapVal(const char* c) {
    //     mvu_.s_ = new string(c);
    //     type_ = STR;
    // }
    MapVal(int i) {
        mvu_.i_ = i;
        type_ = INT;
    }
    MapVal(double d) {
        mvu_.d_ = d;
        type_ = DBL;
    }

    ~MapVal() {
        if (type_ == STR) {
            delete mvu_.s_;
        }
    }

    bool operator<(MapVal const& rhs) const {
        switch (type_) {
            case INT:
                return mvu_.i_ < rhs.mvu_.i_;
            case DBL:
                return mvu_.d_ < rhs.mvu_.d_;
            case STR:
                return *(mvu_.s_) < *(rhs.mvu_.s_);
        }
        return false;
    }

    string toJson() const {
        ostringstream os;
        // string stringVal;
        os << "\"";
        switch (type_) {
            case INT:
                os << mvu_.i_;
                // stringVal = os.str();
                break;
            case DBL:
                os << mvu_.d_;
                break;

            case STR:
                os << *(mvu_.s_);
                break;
        }
        os << "\"";
        return os.str();
    }

    static string mapToJson(map<string, MapVal>& m) {
        ostringstream os;
        for (const auto& [key, value] : m) {
            os << "\t\"" << key << "\" : " << value.toJson() << "," << endl;
        }
        return os.str();
    }

    static string mapToJson(map<MapVal, MapVal>& m) {
        ostringstream os;
        for (const auto& [key, value] : m) {
            os << "\t" << key.toJson() << " : " << value.toJson() << "," << endl;
        }
        return os.str();
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