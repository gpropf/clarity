#include <sys/time.h>
#include <unistd.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "speedtest.hpp"

map<const int, ClarityNodeBase *> ClarityNodeBase::switchboard;
map<string, std::function<void()>> ClarityNodeBase::callbackMap;
TicketMachine ClarityNodeBase::tm_;
val ClarityNodeBase::DualLink::CLElement_ = val::global("CLElement");

vector<int *> ns;
vector<ModelNode<int> *> mns;
vector<ClarityNodeBase *> clns;

time_t msecs_time() {
    struct timeval time_now {};
    gettimeofday(&time_now, nullptr);
    time_t msecs_time = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

    cout << "seconds since epoch: " << time_now.tv_sec << endl;
    cout << "milliseconds since epoch: " << msecs_time << endl << endl;

    return msecs_time;
}

void destroy_everything() {
    cout << "Vectors clns, ns, and mns contain: " << clns.size() << ", "
         << ns.size() << ", " << mns.size() << " elements respectively.\n";
    for (auto cln : clns) {
        delete cln;
    }
    for (auto i : ns) {
        delete i;
    }
    for (auto mn : mns) {
        delete mn;
    }
    clns.clear();
    ns.clear();
    mns.clear();
}

int *n_input_fields = new int(30);
int *n_fieldsets = new int(100);

template <class Nc, typename V, typename N>
void make_trs(CLNodeFactory<Nc, V, N> builder) {
    time_t t1 = msecs_time();
    for (int j = 0; j < *n_fieldsets; j++) {
        // n_input_fields = new int(90);

        for (int i = 0; i < *n_input_fields; i++) {
            int *iptr = new int(i);
            ModelNode<int> *mn = nullptr;
            ClarityNodeBase *cln = builder.withStoredValueType(CppType::Int)
                                   .withName("cln_" + to_string(i))
                                   .withStoredValue(iptr)
                                   .template extractModelNode<V>(mn)
                                   .trInput();
            ns.push_back(iptr);
            mns.push_back(mn);
            clns.push_back(cln);
        }
        // destroy_everything();
    }
    time_t t2 = msecs_time();
    // msecs_time();
    time_t del_t = t2 - t1;
    cout << "Elapsed time: " << del_t << "\n";
}

// using namespace emscripten;
// EMSCRIPTEN_BINDINGS(Util) {

//         function("make_trs", &make_trs);

// }

int main() {
    val utils_instance = val::global("Util").new_();
    val CLE = val::global("CLElement");
    val doNothing = CLE["doNothing"];
    val destroy_everything_cpp = CLE["destroy_everything"];
    val make_trs_ints = CLE["make_trs_ints"];
    val square = CLE["square"];
    val blackbody_st = CLE["blackbody_st"];

    double *d1 = new double(27.8);
    double *d2 = new double(600.4);

    CLNodeFactory<ClarityNodeBase, double, double> builder("div", "maindiv",
                                                       CppType::NoData);

    ClarityNodeBase *maindiv = builder.build();

    CLNodeFactory<ClarityNodeBase, double, double> childOfMaindivBuilder =
        builder.createChildrenOf(maindiv);

    ModelNode<double> *d1_mn;
    ClarityNodeBase *d1_trinp =
        childOfMaindivBuilder.withStoredValueType(CppType::Double)
            .withName("d1")
            .withStoredValue(d1)
            .extractModelNode<double>(d1_mn)
            .trInput();

    ClarityNodeBase *labelled_d1_trinp =
        childOfMaindivBuilder.labelGivenNode(d1_trinp, "CONST LABEL");

    CLNodeFactory<ClarityNodeBase, int, int> childOfMaindivBuilder_int;
    CLNodeFactory<ClarityNodeBase, int, int>::clone(childOfMaindivBuilder,
                                                childOfMaindivBuilder_int);

    childOfMaindivBuilder_int =
        childOfMaindivBuilder_int.withStoredValueType(CppType::Int);

    ClarityNodeBase *fieldsets_inp =
        childOfMaindivBuilder_int  // .withStoredValueType(CppType::Int)
            .withStoredValue(n_fieldsets)
            .textInput();
    ClarityNodeBase *labelled_fieldsets_inp =
        childOfMaindivBuilder_int.labelGivenNode(fieldsets_inp, "fieldsets");

    ClarityNodeBase *n_input_fields_inp =
        childOfMaindivBuilder_int  // .withStoredValueType(CppType::Int)
            .withStoredValue(n_input_fields)
            .textInput();
    ClarityNodeBase *labelled_n_input_fields_inp =
        childOfMaindivBuilder_int.labelGivenNode(n_input_fields_inp, "fields per set");

    ClarityNodeBase *statusButton = childOfMaindivBuilder.button(
        "statusButton", "BOOM!", destroy_everything_cpp);

    ClarityNodeBase *make_trs_button = childOfMaindivBuilder.button(
        "make_trs_button", "Make the fields!", make_trs_ints);

    printf("Setup complete!\n");

    clarity::ClarityNodeBase::callbackMap["destroy_everything"] = [=] {
        destroy_everything();
    };

    clarity::ClarityNodeBase::callbackMap["make_trs_ints"] = [=] {
        make_trs(childOfMaindivBuilder_int);
    };

    utils_instance.call<void>("utiltest", 3);
    return 0;
}
