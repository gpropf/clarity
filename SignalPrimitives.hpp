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

    void setOutput(SignalObject* sobj) {
        output_ = sobj;
        update();
    }

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

/**
 * @brief This class was motivated by the need to produce a signal on demand instead of strictly in
 * response to some event as in the case of the base class. This need can be seen in the case of the
 * `Merge` class in particular where inputs may or may not be present at the same moment. This class
 * creates, in effect, a buffer for its signal value.
 *
 * @tparam S
 */
template <typename S>
class StoredSignal : public SignalObject<S> {
    S currentVal_;
    const bool emitInitialValue_;

   public:
    StoredSignal(bool emitInitialValue) : emitInitialValue_(emitInitialValue) {}
    StoredSignal(S currentVal, bool emitInitialValue)
        : currentVal_(currentVal), emitInitialValue_(emitInitialValue) {}
    bool emitInitialValue() const { return emitInitialValue_; }
    void setCurrentVal(S newVal) { currentVal_ = newVal; }
    S getCurrentVal() const { return currentVal_; }
    virtual bool accept(const S& s) {
        if (s == currentVal_) return false;
        currentVal_ = s;
        return true;
    }

    virtual void update() {}

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

/**
 * @brief Similar to the `CppLambda` class except has 2 inputs. In practice this is considerably
 * more complicated than the single input case because it is possible that one of the inputs may not
 * exist when the other comes in.
 *
 * @tparam inT1
 * @tparam inT2
 * @tparam outT
 */
template <typename inT1, typename inT2, typename outT>
class Merge : public StoredSignal<outT> {
    StoredSignal<inT2>* in2_ = nullptr;
    StoredSignal<inT1>* in1_ = nullptr;

    std::function<outT(inT1 in1, inT2 in2)> mergeFn_;

   public:
    Merge(std::function<outT(inT1 in1, inT2 in2)> mergeFn) : StoredSignal<inT1>(false) {
        mergeFn_ = mergeFn;
    }

    StoredSignal<inT2>* getInput2() {
        if (in2_ == nullptr) in2_ = new StoredSignal<inT2>(false);
        return in2_;
    }
    StoredSignal<inT1>* getInput1() {
        if (in1_ == nullptr) in1_ = new StoredSignal<inT1>(false);
        return in1_;
    }

    /**
     * @brief Check to see if both inputs are initialized and then run the stored lambda, emitting
     * the result.
     *
     * @return true If both inputs exist.
     * @return false If one of the inputs does not exist.
     */
    bool recompute() {
        if (in2_ && in1_) {
            inT2 s2 = in2_->getSignal();
            inT1 s1 = in1_->getSignal();
            this->setCurrentVal(mergeFn_(s1, s2));
            if (this->getOutput()) {
                this->emit(this->getCurrentVal());
            }
            return true;
        }
        return false;
    }

    /**
     * @brief This is a kludge. We're using the output_ to trigger a recompute operation. Probably
     * shouldn't really do anything here at all.
     *
     * @param sOut
     * @return true
     * @return false
     */
    virtual bool accept(const outT& sOut) {
        bool ssAccepted = StoredSignal<outT>::accept(sOut);
        return recompute();
    }

    virtual void update() {}
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