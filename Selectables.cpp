#include "Selectables.hpp"

#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"

// val CLElement_ = val::global("CLElement");

template <>
string Select<vector<pair<int, string>>>::cppValToString() const {
    // cout << "HybridNode<string>::cppValToString()\n";
    if (cppVal_ == nullptr) return "Select<vector<pair<int, string>>> NULLPTR";
    return string("Number of pair<int, string> in the selection: ") +
           to_string(cppVal_->size());  // Show the first string in the vector of strings.
}

template <>
string Select<vector<string>>::cppValToString() const {
    // cout << "HybridNode<string>::cppValToString()\n";
    if (cppVal_ == nullptr) return "Select<vector<string>> NULLPTR";
    return string("Number of items in the selection: ") +
           to_string(cppVal_->size());  // Show the first string in the vector of strings.
}

template <>
string Checkbox<bool>::cppValToString() const {
    // cout << "HybridNode<string>::cppValToString()\n";
    if (cppVal_ == nullptr) return "Checkbox<bool> NULLPTR";
    if (*cppVal_) return string("TRUE");
    return string("FALSE");
}

template <>
void Checkbox<bool>::setDOMVal(const val &inval) {
    this->domElement_.set("checked", inval);
    cout << "Checkbox::setDOMVal(const val &inval)\n";
}

template <>
val Checkbox<bool>::getVal() const {
    val checked = this->domElement_["checked"];
    cout << "Checkbox<bool>::getVal(): this->domElement_['checked'] = " << checked.as<bool>() << "\n";    
    // if (this->cppVal_ != nullptr) {        
    //     *this->cppVal_ = checked.as<bool>();
    //     cout << "Checkbox cppVal_ is now: " << *this->cppVal_ << "\n";
    // }
    // return val(*this->cppVal_);
    return checked;
}

// template <>
// std::string clarity::clto_str(const vector<string> &v) {
//     std::ostringstream os;
//     os << v[0];
//     return os.str();
// }