#include "CanvasElement.hpp"
#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"

template <>
string CanvasGrid<unsigned char>::cppValToString() const {    
    if (cppVal_ == nullptr) return "CanvasGrid<unsigned char> NULLPTR";    
    return clto_str<unsigned int>(*cppVal_);    
}