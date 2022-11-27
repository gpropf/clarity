#ifndef Speedtest_pc_hpp
#define Speedtest_pc_hpp

#include <sys/time.h>
#include <unistd.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <map>

#include "CLNodeFactory.hpp"
#include "ClarityNode.hpp"
#include "PageContent.hpp"
#include "Showcase.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "globals.hpp"

using namespace clarity;

std::function<void()> lbd;
std::function<void(val ev)> updateTotalFields;

/**
 * @brief The actual "testing machine" that creates and destroys node while timing the process.
 *
 */
struct Speedtester {
    int *nInputFields;
    int *nFieldsets;
    int *nSetGroups;
    int *nTotalFields;

    static Speedtester *staticTester;

    vector<int *> ns;
    vector<HybridNode<int> *> clns;
    bool *destroyFieldsImmediately = new bool(false);

    Speedtester() {
        nInputFields = new int(10);
        nFieldsets = new int(3);
        nSetGroups = new int(2);
        nTotalFields = new int(*nInputFields * *nFieldsets * *nSetGroups);
    }

    CLNodeFactory<HybridNode, int, int> *fieldBuilder_;

    static std::string getStringFromInstance(const Speedtester &instance) {
        return clto_str(*instance.nFieldsets);
    }   

    time_t msecsTime() {
        struct timeval timeNow {};
        gettimeofday(&timeNow, nullptr);
        time_t msecsTime = (timeNow.tv_sec * 1000) + (timeNow.tv_usec / 1000);
        return msecsTime;
    }

    void destroyEverything() {
        // cout << "Destroying " << clns.size() << " nodes.\n";
        ClarityNode::nodelogStatic("Destroying " + clto_str(clns.size()) + " nodes.\n",
                                   ClogType::INFO);
        for (auto cln : clns) {
            delete cln;
        }
        for (auto i : ns) {
            delete i;
        }
        clns.clear();
        ns.clear();
    }

    void updateState() {
        *nTotalFields = *nInputFields * *nFieldsets * *nSetGroups;
        if (*nTotalFields > 10000)
            *destroyFieldsImmediately = true;
        else
            *destroyFieldsImmediately = false;
        cout << "UPDATED TOTAL FIELDS: " << *nTotalFields << endl;
    }

    static void updateStateSt(Speedtester *tester) { tester->updateState(); }

    void printState() {
        cout << "nInputFields: " << *nInputFields << endl;
        cout << "nFieldsets: " << *nFieldsets << endl;
        cout << "nSetGroups: " << *nSetGroups << endl;
        cout << "nTotalFields: " << *nTotalFields << endl;
    }

    void makeTrsNoGraph() {
        int totalFieldsToCreate = *nInputFields * *nFieldsets * *nSetGroups;
        cout << "We will be creating a total of " << totalFieldsToCreate << endl;

        int fieldCount = 0;
        int rateCount = 0;
        time_t totalTime = 0;
        for (int k = 0; k < *nSetGroups; k++) {
            cout << "Setgroup: " << k << endl;
            for (int j = 0; j < *nFieldsets; j++) {
                time_t t1 = msecsTime();
                for (int i = 0; i < *nInputFields; i++) {
                    int *iptr = new int(i);
                    auto *cln = (CLNodeFactory<HybridNode, int, int>(*fieldBuilder_))
                                    .withName("cln_" + to_string(fieldCount++))
                                    .withCppVal(iptr)
                                    .trInput();
                    cln->setCppVal(iptr);
                    cln->refresh();
                    ns.push_back(iptr);
                    clns.push_back(cln);
                }
                rateCount++;
                time_t t2 = msecsTime();
                time_t delT = t2 - t1;
                double msPerField = double(delT) / double(*nInputFields);
                cout << "\tSet: " << j << ", ms/field: " << msPerField << endl;
                totalTime += delT;
                double barHeight = msPerField * 50;
                if (*destroyFieldsImmediately) destroyEverything();
            }
        }
        cout << "Total elapsed time: " << totalTime << " (ms)\n";
        cout << "Total fields created: " << fieldCount << "\n";
        double msPerField = double(totalTime) / double(fieldCount);
        cout << "Time per field: " << msPerField << " (ms)\n";
    }

    template <template <typename V> class Nc, typename V, typename N>
    void makeTrs(CLNodeFactory<Nc, V, N> builder, ClarityNode *graphCanvas) {
        val graphCanvasDomElement = graphCanvas->getDomElement();
        val ctx = graphCanvasDomElement.call<val>("getContext", val("2d"));
        ctx.set("strokeStyle", "red");

        int totalFieldsToCreate = *nInputFields * *nFieldsets * *nSetGroups;
        cout << "We will be creating a total of " << totalFieldsToCreate << endl;

        int fieldCount = 0;
        int rateCount = 0;
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
                rateCount++;
                time_t t2 = msecsTime();
                time_t delT = t2 - t1;
                double msPerField = double(delT) / double(*nInputFields);
                cout << "\tSet: " << j << ", ms/field: " << msPerField << endl;
                totalTime += delT;
                double barHeight = msPerField * 50;
                ctx.call<void>("moveTo", val(rateCount), val(150 - barHeight));
                ctx.call<void>("lineTo", val(rateCount), val(150));
                ctx.call<void>("stroke");
                if (*destroyFieldsImmediately) destroyEverything();
            }
        }
        cout << "Total elapsed time: " << totalTime << " (ms)\n";
        cout << "Total fields created: " << fieldCount << "\n";
        double msPerField = double(totalTime) / double(fieldCount);
        cout << "Time per field: " << msPerField << " (ms)\n";
    }
};

EMSCRIPTEN_BINDINGS(speedtester) {
    class_<Speedtester>("Speedtester")
     
        //.class_function("makeFieldsEL", &Speedtester::makeFieldsEL, allow_raw_pointers())
        // .class_function("destroyEverythingEL", &Speedtester::destroyEverythingEL,
        //                 allow_raw_pointers())
        .function("updateState", &Speedtester::updateState, allow_raw_pointers())
        .function("destroyEverything", &Speedtester::destroyEverything, allow_raw_pointers())
        .function("makeTrsNoGraph", &Speedtester::makeTrsNoGraph, allow_raw_pointers());
}

Speedtester *Speedtester::staticTester = nullptr;

/**
 * @brief Contains controls to allow testing speed of creating/destroying nodes.
 *
 */
struct Speedtest : public PageContent {
    std::function<void(val ev)> updateTotalFields_st;
    std::function<void()> lbd;
    std::function<void()> updateTotalFields;

    void runLambda(val ev) { lbd(); }

    static void updateStateEL(val ev) {
        cout << "updateStateEL updateStateEL updateStateEL updateStateEL updateStateEL\n";
        Speedtester::staticTester->updateState();
        updateState_st(ev);
    }

    static std::function<void(val ev)> updateState_st;

    ClarityNode *content(ClarityNode *innerContent = nullptr) {
        ClarityNode::setClogSilent();

        Speedtester::staticTester = new Speedtester();
        ClarityNode::runJSAuxScripts();

        val Module = val::global("Module");
        val Speedtest = Module["Speedtest"];
        val Speedtester = Module["Speedtester"];

        val updateStateEL = Speedtest["updateStateEL"];
        val destroyEverythingEL = val::global("elgDestroyFields")(Speedtester::staticTester);
        val makeFieldsEL = val::global("elgMakeFields")(Speedtester::staticTester);
        val doNothingEL = val::global("doNothingEL");

        CLNodeFactory<HybridNode, int, double> builder("div", "maindiv");
        auto *maindiv = builder.build();
        CLNodeFactory<HybridNode, int, double> childOfMaindivBuilder =
            builder.withChildrenOf(maindiv);

        Speedtester::staticTester->fieldBuilder_ =
            new CLNodeFactory<HybridNode, int, int>(childOfMaindivBuilder);

        auto *nInputFields_inp =
            Speedtester::staticTester->fieldBuilder_->withName("nInputFields_inp")
                .withCppVal(Speedtester::staticTester->nInputFields)
                .textInput();
        auto *labelled_nInputFields_inp = Speedtester::staticTester->fieldBuilder_->labelGivenNode(
            nInputFields_inp, "Fields per set");

        nInputFields_inp->addEventListener(updateStateEL, string("change"));

        auto *nFieldsets_inp = Speedtester::staticTester->fieldBuilder_->withName("fieldsets_inp")
                                   .withCppVal(Speedtester::staticTester->nFieldsets)
                                   .textInput();
        auto *labelled_fieldsets_inp = Speedtester::staticTester->fieldBuilder_->labelGivenNode(
            nFieldsets_inp, "Number of sets");

        nFieldsets_inp->addEventListener(updateStateEL, string("change"));

        auto *nSetGroups_inp = Speedtester::staticTester->fieldBuilder_->withName("nSetGroups_inp")
                                   .withCppVal(Speedtester::staticTester->nSetGroups)
                                   .textInput();
        auto *labelled_nSetGroups_inp = Speedtester::staticTester->fieldBuilder_->labelGivenNode(
            nSetGroups_inp, "Number of set groups");

        nSetGroups_inp->addEventListener(updateStateEL, string("change"));

        auto *nTotalFields_inp =
            Speedtester::staticTester->fieldBuilder_->withName("nTotalFields_inp")
                .withCppVal(Speedtester::staticTester->nTotalFields)
                .textInput();
        auto *labelled_nTotalFields_inp = Speedtester::staticTester->fieldBuilder_->labelGivenNode(
            nTotalFields_inp, "Total Fields");

        auto *boomButton = childOfMaindivBuilder.button("boomButton", "BOOM!", destroyEverythingEL);

        childOfMaindivBuilder.br(); // Taking this *OUT* causes a memory access out of bounds error at runtime!

        auto *makeTrsButton =
            childOfMaindivBuilder.button("makeTrsButton", "Make the fields!", makeFieldsEL);

        auto *updateState_btn =
            childOfMaindivBuilder.button("updateState_btn", "Update State!", updateStateEL);
      
        CLNodeFactory<HybridNode, bool, int> checkboxBuilder(childOfMaindivBuilder);

        auto *destroyFieldsImmediately_cb =
            checkboxBuilder.withCppVal(Speedtester::staticTester->destroyFieldsImmediately)
                .withName("destroyFieldsImmediately_cb")
                .checkbox();

        auto *labelled_destroyFieldsImmediately_cb = childOfMaindivBuilder.label(
            destroyFieldsImmediately_cb, "Destroy fields as we go", true);

        auto *graphCanvas =
            childOfMaindivBuilder.withName("graphCanvas").canvas("canvasTestPattern");

        auto updateTotalFields = [&, nTotalFields_inp, destroyFieldsImmediately_cb](val ev) {
            cout << "updateTotalFields works!\n";
            Speedtester::staticTester->updateState();
            nTotalFields_inp->refresh();
            destroyFieldsImmediately_cb->refresh();
        };
        
        Speedtest::updateState_st = updateTotalFields;

        cout << "Setup complete!\n";
        return maindiv;
    }
};

EMSCRIPTEN_BINDINGS(speedtest) {
    class_<Speedtest>("Speedtest")
        .class_function("updateStateEL", &Speedtest::updateStateEL, allow_raw_pointers());
}

#endif
