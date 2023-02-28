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

#include "Signal.hpp"
#include "MonolithicSignals.hpp"

using std::cout;
using std::endl;
using std::make_shared;
using std::shared_ptr;

namespace cl2 {


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
    bool emitInitialValue_;

   public:
    StoredSignal(bool emitInitialValue) : emitInitialValue_(emitInitialValue) {}

    StoredSignal(S currentVal, bool emitInitialValue)
        : currentVal_(currentVal), emitInitialValue_(emitInitialValue) {}

    bool emitInitialValue() const { return emitInitialValue_; }

    void setEmitInitialValue(bool emitInitialValue) { emitInitialValue_ = emitInitialValue; }

    void setCurrentVal(S newVal) { currentVal_ = newVal; }

    S getCurrentVal() const { return currentVal_; }

    /**
     * @brief Ignores the signal if the value hasn't changed. Stores the signal value in currentVal_
     * otherwise.
     *
     * @param s
     * @return true
     * @return false
     */
    virtual bool accept(const S& s) {
        if (s == currentVal_) return false;
        currentVal_ = s;
        if (this->getParent() != nullptr) {
            this->getParent()->childEvent(reinterpret_cast<int>(this));
        }
        return true;
    }

    // virtual void update() {}

    virtual void update() {
        if (this->getOutput() == nullptr) return;
        if (!StoredSignal<S>::emitInitialValue()) return;
        // emit(getSignal());
        // StoredSignal<S>::emit(StoredSignal<S>::getSignal());
        this->emit(this->getSignal());
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
    shared_ptr<SignalObject<S>> secondOutput_ = nullptr;

   public:
    Tee() {}

    virtual void emit(const S& s) {}

    virtual bool accept(const S& s) {
        SignalObject<S>::emit(s);
        return secondOutput_->accept(s);
    }

    void setSecondOutput(shared_ptr<SignalObject<S>> sobj) { secondOutput_ = sobj; }

    virtual void update() {}

    virtual ~Tee() {
        // cout << "Destroying Tee\n";
    }
};

/**
 * @brief Uses the new split-style signals
 *
 * @tparam S
 */
template <typename S>
class MultiFork : public SignalAcceptor<S> {
    std::vector<shared_ptr<SignalAcceptor<S>>> outputs_;

   public:
    MultiFork() {}

    void addOutput(shared_ptr<SignalAcceptor<S>> output) {
        outputs_.push_back(output);
        this->update();
    }

    virtual bool accept(const S& s) {
        // SignalObject<S>::emit(s);
        bool allAccepted = true;
        for (auto output : outputs_) {
            allAccepted = allAccepted & output->accept(s);
        }
        return allAccepted;
    }

    virtual void update() {}

    virtual ~MultiFork() {
        // cout << "Destroying MultiFork\n";
    }
};

template <typename S>
class MultiTee : public SignalObject<S> {
    std::vector<shared_ptr<SignalObject<S>>> outputs_;

   public:
    MultiTee() {}

    void addOutput(shared_ptr<SignalObject<S>> output) {
        outputs_.push_back(output);
        this->update();
    }

    virtual bool accept(const S& s) {
        SignalObject<S>::emit(s);
        bool allAccepted = true;
        for (auto output : outputs_) {
            allAccepted = allAccepted & output->accept(s);
        }
        return allAccepted;
    }

    virtual void update() {}

    virtual ~MultiTee() {
        // cout << "Destroying MultiTee\n";
    }
};

template <typename S, typename Sout>
class CppLambda : public SignalObject<S> {
    std::function<Sout(S s)> lambda_;
    shared_ptr<SignalObject<Sout>> transformedOutput_;

   public:
    CppLambda() {}

    CppLambda(std::function<Sout(S s)> lambda) : lambda_(lambda) {}

    virtual bool accept(const S& s) {
        Sout sOut = lambda_(s);
        if (transformedOutput_ != nullptr) return transformedOutput_->accept(sOut);
        return false;
    }

    void setTransformedOutput(shared_ptr<SignalObject<Sout>> sobj) { transformedOutput_ = sobj; }

    virtual void update() {}

    virtual ~CppLambda() {
        // cout << "Destroying CppLambda\n";
    }
};

template <typename S, typename Sout>
class CppLambdaSS : public SignalAcceptor<S>, public SignalEmitter<Sout> {
    std::function<Sout(S s)> lambda_;

   public:
    CppLambdaSS() {}

    CppLambdaSS(std::function<Sout(S s)> lambda) : lambda_(lambda) {}

    virtual bool accept(const S& s) {
        Sout sOut = lambda_(s);
        cout << "OUTPUT of CppLambdaSS: " << sOut << endl;
        if (this->output_ != nullptr) return this->output_->accept(sOut);
        return false;
    }

    // void setTransformedOutput(shared_ptr<SignalObject<Sout>> sobj) { transformedOutput_ = sobj; }

    virtual void update() {}

    virtual ~CppLambdaSS() {
        // cout << "Destroying CppLambdaSS\n";
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
class Merge : public StoredSignal<outT>,
              public std::enable_shared_from_this<Merge<inT1, inT2, outT>> {
    shared_ptr<StoredSignal<inT2>> in2_ = nullptr;
    shared_ptr<StoredSignal<inT1>> in1_ = nullptr;

    bool signalPresentOnInput1_ = false;
    bool signalPresentOnInput2_ = false;

    std::function<outT(inT1 in1, inT2 in2)> mergeFn_;

   public:
    Merge(std::function<outT(inT1 in1, inT2 in2)> mergeFn, bool emitInitialValue = false)
        : StoredSignal<outT>(emitInitialValue) {
        mergeFn_ = mergeFn;
    }

    shared_ptr<StoredSignal<inT2>> getInput2() {
        if (in2_ == nullptr) in2_ = make_shared<StoredSignal<inT2>>(false);
        in2_->setParent(this->shared_from_this());
        return in2_;
    }
    shared_ptr<StoredSignal<inT1>> getInput1() {
        if (in1_ == nullptr) in1_ = make_shared<StoredSignal<inT1>>(false);
        in1_->setParent(this->shared_from_this());
        return in1_;
    }

    // virtual void childAccepts(SignalObject* child) {
    //     //if (child == in1_)
    //     cout << "in1_ has accepted a signal." << endl;
    // }

    /**
     * @brief Here we compare the messages from the children to the int value of the raw pointers
     * extracted from the shared_ptr. I suppose using a hash value would be better but this works
     * for now. Once both signals are present we recompute() the value.
     *
     * @param e
     */
    virtual void childEvent(int e) {
        cout << "Merge: Child event " << e << endl;
        // if (reinterpret_cast<int>) {}
        int rawPtrVal = reinterpret_cast<int>(in1_.get());
        if (rawPtrVal == e) {
            signalPresentOnInput1_ = true;
        }
        rawPtrVal = reinterpret_cast<int>(in2_.get());
        if (rawPtrVal == e) {
            signalPresentOnInput2_ = true;
        }
        if (signalPresentOnInput1_ && signalPresentOnInput2_) recompute();
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
            cout << "BOTH INPUTS ARE LIVE" << endl;
            inT2 s2 = in2_->getSignal();
            inT1 s1 = in1_->getSignal();
            this->setCurrentVal(mergeFn_(s1, s2));
            if (this->getOutput()) {
                cout << "MERGE OUTPUT IS LIVE" << endl;
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

    // virtual void update() {
    //     if (this->getOutput() == nullptr) return;
    //     if (!StoredSignal<outT>::emitInitialValue()) return;
    //     StoredSignal<outT>::emit(StoredSignal<outT>::getSignal());
    // }
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

    // virtual void update() {
    //     if (this->getOutput() == nullptr) return;
    //     if (!StoredSignal<S>::emitInitialValue()) return;
    //     emit(getSignal());
    // }

    virtual void emit(const S& s) { SignalObject<S>::emit(s); }

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

template <typename S, typename ObjT>
class ObjectAcceptor : public SignalAcceptor<S> {
    shared_ptr<ObjT> obj_;
   void (ObjT::*signalAcceptorMethod_)(const S& s);

   public:
    ObjectAcceptor(shared_ptr<ObjT> obj) { obj_ = obj; }
    //ObjectAcceptor(ObjT& obj) { obj_(obj); }
    //ObjectAcceptor() {  }

    void setSignalAcceptorMethod(void (ObjT::*signalAcceptorMethod)(const S& s)) {
        signalAcceptorMethod_ = signalAcceptorMethod;
    }

    virtual bool accept(const S& s) {
        SignalAcceptor<S>::accept(s);
       (*obj_.*signalAcceptorMethod_)(s);
       return true;
    }

    virtual void update() {}
};

}  // namespace cl2

#endif