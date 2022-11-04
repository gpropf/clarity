

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

time_t msecsTime() {
    struct timeval timeNow {};
    gettimeofday(&timeNow, nullptr);
    time_t msecsTime = (timeNow.tv_sec * 1000) + (timeNow.tv_usec / 1000);

    // cout << "seconds since epoch: " << time_now.tv_sec << endl;
    // cout << "milliseconds since epoch: " << msecs_time << endl << endl;

    return msecsTime;
}

void destroyEverything(val ev) {
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

int *nInputFields = new int(12);
int *nFieldsets = new int(3);

template <template <typename V> class Nc, typename V, typename N>
void makeTrs(CLNodeFactory<Nc, V, N> builder) {
    int fieldCount = 0;
    time_t totalTime = 0;
    for (int j = 0; j < *nFieldsets; j++) {
        time_t t1 = msecsTime();
        for (int i = 0; i < *nInputFields; i++) {
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
        time_t t2 = msecsTime();
        time_t delT = t2 - t1;
        totalTime += delT;
        if (*destroyFieldsImmediately) destroyEverything(val::null());
    }
    cout << "Total elapsed time: " << totalTime << " (ms)\n";
    cout << "Total fields created: " << fieldCount << "\n";
    double msPerField = double(totalTime) / double(fieldCount);
    cout << "Time per field: " << msPerField << " (ms)\n";
}

void cppTestFn(val ev) {
    cout << "I'm a C++ function called from JS.\n"
         << "Event: " << ev["type"].as<string>() << endl;
}

std::function<void()> lbd;
void runLambda(val ev) {
    lbd();
}

EMSCRIPTEN_BINDINGS(speedtest) {
    emscripten::function("cppTestFn", &cppTestFn);
    emscripten::function("destroyEverything", &destroyEverything);
    emscripten::function("runLambda", &runLambda);
}

int main() {
    val cppTestFn = val::global("Module")["cppTestFn"];
    val destroyEverything = val::global("Module")["destroyEverything"];
    val runLambda = val::global("Module")["runLambda"];    
    val blackbody_st = ClarityNode::CLElement_["blackbody_st"];
    val nodeAudit = ClarityNode::CLElement_["nodeAudit_int"];

    CLNodeFactory<HybridNode, int, double> builder("div", "maindiv");
    HybridNode<int> *maindiv = builder.build();
    CLNodeFactory<HybridNode, int, double> childOfMaindivBuilder =
        builder.createChildrenOf(maindiv);

    CLNodeFactory<HybridNode, int, int> childOfMaindivBuilder_int(childOfMaindivBuilder);

    HybridNode<int> *fieldsets_inp = childOfMaindivBuilder_int.withCppVal(nFieldsets).textInput();
    HybridNode<int> *labelled_fieldsets_inp =
        childOfMaindivBuilder_int.labelGivenNode(fieldsets_inp, "fieldsets");
    HybridNode<int> *nInputFields_inp =
        childOfMaindivBuilder_int.withCppVal(nInputFields).textInput();
    HybridNode<int> *labelled_nInputFields_inp =
        childOfMaindivBuilder_int.labelGivenNode(nInputFields_inp, "fields per set");
    // HybridNode<int> *statusButton =
    //     childOfMaindivBuilder.button("statusButton", "BOOM!", destroy_everything_cpp);
    HybridNode<int> *statusButton = childOfMaindivBuilder.button("statusButton", "BOOM!", destroyEverything);

    //statusButton->addEventListener(destroyEverything, "click");

    HybridNode<int> *hello_button =
        childOfMaindivBuilder.button("make_trs_button", "Say Hello!", cppTestFn);
    // HybridNode<int> *make_trs_button =
    //     childOfMaindivBuilder.button("make_trs_button", "Make the fields!", make_trs_ints);
    HybridNode<int> *make_trs_button =
        childOfMaindivBuilder.button("make_trs_button", "Make the fields!", runLambda);
    HybridNode<int> *auditButton =
        childOfMaindivBuilder.button("auditButton", "Node Audit", nodeAudit);

    CLNodeFactory<HybridNode, bool, int> checkboxBuilder(childOfMaindivBuilder);

    auto *destroyFieldsImmediately_cb = checkboxBuilder.withCppVal(destroyFieldsImmediately)
                                            .withName("destroyFieldsImmediately_cb")
                                            .checkbox();

    auto *labelled_destroyFieldsImmediately_cb =
        checkboxBuilder.labelGivenNode(destroyFieldsImmediately_cb, "Destroy fields as we go");

    // clarity::ClarityNode::callbackMap["destroyEverything"] = [=] { destroyEverything(); };
    

    lbd = [=] {
        makeTrs(childOfMaindivBuilder_int);
    };

    printf("Setup complete!\n");
    return 0;
}
