#ifndef SignalPrimitives_hpp
#define SignalPrimitives_hpp

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

// template <typename T>
// struct Signal {};

/**
 * @brief Base class representing something that can send or receive signals.
 *
 * @tparam S data type of signal, e.g. string, int, double, etc...
 */
template <typename S>
class SignalObject {
    // It does seem necessary to set these to nullptr if we want to test for lack of initialization
    // later.

    SignalObject* output_ = nullptr;

   public:
    SignalObject* getOutput() const { return output_; }

    void setOutput(SignalObject* sobj) { output_ = sobj; }

    /**
     * @brief Send the signal to the output.
     *
     * @param s
     */
    virtual void emit(const S& s) const {
        if (output_ != nullptr) output_->accept(s);
    }

    virtual bool accept(const S& s) = 0;

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

    virtual ~SignalObject() {
        // cout << "Destroying SignalObject\n";
    }
};

template <typename S>
class StoredSignal : public SignalObject<S> {
    S currentVal_;
    const bool emitInitialValue_;  // = false;

   public:
    StoredSignal(bool emitInitialValue = false) : emitInitialValue_(emitInitialValue) {}
    StoredSignal(S currentVal, bool emitInitialValue = false)
        : currentVal_(currentVal), emitInitialValue_(emitInitialValue) {}
    bool emitInitialValue() const { return emitInitialValue_; }
    virtual bool accept(const S& s) {
        if (s == currentVal_) return false;
        currentVal_ = s;
        return true;
    }
    virtual S getSignal() const { return currentVal_; };
};

/**
 * @brief Tee simply does roughly what the Linux command of that name does. It duplicates an output.
 * It acts as a passive conduit so this is why the emit() method does nothing. Essentially until it
 * performs an accept() operation, it does nothing.
 *
 * @tparam S
 */
template <typename S>
class Tee : public SignalObject<S> {
    SignalObject<S>* secondOutput_;

   public:
    Tee() {}

    virtual void emit(const S& s) const {}

    virtual bool accept(const S& s) {
        SignalObject<S>::emit(s);
        return secondOutput_->accept(s);
    }

    void setSecondOutput(SignalObject<S>* sobj) { secondOutput_ = sobj; }

    virtual void update() {}

    virtual ~Tee() {
        // cout << "Destroying Tee\n";
    }
};

template <typename S, typename Sout>
class CppLambda : public SignalObject<S> {
    std::function<Sout(S s)> lambda_;
    SignalObject<Sout>* transformedOutput_;

   public:
    CppLambda() {}

    CppLambda(std::function<Sout(S s)> lambda) : lambda_(lambda) {}

    virtual bool accept(const S& s) {
        Sout sOut = lambda_(s);
        if (transformedOutput_ != nullptr) return transformedOutput_->accept(sOut);
        return false;
    }

    void setTransformedOutput(SignalObject<Sout>* sobj) { transformedOutput_ = sobj; }

    virtual void update() {}

    virtual ~CppLambda() {
        // cout << "Destroying CppLambda\n";
    }
};

template <typename inT1, typename inT2, typename outT>
class Merge : public SignalObject<inT1> {
    SignalObject<outT>* out_;
    SignalObject<inT2>* in2_;

    std::function<outT(inT1 in1, inT2 in2)> mergeFn_;

   public:
    void setInput2(SignalObject<inT2>* in2) { in2_ = in2; }

    virtual bool accept(const inT1& s1) {
        return true;  // FIXME
    }
    // virtual bool accept(const inT2& s2) {}
};

/**
 * @brief A signal wrapper for any type of C++ class that has getter and setter methods.
 *
 * @tparam S
 * @tparam ObjT
 */
template <typename S, typename ObjT>
class CppObjectSignalObject : public StoredSignal<S> {
    shared_ptr<ObjT> obj_;

   public:
    void (ObjT::*setter)(const S& s);
    S (ObjT::*getter)();

    CppObjectSignalObject(ObjT& obj) : StoredSignal<S>() { obj_ = make_shared<ObjT>(obj); }
    CppObjectSignalObject(ObjT& obj, bool emitInitialValue) : StoredSignal<S>(emitInitialValue) {
        obj_ = make_shared<ObjT>(obj);
    }

    virtual void update() {
        if (this->getOutput() == nullptr) return;
        if (!StoredSignal<S>::emitInitialValue()) return;
        emit(getSignal());
    }

    virtual void emit(const S& s) const { SignalObject<S>::emit(s); }

    virtual bool accept(const S& s) {
        bool storedSignalAccepts = StoredSignal<S>::accept(s);
        // StoredSignal<S>::accept(s);
        if (!storedSignalAccepts) return false;
        (*obj_.*setter)(s);
        return true;
    }

    /**
     * @brief Pretty much a convenience method to extract a signal from the object using the getter.
     * Makes it easy to use a timer function to get the value using setInterval() from JS.
     *
     * @return S
     */
    virtual S getSignal() const { return (*obj_.*getter)(); }
};

}  // namespace cl2

#endif