#ifndef Signal_hpp
#define Signal_hpp

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using std::shared_ptr;
using std::make_shared;

namespace cl2 {

template <typename T>
struct Signal {};

template <typename T, typename S>
struct SignalObject {
    shared_ptr<T> obj_ = nullptr;
    std::vector<shared_ptr<void>> inputs_;
    std::vector<shared_ptr<void>> outputs_;

    void addOutput(shared_ptr<void> sobj) {
        outputs_.push_back(sobj);
    }

    virtual void emit(S & s) {
        for (auto output: outputs_) {
            //output->accept(s);
        }
    }

    virtual void accept(S & s) {
        
    }
    // T const * obj;
    SignalObject(T& obj) { obj_ = make_shared<T>(obj); }
};

template <typename T, typename S>
struct SignalSource : public SignalObject<T, S> {
    SignalSource(T& obj) : SignalObject<T, S>(obj) {}
    
};

template <typename T, typename S>
struct SignalSink : public SignalObject<T,S> {
    SignalSink(T& obj) : SignalObject<T, S>(obj) {}
};

}  // namespace cl2

#endif