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
};




























}






















#endif





