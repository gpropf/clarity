#ifndef Signal_hpp
#define Signal_hpp

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::make_shared;
using std::shared_ptr;

namespace cl2 {

template <typename T>
struct Signal {};

template <typename S>
struct SignalObject {
    shared_ptr<void> obj_;  // = nullptr;
    std::vector<SignalObject*> inputs_;
    std::vector<SignalObject*> outputs_;

    void addOutput(SignalObject* sobj) { outputs_.push_back(sobj); }

    virtual void emit(const S& s) {
        for (auto output : outputs_) {
            output->accept(s);
            //emitOne(output, s);
        }
    }

    virtual void emitOne(SignalObject* sobj, const S& s){};

    virtual void accept(const S& s) =0 ;
    // {
    //     cout << "SignalObject::accept() This is wrong!" << endl;
    // };

    virtual void finalize() {}


    virtual ~SignalObject() {
         cout << "Destructing SignalObject\n"; 
    }
    // T const * obj;
    // SignalObject(T& obj) { obj_ = make_shared<T>(obj); }
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