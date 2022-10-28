#include "CanvasElement.hpp"
#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"

val CLElement_ = val::global("CLElement");

template <>
string CanvasGrid<unsigned char>::cppValToString() const {
    //cout << "HybridNode<string>::cppValToString()\n";
    if (cppVal_ == nullptr) return "CanvasGrid<unsigned char> NULLPTR";
    cout << "CanvasGrid<unsigned char>::cppValToString()\n";
    return clto_str<unsigned int>(*cppVal_);// static_cast<string>(cppVal_);
    //return string("<unsigned char> Canvas.");
}