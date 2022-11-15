

#include "Speedtest_pc.hpp"

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
//#include "embindings.hpp"
#include "globals.hpp"



// template <>
// string HybridNode<Speedtester>::cppValToString() const {
//     // cout << "HybridNode<string>::cppValToString()\n";
//     if (cppVal_ == nullptr) return "HybridNode<Speedtest> NULLPTR";
//     return string("Speedtest object '" + this->name_ + "' currently set up to generate ") +
//            clto_str(*cppVal_->nTotalFields) + string(" fields\n");  // static_cast<string>(cppVal_);
// }

// Speedtest *Speedtest::singleton = nullptr;

int main() {
    Speedtest speedtest;
    // Speedtest::singleton = &speedtest;
    TestFramework testFramework;
    testFramework.content(speedtest.content());
    return 0;
}
