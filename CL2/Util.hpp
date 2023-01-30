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

    inline const int getNext() { return ++id_; }
};

int getId(int initVal = 0, bool reset = false) {
    static int id_ = initVal;
    if (reset) id_ = initVal;
    // else
    return ++id_;
}

std::string getStrId(int initVal = 0, bool reset = false) {
    return std::to_string (getId(initVal,reset));
}

#endif