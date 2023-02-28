#ifndef MonolithicSignals_hpp
#define MonolithicSignals_hpp


#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::make_shared;
using std::shared_ptr;


namespace cl2 {

/**
 * @brief Base class representing something that can send or receive signals.
 *
 * @tparam S data type of signal, e.g. string, int, double, etc...
 */
template <typename S>
class SignalObject : public Signal<S> {
    // It does seem necessary to set these to nullptr if we want to test for lack of initialization
    // later.

    shared_ptr<SignalObject> output_ = nullptr;
    shared_ptr<SignalObject> parent_ = nullptr;

   public:
    shared_ptr<SignalObject> getOutput() const { return output_; }

    void setOutput(shared_ptr<SignalObject> sobj) {
        output_ = sobj;
        this->update();
    }

    /**
     * @brief Send the signal to the output.
     *
     * @param s
     */
    virtual void emit(const S& s) {
        if (output_ != nullptr) output_->accept(s);
    }

    virtual bool accept(const S& s) = 0;

    virtual ~SignalObject() {
        // cout << "Destroying SignalObject\n";
    }
};




















}

#endif
