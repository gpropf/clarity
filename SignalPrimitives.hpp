/**
 * @file SignalPrimitives.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief Somewhat more complex but still basic signal machinery based on the new "split-style"
 * signals.
 * @version 0.1
 * @date 2023-03-01
 *
 * @copyright Copyright (c) 2023
 *
 */

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

// #include "MonolithicSignals.hpp"
#include "Signal.hpp"

using std::cout;
using std::endl;
using std::make_shared;
using std::shared_ptr;

namespace cl2 {

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

template <typename S, typename Sout>
class CppLambda : public SignalAcceptor<S>, public SignalEmitter<Sout> {
    std::function<Sout(S s)> lambda_;

   public:
    CppLambda() {}

    CppLambda(std::function<Sout(S s)> lambda) : lambda_(lambda) {}

    // void printState() {
    //     cout << "SignalAcceptor::currentValue_: " << SignalAcceptor<S>::getCurrentValue()
    //          << ", SignalEmitter::currentValue_: " << SignalEmitter<Sout>::getCurrentValue()
    //          << endl;
    // }

    virtual bool accept(const S& s) {
        SignalAcceptor<S>::accept(s);
        //printState();
        Sout sOut = lambda_(s);
        cout << "OUTPUT of CppLambda: " << sOut << endl;

        this->emit(sOut);
        //printState();
        // if (this->output_ != nullptr) return this->output_->accept(sOut);
        return false;
    }

    // void setTransformedOutput(shared_ptr<SignalObject<Sout>> sobj) { transformedOutput_ = sobj; }

    virtual void update() {}

    virtual ~CppLambda() {
        // cout << "Destroying CppLambda\n";
    }
};

template <typename S, typename ObjT>
class ObjectAcceptor : public SignalAcceptor<S> {
    shared_ptr<ObjT> obj_;
    void (ObjT::*signalAcceptorMethod_)(const S& s);

   public:
    ObjectAcceptor(shared_ptr<ObjT> obj) { obj_ = obj; }
    ObjectAcceptor() {}
    // ObjectAcceptor(ObjT& obj) { obj_(obj); }
    // ObjectAcceptor() {  }

    void setSignalAcceptorMethod(void (ObjT::*signalAcceptorMethod)(const S& s)) {
        signalAcceptorMethod_ = signalAcceptorMethod;
    }

    void setObjectPointer(shared_ptr<ObjT> obj) { obj_ = obj; }

    virtual bool accept(const S& s) {
        SignalAcceptor<S>::accept(s);
        (*obj_.*signalAcceptorMethod_)(s);
        return true;
    }

    virtual void update() {}

    virtual ~ObjectAcceptor() {
        // cout << "Destroying ObjectAcceptor\n";
    }
};

// template <typename S, typename ObjT>
// class RawPointerObjectAcceptor : public SignalAcceptor<S> {
//     ObjT * obj_;
//     void (ObjT::*signalAcceptorMethod_)(const S& s);

//    public:
//     RawPointerObjectAcceptor(ObjT &obj) { obj_ = &obj; }

//     void setSignalAcceptorMethod(void (ObjT::*signalAcceptorMethod)(const S& s)) {
//         signalAcceptorMethod_ = signalAcceptorMethod;
//     }

//     virtual bool accept(const S& s) {
//         SignalAcceptor<S>::accept(s);
//         (*obj_.*signalAcceptorMethod_)(s);
//         return true;
//     }

//     virtual void update() {}

//     virtual ~RawPointerObjectAcceptor() {
//         // cout << "Destroying ObjectAcceptor\n";
//     }
// };

template <typename S, typename ObjT>
class ObjectEmitter : public SignalEmitter<S> {
    shared_ptr<ObjT> obj_;
    S (ObjT::*signalEmitterMethod_)();

   public:
    ObjectEmitter(shared_ptr<ObjT> obj, bool emitInitialValue = true) : obj_(obj) {
        this->emitInitialValue_ = emitInitialValue;
    }
    // ObjectAcceptor(ObjT& obj) { obj_(obj); }
    ObjectEmitter() {}

    void setSignalEmitterMethod(S (ObjT::*signalEmitterMethod)()) {
        signalEmitterMethod_ = signalEmitterMethod;
    }

    void setObjectPointer(shared_ptr<ObjT> obj, bool emitInitialValue = true) {
        obj_ = obj;
        this->emitInitialValue_ = emitInitialValue;
    }

    virtual void emit() {
        const S newSignal = (*obj_.*signalEmitterMethod_)();
        SignalEmitter<S>::emit(newSignal);
    }

    virtual void update() {
        if (this->emitInitialValue_) {
            cout << "emitInitialValue_ set, emitting initial value!" << endl;
            this->emit();
        }
    }

    virtual ~ObjectEmitter() {
        // cout << "Destroying ObjectAcceptor\n";
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
class Merge : public SignalEmitter<outT>,
              public std::enable_shared_from_this<Merge<inT1, inT2, outT>> {
    shared_ptr<SignalAcceptor<inT2>> in2_ = nullptr;
    shared_ptr<SignalAcceptor<inT1>> in1_ = nullptr;

    bool signalPresentOnInput1_ = false;
    bool signalPresentOnInput2_ = false;

    std::function<outT(inT1 in1, inT2 in2)> mergeFn_;

   public:
    Merge(std::function<outT(inT1 in1, inT2 in2)> mergeFn, bool emitInitialValue = false)
        : SignalEmitter<outT>(emitInitialValue) {
        mergeFn_ = mergeFn;
    }

    shared_ptr<SignalAcceptor<inT2>> getInput2() {
        if (in2_ == nullptr) in2_ = make_shared<SignalAcceptor<inT2>>();
        in2_->setParent(this->shared_from_this());
        return in2_;
    }
    shared_ptr<SignalAcceptor<inT1>> getInput1() {
        if (in1_ == nullptr) in1_ = make_shared<SignalAcceptor<inT1>>();
        in1_->setParent(this->shared_from_this());
        return in1_;
    }

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
            inT2 s2 = in2_->getCurrentValue();
            inT1 s1 = in1_->getCurrentValue();
            this->currentValue_ = mergeFn_(s1, s2);
            if (this->getOutput()) {
                cout << "MERGE OUTPUT IS LIVE" << endl;
                this->emit(this->getCurrentValue());
            }
            return true;
        }
        return false;
    }

    virtual void update() {
        if (this->getOutput() == nullptr) return;
        // if (!SignalEmitter<outT>::emitInitialValue()) return;
        // SignalEmitter<outT>::emit(SignalEmitter<outT>::getCurrentValue());
    }
};

}  // namespace cl2

#endif