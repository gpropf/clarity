#ifndef simpleTest_hpp
#define simpleTest_hpp

#include "clarity.hpp"
#include "ControlNetworkNode.hpp"
#include "ModelNode.hpp"
#include "WebNode.hpp"
#include "WebElemNode.hpp"
#include "WebAttrNode.hpp"
#include "ButtonElement.hpp"
//#include "CompoundElement.hpp"


using namespace clarity;
//using CppType = clarity::ControlNetworkNode::CppType;

struct TBase {

    void * foo_;
};

template <typename T>
struct Ttest : public TBase {
Ttest(T * t) {
    foo_ = t; 
}

};




#endif
