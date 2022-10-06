#include "speedtest.hpp"

#include <sys/time.h>
#include <unistd.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"

map<const int, ClarityNode *> ClarityNode::switchboard;
map<string, std::function<void()>> ClarityNode::callbackMap;
TicketMachine ClarityNode::tm;
val ClarityNode::DualLink::CLElement_ = val::global("CLElement");
template <>
const array<string, 8> CanvasGrid<unsigned char>::colors = {
    "#F5F5DC", "#00FF00", "#00AA00", "#FF00FF",
    "#AA00AA", "#00AAFF", "#9090AA", "#888888"};

vector<int *> ns;
// vector<ModelNode<int> *> mns;
vector<HybridNode<int> *> clns;

time_t msecs_time() {
    struct timeval time_now {};
    gettimeofday(&time_now, nullptr);
    time_t msecs_time = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

    cout << "seconds since epoch: " << time_now.tv_sec << endl;
    cout << "milliseconds since epoch: " << msecs_time << endl << endl;

    return msecs_time;
}

void destroy_everything() {
    cout << "Vectors clns, ns contain: " << clns.size() << ", " << ns.size()
         << " elements respectively.\n";
    for (auto cln : clns) {
        delete cln;
    }
    for (auto i : ns) {
        delete i;
    }

    clns.clear();
    ns.clear();
}

int *n_input_fields = new int(5);
int *n_fieldsets = new int(6);

template <class Nc, typename V, typename N>
void make_trs(CLNodeFactory<Nc, V, N> builder) {
    time_t t1 = msecs_time();
    for (int j = 0; j < *n_fieldsets; j++) {
        // n_input_fields = new int(90);

        for (int i = 0; i < *n_input_fields; i++) {
            int *iptr = new int(i);
            // ModelNode<int> *mn = nullptr;
            HybridNode<int> *cln =
                (CLNodeFactory<HybridNode<int>, int, int>(builder))
                    .withName("cln_" + to_string(i))
                    .withCppVal(iptr)
                    //.template extractModelNode<V>(mn)
                    .trInput();

            cln->setCppVal(iptr);
            cln->refreshDOMValueFromModel();
            cln->pushValToPeers(cln);

            ns.push_back(iptr);
            // mns.push_back(mn);
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
    val nodeAudit = CLE["nodeAudit"];

    // double *d1 = new double(27.8);
    // double *d2 = new double(600.4);

    CLNodeFactory<HybridNode<int>, int, double> builder("div", "maindiv",
                                                        CppType::NoData);

    HybridNode<int> *maindiv = builder.build();

    CLNodeFactory<HybridNode<int>, int, double> childOfMaindivBuilder =
        builder.createChildrenOf(maindiv);

    ModelNode<double> *d1_mn;
    // HybridNode<int> *d1_trinp =
    //     childOfMaindivBuilder
    //         .withName("d1")
    //         .withCppVal(d1)
    //        // .extractModelNode<int>(d1_mn)
    //         .trInput();

    // HybridNode<int> *labelled_d1_trinp =
    //     childOfMaindivBuilder.labelGivenNode(d1_trinp, "CONST LABEL");

    CLNodeFactory<HybridNode<int>, int, int> childOfMaindivBuilder_int;
    CLNodeFactory<HybridNode<int>, int, int>::clone(childOfMaindivBuilder,
                                                    childOfMaindivBuilder_int);

    HybridNode<int> *fieldsets_inp =
        childOfMaindivBuilder_int  // .withStoredValueType(CppType::Int)
            .withCppVal(n_fieldsets)
            .textInput();

    fieldsets_inp->setCppVal(n_fieldsets);
    fieldsets_inp->refreshDOMValueFromModel();
    fieldsets_inp->pushValToPeers(fieldsets_inp);

    HybridNode<int> *labelled_fieldsets_inp =
        childOfMaindivBuilder_int.labelGivenNode(fieldsets_inp, "fieldsets");

    HybridNode<int> *n_input_fields_inp =
        childOfMaindivBuilder_int  // .withStoredValueType(CppType::Int)
            .withCppVal(n_input_fields)
            .textInput();
    HybridNode<int> *labelled_n_input_fields_inp =
        childOfMaindivBuilder_int.labelGivenNode(n_input_fields_inp,
                                                 "fields per set");

    HybridNode<int> *statusButton = childOfMaindivBuilder.button(
        "statusButton", "BOOM!", destroy_everything_cpp);

    HybridNode<int> *make_trs_button = childOfMaindivBuilder.button(
        "make_trs_button", "Make the fields!", make_trs_ints);

    HybridNode<int> *auditButton =
        childOfMaindivBuilder.button("auditButton", "Node Audit", nodeAudit);

    printf("Setup complete!\n");

    clarity::ClarityNode::callbackMap["destroy_everything"] = [=] {
        destroy_everything();
    };

    clarity::ClarityNode::callbackMap["make_trs_ints"] = [=] {
        make_trs(childOfMaindivBuilder_int);
    };

    //utils_instance.call<void>("utiltest", 3);
    return 0;
}
