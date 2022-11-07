#include "Selectables.hpp"

#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"

template <>
string SimpleSelect<int>::cppValToString() const {
    if (cppVal_ == nullptr) return "SimpleSelect<int> NULLPTR";
    return (string("Current selection index : ") + clto_str(*cppVal_) + string(", ") +
            this->options_[*cppVal_].second);
}

template <>
void SimpleSelect<int>::setDOMVal(const val &inval) {
    cout << "SimpleSelect<int>::setDOMVal(const val &inval)\n";
}

template <>
string Select<vector<pair<int, string>>>::cppValToString() const {
    if (cppVal_ == nullptr) return "Select<vector<pair<int, string>>> NULLPTR";
    return string("Number of pair<int, string> in the selection: ") + clto_str(cppVal_->size());
}

template <>
string Select<vector<string>>::cppValToString() const {
    if (cppVal_ == nullptr) return "Select<vector<string>> NULLPTR";
    return string("Number of strings in the selection: ") + clto_str(cppVal_->size());
}

template <>
string Checkbox<bool>::cppValToString() const {
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
    cout << "Checkbox<bool>::getVal(): this->domElement_['checked'] = " << checked.as<bool>()
         << "\n";
    return checked;
}
