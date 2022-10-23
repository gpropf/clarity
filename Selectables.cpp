#include "Selectables.hpp"
#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"

//val CLElement_ = val::global("CLElement");

template <>
string Select<vector<string>>::cppValToString() const {
    //cout << "HybridNode<string>::cppValToString()\n";
    if (cppVal_ == nullptr) return "Select<vector<string>> NULLPTR";
    return string ("Number of items in the selection: ") + to_string(cppVal_->size()); // Show the first string in the vector of strings.
}

// template <>
// std::string clarity::clto_str(const vector<string> &v) {
//     std::ostringstream os;
//     os << v[0];
//     return os.str();
// }