#ifndef Signal_hpp
#define Signal_hpp

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using std::make_shared;
using std::shared_ptr;

namespace cl2 {

template <typename T>
struct Signal {};

struct SignalObject {
    shared_ptr<void> obj_;  // = nullptr;
    std::vector<shared_ptr<SignalObject>> inputs_;
    std::vector<shared_ptr<SignalObject>> outputs_;

    void addOutput(SignalObject& sobj) { outputs_.push_back(make_shared<SignalObject>(sobj)); }

    virtual void emit(shared_ptr<void> s) {
        for (auto output : outputs_) {
            // output->accept(s);
        }
    }

    virtual void accept(shared_ptr<void> s) {};
    // T const * obj;
    //SignalObject(T& obj) { obj_ = make_shared<T>(obj); }
};

// template <typename T, typename S>
// struct SignalSource : public SignalObject<T, S> {
//     SignalSource(T& obj) : SignalObject<T, S>(obj) {}
// };

// template <typename T, typename S>
// struct SignalSink : public SignalObject<T, S> {
//     SignalSink(T& obj) : SignalObject<T, S>(obj) {}
// };

}  // namespace cl2

#endif