/**
 * @file ClarityNode.cpp
 * @author your name (you@domain.com)
 * @brief This file is primarily for template specializations of getVal() for common data types.
 * @version 1.0
 * @date 2022-10-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ClarityNode.hpp"

template <>
string HybridNode<string>::cppValToString() const {
    //cout << "HybridNode<string>::cppValToString()\n";
    if (cppVal_ == nullptr) return "HybridNode<string> NULLPTR";
    return *cppVal_;// static_cast<string>(cppVal_);
}

template <>
string HybridNode<unsigned char>::cppValToString() const {
    //cout << "HybridNode<string>::cppValToString()\n";
    if (cppVal_ == nullptr) return "HybridNode<unsigned char> NULLPTR";
    return clto_str<unsigned int>(*cppVal_);// static_cast<string>(cppVal_);
    //return string("<unsigned char> Canvas.");
}

template <>
string HybridNode<double>::cppValToString() const {
    //cout << "HybridNode<string>::cppValToString()\n";
    if (cppVal_ == nullptr) return "HybridNode<double> NULLPTR";
    return clto_str<double>(*cppVal_);// static_cast<string>(cppVal_);
    //return string("<unsigned char> Canvas.");
}

template <>
string HybridNode<float>::cppValToString() const {
    //cout << "HybridNode<string>::cppValToString()\n";
    if (cppVal_ == nullptr) return "HybridNode<float> NULLPTR";
    return clto_str<float>(*cppVal_);// static_cast<string>(cppVal_);
    //return string("<unsigned char> Canvas.");
}

template <>
string HybridNode<int>::cppValToString() const {    
    if (cppVal_ == nullptr) return "HybridNode<int> NULLPTR";
    return clto_str<double>(*cppVal_);
}

template <>
string HybridNode<bool>::cppValToString() const {
    //cout << "HybridNode<string>::cppValToString()\n";
    if (cppVal_ == nullptr) return "HybridNode<bool> NULLPTR";
    return clto_str<bool>(*cppVal_);// static_cast<string>(cppVal_);
    //return string("<unsigned char> Canvas.");
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
val HybridNode<unsigned char>::getVal() const {
    string valueText = getDOMText();
    return val(stoi(valueText));
}

template <>
val HybridNode<string>::getVal() const {
    string valueText = getDOMText();
    return val(valueText);
}