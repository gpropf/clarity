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

template <typename S, typename ObjT>
class ObjectAcceptor : public SignalAcceptor<S> {
    shared_ptr<ObjT> obj_;
    void (ObjT::*signalAcceptorMethod_)(const S& s);

   public:
    ObjectAcceptor(shared_ptr<ObjT> obj) { obj_ = obj; }
    // ObjectAcceptor(ObjT& obj) { obj_(obj); }
    // ObjectAcceptor() {  }

    void setSignalAcceptorMethod(void (ObjT::*signalAcceptorMethod)(const S& s)) {
        signalAcceptorMethod_ = signalAcceptorMethod;
    }

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

template <typename S, typename ObjT>
class ObjectEmitter : public SignalEmitter<S> {
    shared_ptr<ObjT> obj_;
    const S (ObjT::*signalEmitterMethod_)();

   public:
    ObjectEmitter(shared_ptr<ObjT> obj, bool emitInitialValue = true) :obj_(obj) {
        this->emitInitialValue_ = emitInitialValue;
    }
    // ObjectAcceptor(ObjT& obj) { obj_(obj); }
    // ObjectAcceptor() {  }

    void setSignalEmitterMethod(const S (ObjT::*signalEmitterMethod)()) {
        signalEmitterMethod_ = signalEmitterMethod;
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








}  // namespace cl2

#endif