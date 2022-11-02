

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
bool *destroyFieldsImmediately = new bool(false);



time_t msecs_time() {
    struct timeval time_now {};
    gettimeofday(&time_now, nullptr);
    time_t msecs_time = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

    // cout << "seconds since epoch: " << time_now.tv_sec << endl;
    // cout << "milliseconds since epoch: " << msecs_time << endl << endl;

    return msecs_time;
}

void destroy_everything() {
    cout << "Destroying " << clns.size() << " nodes.\n";
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

template <template <typename V> class Nc, typename V, typename N>
void make_trs(CLNodeFactory<Nc, V, N> builder) {
    int fieldCount = 0;
    time_t totalTime = 0;
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
        totalTime += del_t;
        if (*destroyFieldsImmediately) destroy_everything();
    }
    cout << "Total elapsed time: " << totalTime << " (ms)\n";
    cout << "Total fields created: " << fieldCount << "\n";
    double msPerField = double(totalTime) / double(fieldCount);
    cout << "Time per field: " << msPerField << " (ms)\n";
}

void cppTestFn(val v) { cout << "I'm a C++ function called from JS.\n"; }

EMSCRIPTEN_BINDINGS(speedtest) { emscripten::function("cppTestFn", &cppTestFn); }

int main() {
    val cppTestFn = val::global("Module")["cppTestFn"];    
    val utils_instance = val::global("Util").new_();    
    val destroy_everything_cpp = ClarityNode::CLElement_["destroy_everything"];
    val make_trs_ints = ClarityNode::CLElement_["make_trs_ints"];    
    val blackbody_st = ClarityNode::CLElement_["blackbody_st"];
    val nodeAudit = ClarityNode::CLElement_["nodeAudit_int"];

    CLNodeFactory<HybridNode, int, double> builder("div", "maindiv");
    HybridNode<int> *maindiv = builder.build();
    CLNodeFactory<HybridNode, int, double> childOfMaindivBuilder =
        builder.createChildrenOf(maindiv);

    CLNodeFactory<HybridNode, int, int> childOfMaindivBuilder_int(childOfMaindivBuilder);

    HybridNode<int> *fieldsets_inp = childOfMaindivBuilder_int.withCppVal(n_fieldsets).textInput();
    HybridNode<int> *labelled_fieldsets_inp =
        childOfMaindivBuilder_int.labelGivenNode(fieldsets_inp, "fieldsets");
    HybridNode<int> *n_input_fields_inp =
        childOfMaindivBuilder_int.withCppVal(n_input_fields).textInput();
    HybridNode<int> *labelled_n_input_fields_inp =
        childOfMaindivBuilder_int.labelGivenNode(n_input_fields_inp, "fields per set");
    HybridNode<int> *statusButton =
        childOfMaindivBuilder.button("statusButton", "BOOM!", destroy_everything_cpp);

    HybridNode<int> *hello_button =
        childOfMaindivBuilder.button("make_trs_button", "Say Hello!", cppTestFn);
    HybridNode<int> *make_trs_button =
        childOfMaindivBuilder.button("make_trs_button", "Make the fields!", make_trs_ints);
    HybridNode<int> *auditButton =
        childOfMaindivBuilder.button("auditButton", "Node Audit", nodeAudit);

    CLNodeFactory<HybridNode, bool, int> checkboxBuilder(childOfMaindivBuilder);    

    auto *destroyFieldsImmediately_cb = checkboxBuilder.withCppVal(destroyFieldsImmediately)
                                            .withName("destroyFieldsImmediately_cb")
                                            .checkbox();

    auto *labelled_destroyFieldsImmediately_cb =
        checkboxBuilder.labelGivenNode(destroyFieldsImmediately_cb, "Destroy fields as we go");

    clarity::ClarityNode::callbackMap["destroy_everything"] = [=] { destroy_everything(); };
    clarity::ClarityNode::callbackMap["make_trs_ints"] = [=] {
        make_trs(childOfMaindivBuilder_int);
    };

    printf("Setup complete!\n");
    return 0;
}
