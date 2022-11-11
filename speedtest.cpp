

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
    // cout << "Destroying " << clns.size() << " nodes.\n";
    ClarityNode::nodelogStatic("Destroying " + clto_str(clns.size()) + " nodes.\n");
    for (auto cln : clns) {
        delete cln;
    }
    for (auto i : ns) {
        delete i;
    }
    clns.clear();
    ns.clear();
}

int *nInputFields = new int(25);
int *nFieldsets = new int(12);
int *nSetGroups = new int(20);
int *nTotalFields = new int(*nInputFields * *nFieldsets * *nSetGroups);

template <template <typename V> class Nc, typename V, typename N>
void makeTrs(CLNodeFactory<Nc, V, N> builder) {
    int totalFieldsToCreate = *nInputFields * *nFieldsets * *nSetGroups;
    cout << "We will be creating a total of " << totalFieldsToCreate << endl;

    // destroyFieldsImmediately_cb->refresh();
    int fieldCount = 0;
    time_t totalTime = 0;
    for (int k = 0; k < *nSetGroups; k++) {
        cout << "Setgroup: " << k << endl;
        for (int j = 0; j < *nFieldsets; j++) {            
            time_t t1 = msecsTime();
            for (int i = 0; i < *nInputFields; i++) {
                int *iptr = new int(i);
                auto *cln = (CLNodeFactory<HybridNode, int, int>(builder))
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
            double msPerField = double(delT) / double(*nInputFields);
            cout << "\tSet: " << j << ", ms/field: " << msPerField << endl;
            totalTime += delT;
            if (*destroyFieldsImmediately) destroyEverything(val::null());
        }
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
std::function<void()> updateTotalFields;
void runLambda(val ev) { lbd(); }
void runUpdateTotalFields(val ev) { updateTotalFields(); }

EMSCRIPTEN_BINDINGS(speedtest) {
    emscripten::function("cppTestFn", &cppTestFn);
    emscripten::function("destroyEverything", &destroyEverything);
    emscripten::function("runLambda", &runLambda);
    emscripten::function("runUpdateTotalFields", &runUpdateTotalFields);
}

int main() {
    ClarityNode::setClogSilent();

    val cppTestFn = val::global("Module")["cppTestFn"];
    val destroyEverything = val::global("Module")["destroyEverything"];
    val runLambda = val::global("Module")["runLambda"];
    val runUpdateTotalFields = val::global("Module")["runUpdateTotalFields"];
    val blackbody_st = ClarityNode::JSProxyNode_["blackbody_st"];
    val listNodes = ClarityNode::JSProxyNode_["listNodes_int"];

    CLNodeFactory<HybridNode, int, double> builder("div", "maindiv");
    auto *maindiv = builder.build();
    CLNodeFactory<HybridNode, int, double> childOfMaindivBuilder = builder.withChildrenOf(maindiv);

    CLNodeFactory<HybridNode, int, int> childOfMaindivBuilder_int(childOfMaindivBuilder);

    auto *nInputFields_inp =
        childOfMaindivBuilder_int.withName("nInputFields_inp").withCppVal(nInputFields).textInput();
    auto *labelled_nInputFields_inp =
        childOfMaindivBuilder_int.labelGivenNode(nInputFields_inp, "Fields per set");

    nInputFields_inp->addEventListener(runUpdateTotalFields, string("change"));

    auto *nFieldsets_inp =
        childOfMaindivBuilder_int.withName("fieldsets_inp").withCppVal(nFieldsets).textInput();
    auto *labelled_fieldsets_inp =
        childOfMaindivBuilder_int.labelGivenNode(nFieldsets_inp, "Number of sets");

    nFieldsets_inp->addEventListener(runUpdateTotalFields, string("change"));

    auto *nSetGroups_inp =
        childOfMaindivBuilder_int.withName("nSetGroups_inp").withCppVal(nSetGroups).textInput();
    auto *labelled_nSetGroups_inp =
        childOfMaindivBuilder_int.labelGivenNode(nSetGroups_inp, "Number of set groups");

    nSetGroups_inp->addEventListener(runUpdateTotalFields, string("change"));

    auto *nTotalFields_inp =
        childOfMaindivBuilder_int.withName("nTotalFields_inp").withCppVal(nTotalFields).textInput();
    auto *labelled_nTotalFields_inp =
        childOfMaindivBuilder_int.labelGivenNode(nTotalFields_inp, "Total Fields");

    // auto *boomButton =
    //     childOfMaindivBuilder.button("boomButton", "BOOM!", destroy_everything_cpp);
    auto *boomButton = childOfMaindivBuilder.button("boomButton", "BOOM!", destroyEverything);

    // boomButton->addEventListener(destroyEverything, "click");

    auto *helloButton = childOfMaindivBuilder.button("helloButton", "Say Hello!", cppTestFn);
    // auto *makeTrsButton =
    //     childOfMaindivBuilder.button("makeTrsButton", "Make the fields!", make_trs_ints);
    auto *makeTrsButton =
        childOfMaindivBuilder.button("makeTrsButton", "Make the fields!", runLambda);
    auto *listNodes_btn = childOfMaindivBuilder.button("listNodes_btn", "List Nodes", listNodes);

    CLNodeFactory<HybridNode, bool, int> checkboxBuilder(childOfMaindivBuilder);

    auto *destroyFieldsImmediately_cb = checkboxBuilder.withCppVal(destroyFieldsImmediately)
                                            .withName("destroyFieldsImmediately_cb")
                                            .checkbox();

    auto *labelled_destroyFieldsImmediately_cb = childOfMaindivBuilder.labelGivenNode(
        destroyFieldsImmediately_cb, "Destroy fields as we go");

    // clarity::ClarityNode::callbackMap["destroyEverything"] = [=] { destroyEverything(); };
    updateTotalFields = [=] {
        *nTotalFields = *nInputFields * *nFieldsets * *nSetGroups;
        if (*nTotalFields > 10000)
            *destroyFieldsImmediately = true;
        else
            *destroyFieldsImmediately = false;
        cout << "UPDATED TOTAL FIELDS: " << *nTotalFields << endl;
        nTotalFields_inp->refresh();
        destroyFieldsImmediately_cb->refresh();
    };

    lbd = [=] { makeTrs(childOfMaindivBuilder_int); };

    cout << "Setup complete!\n";
    return 0;
}
