/**
 * @file ObjectChannel.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief This class allows you "embed" an arbitrary object along with setter and getter methods in
 * a channel. Essentially, it connects objects to the signal pipeline.
 * @version 0.3
 * @date 2023-10-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef ObjectChannel3_hpp
#define ObjectChannel3_hpp

#include <memory>

#include "Channel.hpp"
#include "Util.hpp"
#include "WebElements.hpp"
// #include "WebElementChannel.hpp"

using std::cout;
using std::map;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

namespace cl3 {

template <typename S>
S valToCPP(val v) {
    return std::stoi(v.as<string>());
}

template <>
string valToCPP(val v) {
    return v.as<string>();
}

template <>
int valToCPP(val v) {
    return std::stoi(v.as<string>());
}

template <>
double valToCPP(val v) {
    return std::stod(v.as<string>());
}

template <typename S>
val cppToVal(S s) {
    return val(to_string(s));
}

template <>
val cppToVal(string s) {
    return val(s);
}

// Metaval stuff

template <typename S>
S metavalToCPP(shared_ptr<Metaval> v) {
    return v->toString();
}

template <>
string metavalToCPP(shared_ptr<Metaval> v) {
    return v->toString();
}

template <>
int metavalToCPP(shared_ptr<Metaval> v) {
    return std::stoi(v->toString());
}

template <>
double metavalToCPP(shared_ptr<Metaval> v) {
    return std::stod(v->toString());
}

template <class ObjClass, typename S>
class ObjectChannel : public Channel {
    S signalValue_;
    shared_ptr<ObjClass> objPtr_;
    void (ObjClass::*setter_)(const S& s);
    S (ObjClass::*getter_)() const;

    void setGetterMethod(S (ObjClass::*getter)()) { getter_ = getter; }
    void setSetterMethod(void (ObjClass::*setter)(const S& s)) { setter_ = setter; }

   public:
    ObjectChannel(shared_ptr<ObjClass> objPtr, string name = "",
                  void (ObjClass::*setter)(const S& s) = nullptr,
                  S (ObjClass::*getter)() const = nullptr)
        : objPtr_(objPtr), setter_(setter), getter_(getter) {
        name_ = name;
    }

    /**
     * @brief This injected value will be used to call the designated setter method and update the
     * values on the encapsulated object.
     *
     * @param v
     * @param signalGeneration
     */
    virtual void inject(val v, int signalGeneration = 0) {
        Channel::inject(v, signalGeneration);
        S s = valToCPP<S>(v);
        cout << "Injected value treated as '" << typeid(s).name() << "'" << endl;
        (*objPtr_.*setter_)(s);
    }

    virtual void injectMetaval(shared_ptr<Metaval> mv, int signalGeneration = 0) {
        val printVal = val::global("printVal");
        Channel::injectMetaval(mv, signalGeneration);

        S s = metavalToCPP<S>(mv);

        cout << "ObjectChannel::injectMetaval - Channel name: " << name_
             << ", signal generation = " << signalGeneration << endl;

        cout << "ObjectChannel::MV->toString: " << mv->toString() << endl;
        (*objPtr_.*setter_)(s);

        // for (auto c : this->channels_) {
        //     if (signalGeneration == 0) c->injectMetaval(mv, signalGeneration + 1);
        // }
    }

    /**
     * @brief This method pulls the value from the encapsulated object and injects it into the
     * channel.
     *
     * @param tabs
     */
    virtual void syncFrom(string tabs = "") {
        cout << tabs << "ObjectChannel::syncFrom() for '" << name_ << "', uid: " << getUid()
             << endl;
        val v = cppToVal((*objPtr_.*getter_)());
        inject(v);
    };

    virtual void syncTo(){};
};
};  // namespace cl3

#endif