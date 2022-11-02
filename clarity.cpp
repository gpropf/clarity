#include "clarity.hpp"

/**
 * @brief Very simple logging function.
 * 
 * @param msg 
 * @param clt 
 */
void clarity::clog(const string &msg, ClogType clt) {
    switch (clt) {
        case ClogType::WARNING:
            cout << "WARNING: " << msg << endl;
            break;
        case ClogType::ERROR:
            cout << "ERROR! " << msg << endl;
            break;
        //default:
    }
}