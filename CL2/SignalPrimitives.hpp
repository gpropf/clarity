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

    virtual void accept(const S& s) = 0;

    /**
     * @brief Sometimes it is necessary to hold off certain tasks until the object is fully set up.
     * JS event handlers in particular need to see the completed object in order to work properly
     * when they fire. This method must be called once a `SignalObject` has all of its inputs and
     * outputs set up.
     *
     */
    virtual void finalize() = 0;

    virtual ~SignalObject() {
        // cout << "Destroying SignalObject\n";
    }
};

template <typename S>
class StoredSignal : public SignalObject<S> {
    S currentVal_;

   public:
    virtual void accept(const S& s) { currentVal_ = s; }
    virtual S getSignal() const = 0;
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

    virtual void accept(const S& s) {
        SignalObject<S>::emit(s);
        secondOutput_->accept(s);
    }

    void setSecondOutput(SignalObject<S>* sobj) { secondOutput_ = sobj; }

    virtual void finalize() {}

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

    virtual void accept(const S& s) {
        Sout sOut = lambda_(s);
        if (transformedOutput_ != nullptr) transformedOutput_->accept(sOut);
    }

    void setTransformedOutput(SignalObject<Sout>* sobj) { transformedOutput_ = sobj; }

    virtual void finalize() {}

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

    virtual void accept(const inT1& s1) {}
    // virtual void accept(const inT2& s2) {}
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

    CppObjectSignalObject(ObjT& obj) : obj_(make_shared<ObjT>(obj)) {}

    virtual void finalize() {}

    virtual void emit(const S& s) const { SignalObject<S>::emit(s); }

    virtual void accept(const S& s) {
        StoredSignal<S>::accept(s);
        (*obj_.*setter)(s);
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