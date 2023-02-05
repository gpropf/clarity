#ifndef Util_hpp
#define Util_hpp

#include <string>

/**
 * @brief Hands out (presumably) unique int ids with a simply incrementing
 * counter. Tickets start with 1.
 *
 */
class TicketMachine {
   private:
    int id_ = 0;

   public:
    TicketMachine(int startVal = 0) { id_ = startVal; }

    int getNext() { return ++id_; }
    std::string getNextStrId() { return std::to_string(getNext()); }
};

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

#endif