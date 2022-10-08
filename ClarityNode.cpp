#include "ClarityNode.hpp"

template <>
string HybridNode<string>::cppValToString() const {
    //cout << "HybridNode<string>::cppValToString()\n";
    if (cppVal_ == nullptr) return "HybridNode<string> NULLPTR";
    return *cppVal_;// static_cast<string>(cppVal_);
}

template <>
val HybridNode<double>::getVal() const {
    string valueText = getDOMText();
    return val(stod(valueText));
}

template <>
val HybridNode<float>::getVal() const {
    string valueText = getDOMText();
    return val(stof(valueText));
}

template <>
val HybridNode<int>::getVal() const {
    string valueText = getDOMText();
    return val(stoi(valueText));
}

template <>
val HybridNode<string>::getVal() const {
    string valueText = getDOMText();
    return val(valueText);
}