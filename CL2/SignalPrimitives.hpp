#ifndef SignalPrimitives_hpp
#define SignalPrimitives_hpp

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
    SignalObject* input_ = nullptr;
    SignalObject* output_ = nullptr;

   public:
    SignalObject* getOutput() const { return output_; }
    SignalObject* getInput() const { return input_; }

    void setOutput(SignalObject* sobj) {        
        output_ = sobj;
        sobj->setInput(this);
    }

    void setInput(SignalObject* sobj) {       
        input_ = sobj;
    }

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
class Tee : public SignalObject<S> {
    SignalObject<S>* secondOutput_;

   public:
    Tee() {}

    virtual void emit(const S& s) const {
        // SignalObject<S>::emit(s);
        // secondOutput_->accept(s);
    }

    virtual void accept(const S& s) {
        SignalObject<S>::emit(s);
        secondOutput_->accept(s);
    }

    void setSecondOutput(SignalObject<S>* sobj) {
        // outputs_.push_back(sobj);
        secondOutput_ = sobj;
        sobj->setInput(this);
    }

    virtual void finalize() {}

    virtual ~Tee() {
        // cout << "Destroying Tee\n";
    }
};

template <typename S, typename Sout>
class Transformer : public SignalObject<S> {
    std::function<Sout(S s)> transformFn_;
    SignalObject<Sout>* transformedOutput_;

   public:
    Transformer() {}

    Transformer(std::function<Sout(S s)> transformFn) : transformFn_(transformFn) {}

    virtual void accept(const S& s) {
        Sout sOut = transformFn_(s);
        if (transformedOutput_ != nullptr) transformedOutput_->accept(sOut);
    }

    void setTransformedOutput(SignalObject<Sout>* sobj) {
        // outputs_.push_back(sobj);
        transformedOutput_ = sobj;
        sobj->setInput(this->transformedOutput_);
    }

    virtual void finalize() {}

    virtual ~Transformer() {
         // cout << "Destroying Transformer\n";
    }
};

}  // namespace cl2

#endif