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