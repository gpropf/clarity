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

// template <typename T>
// struct Signal {};

/**
 * @brief Base class representing something that can send or receive signals.
 *
 * @tparam S data type of signal, e.g. string, int, double, etc...
 */
template <typename S>
struct SignalObject {
    shared_ptr<void> obj_;  // = nullptr;
    std::vector<SignalObject*> inputs_;
    std::vector<SignalObject*> outputs_;

    void addOutput(SignalObject* sobj) { outputs_.push_back(sobj); }

    virtual void emit(const S& s) {
        for (auto output : outputs_) {
            output->accept(s);
            // emitOne(output, s);
        }
    }

    virtual void emitOne(SignalObject* sobj, const S& s){};

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
        // cout << "Destructing SignalObject\n";
    }
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