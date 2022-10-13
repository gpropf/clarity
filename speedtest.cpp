

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
#include "globals.hpp"

vector<int *> ns;
vector<HybridNode<int> *> clns;

template <>
const array<string, 8> CanvasGrid<unsigned char>::colors = {"#F5F5DC", "#00FF00", "#00AA00", "#FF00FF",
                                                            "#AA00AA", "#00AAFF", "#9090AA", "#888888"};
// We need this here because embindings.hpp pulls in CanvasElement and then you need to define the colors. Need to make
// embindings.hpp less general.

time_t msecs_time() {
    struct timeval time_now {};
    gettimeofday(&time_now, nullptr);
    time_t msecs_time = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

    cout << "seconds since epoch: " << time_now.tv_sec << endl;
    cout << "milliseconds since epoch: " << msecs_time << endl << endl;

    return msecs_time;
}

void destroy_everything() {
    cout << "Vectors clns, ns contain: " << clns.size() << ", " << ns.size() << " elements respectively.\n";
    for (auto cln : clns) {
        delete cln;
    }
    for (auto i : ns) {
        delete i;
    }
    clns.clear();
    ns.clear();
}

int *n_input_fields = new int(30);
int *n_fieldsets = new int(5);

template <template<typename V> class Nc, typename V, typename N>
void make_trs(CLNodeFactory<Nc, V, N> builder) {
    int fieldCount = 0;
    for (int j = 0; j < *n_fieldsets; j++) {
        time_t t1 = msecs_time();
        for (int i = 0; i < *n_input_fields; i++) {
            int *iptr = new int(i);
            HybridNode<int> *cln = (CLNodeFactory<HybridNode, int, int>(builder))
                                       .withName("cln_" + to_string(fieldCount++))
                                       .withCppVal(iptr)
                                       .trInput();
            cln->setCppVal(iptr);
            cln->refresh();
            ns.push_back(iptr);
            clns.push_back(cln);
        }
        time_t t2 = msecs_time();
        time_t del_t = t2 - t1;
        cout << "Elapsed time: " << del_t << "\n";
        //destroy_everything();
    }
}

int main() {
    val utils_instance = val::global("Util").new_();
    val CLE = val::global("CLElement");
    val doNothing = CLE["doNothing"];
    val destroy_everything_cpp = CLE["destroy_everything"];
    val make_trs_ints = CLE["make_trs_ints"];
    val square = CLE["square"];
    val blackbody_st = CLE["blackbody_st"];
    val nodeAudit = CLE["nodeAudit_int"];

    double *d1 = new double(27.8);
    ModelNode<double> *amn = new ModelNode<double>(d1, "independently_created_modelnode");
    CLNodeFactory<HybridNode, int, double> builder("div", "maindiv");
    HybridNode<int> *maindiv = builder.build();
    CLNodeFactory<HybridNode, int, double> childOfMaindivBuilder = builder.createChildrenOf(maindiv);
    
    CLNodeFactory<HybridNode, int, int> childOfMaindivBuilder_int(childOfMaindivBuilder);

    HybridNode<int> *fieldsets_inp = childOfMaindivBuilder_int.withCppVal(n_fieldsets).textInput();
    HybridNode<int> *labelled_fieldsets_inp = childOfMaindivBuilder_int.labelGivenNode(fieldsets_inp, "fieldsets");
    HybridNode<int> *n_input_fields_inp = childOfMaindivBuilder_int.withCppVal(n_input_fields).textInput();
    HybridNode<int> *labelled_n_input_fields_inp =
        childOfMaindivBuilder_int.labelGivenNode(n_input_fields_inp, "fields per set");
    HybridNode<int> *statusButton = childOfMaindivBuilder.button("statusButton", "BOOM!", destroy_everything_cpp);
    HybridNode<int> *make_trs_button =
        childOfMaindivBuilder.button("make_trs_button", "Make the fields!", make_trs_ints);
    HybridNode<int> *auditButton = childOfMaindivBuilder.button("auditButton", "Node Audit", nodeAudit);
    clarity::ClarityNode::callbackMap["destroy_everything"] = [=] { destroy_everything(); };
    clarity::ClarityNode::callbackMap["make_trs_ints"] = [=] { make_trs(childOfMaindivBuilder_int); };

    printf("Setup complete!\n");
    return 0;
}
