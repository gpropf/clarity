#ifndef Signal_hpp
#define Signal_hpp

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



template <typename S>
class Signal {
    shared_ptr<Signal> parent_ = nullptr;

   protected:
    S currentValue_;

   public:
    /**
     * @brief This is a fairly crude message passing system that exists to alert the parent of a
     * signal that one of its children has done something. Right now we just convert the `this`
     * pointer of the child to an int so the parent knows which one it was. Eventually, I'll have a
     * proper message struct with an argument and a little tag to let the parent know what's going
     * on in detail.
     *
     * @param e
     */
    virtual void childEvent(int e) { cout << "Child event " << e << endl; }

    shared_ptr<Signal> getParent() const { return parent_; }

    void setParent(shared_ptr<Signal> parent) { parent_ = parent; }

    /**
     * @brief Sometimes it is necessary to hold off certain tasks until the object is fully set up.
     * JS event handlers in particular need to see the completed object in order to work properly
     * when they fire. This method must be called once a `SignalObject` has all of its inputs and
     * outputs set up. If the object is modified this should be called again. It is meant to be
     * idempotent so it's important to clean up any event listeners or other things that are set
     * here on second and subsequent calls.
     *
     */
    virtual void update() = 0;

    virtual ~Signal() {
        // cout << "Destroying Signal\n";
    }
};



template <typename S>
class SignalAcceptor;  // Forward declaration so we can refer to this class in the SignalEmitter
                       // class.

/**
 * @brief Base class representing something that can emit signals.
 *
 * @tparam S data type of signal, e.g. string, int, double, etc...
 */
template <typename S>
class SignalEmitter : public Signal<S> {
   protected:
    shared_ptr<SignalAcceptor<S>> output_ = nullptr;
    bool emitInitialValue_ = false;

   public:
    shared_ptr<SignalAcceptor<S>> getOutput() const { return output_; }

    void setOutput(shared_ptr<SignalAcceptor<S>> sobj) {
        output_ = sobj;
        this->update();
    }

    /**
     * @brief Send the signal to the output.
     *
     * @param s
     */
    virtual void emit(const S& s) {
        this->currentValue_ = s;
        if (output_ != nullptr) {
            cout << "SignalEmitter::emit called" << endl;
            output_->accept(s);

        } else {
            cout << "SignalEmitter::emit called but OUTPUT NOT SET!" << endl;
        }
    }

    virtual ~SignalEmitter() {
        // cout << "Destroying SignalObject\n";
    }
};

/**
 * @brief Base class representing something that can emit signals.
 *
 * @tparam S data type of signal, e.g. string, int, double, etc...
 */
template <typename S>
class SignalAcceptor : public Signal<S> {
    // It does seem necessary to set these to nullptr if we want to test for lack of initialization
    // later.

   protected:
    shared_ptr<SignalEmitter<S>> input_ = nullptr;

   public:
    shared_ptr<Signal<S>> getInput() const { return input_; }

    void setInput(shared_ptr<Signal<S>> sobj) {
        input_ = sobj;
        this->update();
    }

    virtual bool accept(const S& s) {
        this->currentValue_ = s;
        return true;
    }

    virtual ~SignalAcceptor() {
        // cout << "Destroying SignalObject\n";
    }
};



























}






















#endif





