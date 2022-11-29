#ifndef Speedtest_hpp
#define Speedtest_hpp

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

// std::function<void()> lbd;
std::function<void(val ev)> updateTotalFields;

/**
 * @brief The actual "testing machine" that creates and destroys node while timing the process.
 *
 */
struct Speedtester {
    int *nInputFields_;
    int *nFieldsets_;
    int *nSetGroups_;
    int *nTotalFields_;

    static Speedtester *staticTester_;

    vector<int *> ns_;
    vector<HybridNode<int> *> clns_;
    bool *destroyFieldsImmediately_ = new bool(false);

    Speedtester() {
        nInputFields_ = new int(50);
        nFieldsets_ = new int(25);
        nSetGroups_ = new int(3);
        nTotalFields_ = new int(*nInputFields_ * *nFieldsets_ * *nSetGroups_);
    }

    CLNodeFactory<HybridNode, int, int> *fieldBuilder_;
    CanvasElement<int> *graphCanvas_;

    static std::string getStringFromInstance(const Speedtester &instance) {
        return clto_str(*instance.nFieldsets_);
    }

    time_t msecsTime() {
        struct timeval timeNow {};
        gettimeofday(&timeNow, nullptr);
        time_t msecsTime = (timeNow.tv_sec * 1000) + (timeNow.tv_usec / 1000);
        return msecsTime;
    }

    void destroyEverything() {
        // cout << "Destroying " << clns_.size() << " nodes.\n";
        ClarityNode::nodelogStatic("Destroying " + clto_str(clns_.size()) + " nodes.\n",
                                   ClogType::INFO);
        for (auto cln : clns_) {
            delete cln;
        }
        for (auto i : ns_) {
            delete i;
        }
        clns_.clear();
        ns_.clear();
    }

    void updateState() {
        *nTotalFields_ = *nInputFields_ * *nFieldsets_ * *nSetGroups_;
        if (*nTotalFields_ > 10000)
            *destroyFieldsImmediately_ = true;
        else
            *destroyFieldsImmediately_ = false;
        cout << "UPDATED TOTAL FIELDS: " << *nTotalFields_ << endl;
    }

    static void updateStateSt(Speedtester *tester) { tester->updateState(); }

    void printState() {
        cout << "nInputFields_: " << *nInputFields_ << endl;
        cout << "nFieldsets_: " << *nFieldsets_ << endl;
        cout << "nSetGroups_: " << *nSetGroups_ << endl;
        cout << "nTotalFields_: " << *nTotalFields_ << endl;
    }

    void makeTrsNoArgs() {
        val graphCanvasDomElement = Speedtester::staticTester_->graphCanvas_->getDomElement();
        val clearCanvasUsingDomElement = ClarityNode::JSProxyNode_["clearCanvasUsingDomElement"];
        clearCanvasUsingDomElement(graphCanvasDomElement, val("#223388"));
        // graphCanvasDomElement.call<void>
        // graphCanvasDomElement.call<val>("clearCanvasUsingDomElement", graphCanvasDomElement,
        // val("#aa4567"));
        val ctx = graphCanvasDomElement.call<val>("getContext", val("2d"));
        ctx.set("strokeStyle", "red");
        int totalFieldsToCreate = *nInputFields_ * *nFieldsets_ * *nSetGroups_;
        cout << "We will be creating a total of " << totalFieldsToCreate << endl;
        int totalRateMeasurements = *nSetGroups_ * *nFieldsets_;
        double measurementWidth = 300 / totalRateMeasurements;
        int fieldCount = 0;
        int rateCount = 0;
        time_t totalTime = 0;
        for (int k = 0; k < *nSetGroups_; k++) {
            cout << "Setgroup: " << k << endl;
            for (int j = 0; j < *nFieldsets_; j++) {
                time_t t1 = msecsTime();
                for (int i = 0; i < *nInputFields_; i++) {
                    int *iptr = new int(i);
                    auto *cln = (CLNodeFactory<HybridNode, int, int>(*fieldBuilder_))
                                    .withName("cln_" + to_string(fieldCount++))
                                    .withCppVal(iptr)
                                    .trInput();
                    cln->setCppVal(iptr);
                    cln->refresh();
                    ns_.push_back(iptr);
                    clns_.push_back(cln);
                }
                rateCount++;
                time_t t2 = msecsTime();
                time_t delT = t2 - t1;
                double msPerField = double(delT) / double(*nInputFields_);
                cout << "\tSet: " << j << ", ms/field: " << msPerField << endl;
                totalTime += delT;
                double barHeight = msPerField * 50;
                ctx.call<void>("moveTo", val(rateCount * measurementWidth), val(150));
                ctx.call<void>("lineTo", val(rateCount * measurementWidth), val(150 - barHeight));
                ctx.call<void>("stroke");
                if (*destroyFieldsImmediately_) destroyEverything();
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

        int totalFieldsToCreate = *nInputFields_ * *nFieldsets_ * *nSetGroups_;
        cout << "We will be creating a total of " << totalFieldsToCreate << endl;

        int fieldCount = 0;
        int rateCount = 0;
        time_t totalTime = 0;
        for (int k = 0; k < *nSetGroups_; k++) {
            cout << "Setgroup: " << k << endl;
            for (int j = 0; j < *nFieldsets_; j++) {
                time_t t1 = msecsTime();
                for (int i = 0; i < *nInputFields_; i++) {
                    int *iptr = new int(i);
                    auto *cln = (CLNodeFactory<HybridNode, int, int>(builder))
                                    .withName("cln_" + to_string(fieldCount++))
                                    .withCppVal(iptr)
                                    .trInput();
                    cln->setCppVal(iptr);
                    cln->refresh();
                    ns_.push_back(iptr);
                    clns_.push_back(cln);
                }
                rateCount++;
                time_t t2 = msecsTime();
                time_t delT = t2 - t1;
                double msPerField = double(delT) / double(*nInputFields_);
                cout << "\tSet: " << j << ", ms/field: " << msPerField << endl;
                totalTime += delT;
                double barHeight = msPerField * 50;
                ctx.call<void>("moveTo", val(rateCount), val(150 - barHeight));
                ctx.call<void>("lineTo", val(rateCount), val(150));
                ctx.call<void>("stroke");
                if (*destroyFieldsImmediately_) destroyEverything();
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
        .function("makeTrsNoArgs", &Speedtester::makeTrsNoArgs, allow_raw_pointers());
}

Speedtester *Speedtester::staticTester_ = nullptr;

/**
 * @brief Contains controls to allow testing speed of creating/destroying nodes.
 *
 */
struct Speedtest : public PageContent {
    // std::function<void(val ev)> updateTotalFields_st;
    // std::function<void()> lbd_;
    // std::function<void()> updateTotalFields_;

    // void runLambda(val ev) { lbd_(); }

    static void updateStateEL(val ev) {
        cout << "updateStateEL updateStateEL updateStateEL updateStateEL updateStateEL\n";
        Speedtester::staticTester_->updateState();
        updateStateStatic_(ev);
    }

    static std::function<void(val ev)> updateStateStatic_;

    ClarityNode *content(ClarityNode *innerContent = nullptr) {
        ClarityNode::setClogSilent();

        Speedtester::staticTester_ = new Speedtester();
        ClarityNode::runJSAuxScripts();

        val Module = val::global("Module");
        val Speedtest = Module["Speedtest"];
        val Speedtester = Module["Speedtester"];

        val updateStateEL = Speedtest["updateStateEL"];
        val destroyEverythingEL = val::global("elgDestroyFields")(Speedtester::staticTester_);
        val makeFieldsEL = val::global("elgMakeFields")(Speedtester::staticTester_);
        val doNothingEL = val::global("doNothingEL");

        CLNodeFactory<HybridNode, int, double> builder("div", "maindiv");
        auto *maindiv = builder.build();
        CLNodeFactory<HybridNode, int, double> childOfMaindivBuilder =
            builder.withChildrenOf(maindiv);

        Speedtester::staticTester_->fieldBuilder_ =
            new CLNodeFactory<HybridNode, int, int>(childOfMaindivBuilder);

        auto *nInputFields_inp =
            Speedtester::staticTester_->fieldBuilder_->withName("nInputFields_inp")
                .withCppVal(Speedtester::staticTester_->nInputFields_)
                .textInput();
        auto *labelled_nInputFields_inp = Speedtester::staticTester_->fieldBuilder_->labelGivenNode(
            nInputFields_inp, "Fields per set");

        nInputFields_inp->addEventListener(updateStateEL, string("change"));

        auto *nFieldsets_inp = Speedtester::staticTester_->fieldBuilder_->withName("fieldsets_inp")
                                   .withCppVal(Speedtester::staticTester_->nFieldsets_)
                                   .textInput();
        auto *labelled_fieldsets_inp = Speedtester::staticTester_->fieldBuilder_->labelGivenNode(
            nFieldsets_inp, "Number of sets");

        nFieldsets_inp->addEventListener(updateStateEL, string("change"));

        auto *nSetGroups_inp = Speedtester::staticTester_->fieldBuilder_->withName("nSetGroups_inp")
                                   .withCppVal(Speedtester::staticTester_->nSetGroups_)
                                   .textInput();
        auto *labelled_nSetGroups_inp = Speedtester::staticTester_->fieldBuilder_->labelGivenNode(
            nSetGroups_inp, "Number of set groups");

        nSetGroups_inp->addEventListener(updateStateEL, string("change"));

        auto *nTotalFields_inp =
            Speedtester::staticTester_->fieldBuilder_->withName("nTotalFields_inp")
                .withCppVal(Speedtester::staticTester_->nTotalFields_)
                .textInput();
        auto *labelled_nTotalFields_inp = Speedtester::staticTester_->fieldBuilder_->labelGivenNode(
            nTotalFields_inp, "Total Fields");

        auto *boomButton = childOfMaindivBuilder.button("boomButton", "BOOM!", destroyEverythingEL);

        // childOfMaindivBuilder.br(); // Taking this *OUT* causes a memory access out of bounds
        // error at runtime!

        auto *makeTrsButton =
            childOfMaindivBuilder.button("makeTrsButton", "Make the fields!", makeFieldsEL);

        // auto *updateState_btn =
        //     childOfMaindivBuilder.button("updateState_btn", "Update State!", updateStateEL);

        CLNodeFactory<HybridNode, bool, int> checkboxBuilder(childOfMaindivBuilder);

        auto *destroyFieldsImmediately_cb =
            checkboxBuilder.withCppVal(Speedtester::staticTester_->destroyFieldsImmediately_)
                .withName("destroyFieldsImmediately_cb")
                .checkbox();

        auto *labelled_destroyFieldsImmediately_cb = childOfMaindivBuilder.label(
            destroyFieldsImmediately_cb, "Destroy fields as we go", true);

        Speedtester::staticTester_->graphCanvas_ =
            childOfMaindivBuilder.withName("graphCanvas_").canvas("canvasTestPattern");

        auto updateTotalFields = [&, nTotalFields_inp, destroyFieldsImmediately_cb](val ev) {
            cout << "updateTotalFields works!\n";
            Speedtester::staticTester_->updateState();
            nTotalFields_inp->refresh();
            destroyFieldsImmediately_cb->refresh();
        };

        Speedtest::updateStateStatic_ = updateTotalFields;

        cout << "Setup complete!\n";
        return maindiv;
    }
};

EMSCRIPTEN_BINDINGS(speedtest) {
    class_<Speedtest>("Speedtest")
        .class_function("updateStateEL", &Speedtest::updateStateEL, allow_raw_pointers());
}

#endif
