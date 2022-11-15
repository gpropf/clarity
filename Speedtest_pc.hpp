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

class MyClass {
   public:
    MyClass(int x, std::string y) : x(x), y(y) {}

    void incrementX() { ++x; }

    int getX() const { return x; }
    void setX(int x_) { x = x_; }

    static std::string getStringFromInstance(const MyClass &instance) { return instance.y; }

    static std::function<void()> getCallback(const MyClass &instance) {
        std::function<void()> lbd = []() { cout << "CALLBACK!" << endl;};
        //return string("FOO");
        return lbd;
    }

   private:
    int x;
    std::string y;
};

struct Speedtester {
    int *nInputFields;
    int *nFieldsets;
    int *nSetGroups;
    int *nTotalFields;

    vector<int *> ns;
    vector<HybridNode<int> *> clns;
    bool *destroyFieldsImmediately = new bool(false);

    Speedtester() {
        nInputFields = new int(21);
        nFieldsets = new int(11);
        nSetGroups = new int(17);
        nTotalFields = new int(*nInputFields * *nFieldsets * *nSetGroups);
    }

    static std::string getStringFromInstance(const Speedtester &instance) {
        return clto_str(*instance.nFieldsets);
    }

    static auto getCallback(const Speedtester &instance) {
        auto lbd = []() { cout << "CALLBACK!" << endl;};
        return string("FOO");
        //return lbd;
    }

    static void cppTestFn(val ev) {
        cout << "I'm a C++ function called from JS.\n"
             << "Event: " << ev["type"].as<string>() << endl;
    }

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

    void printState() {
        cout << "nInputFields: " << *nInputFields << endl;
        cout << "nFieldsets: " << *nFieldsets << endl;
        cout << "nSetGroups: " << *nSetGroups << endl;
        cout << "nTotalFields: " << *nTotalFields << endl;
    }

    template <template <typename V> class Nc, typename V, typename N>
    void makeTrs(CLNodeFactory<Nc, V, N> builder, ClarityNode *graphCanvas) {
        val graphCanvasDomElement = graphCanvas->getDomElement();
        val ctx = graphCanvasDomElement.call<val>("getContext", val("2d"));
        ctx.set("strokeStyle", "red");
        // ctx.call<void>("moveTo", val(0), val(0));
        // ctx.call<void>("lineTo", val(200), val(100));
        // ctx.call<void>("stroke");
        // ctx.moveTo(0, 0);
        // ctx.lineTo(200, 100);
        // ctx.stroke();
        int totalFieldsToCreate = *nInputFields * *nFieldsets * *nSetGroups;
        cout << "We will be creating a total of " << totalFieldsToCreate << endl;

        // destroyFieldsImmediately_cb->refresh();
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
                if (*destroyFieldsImmediately) destroyEverything(val::null());
            }
        }
        cout << "Total elapsed time: " << totalTime << " (ms)\n";
        cout << "Total fields created: " << fieldCount << "\n";
        double msPerField = double(totalTime) / double(fieldCount);
        cout << "Time per field: " << msPerField << " (ms)\n";
    }

    auto makeTestEL() {
        // using namespace std::placeholders;
        cout << "Speedtester::makeTestEL()\n";
        auto lbd = [&](val ev) { this->printState(); };
        return lbd;
    }

    static auto makeTestEL_static(Speedtester *st) {
        auto lbd = [&](val ev) { st->printState(); };
        return lbd;
    }

    static auto makeTestEL_static_i() {
        // auto lbd = [&](val ev) { cout << "Int ptr: " << *i << endl;; };
        auto lbd = [&] {
            cout << "Static function" << endl;
            ;
        };
        return lbd;
    }
};

EMSCRIPTEN_BINDINGS(my_class_example) {
    class_<MyClass>("MyClass")
        .constructor<int, std::string>()
        .function("incrementX", &MyClass::incrementX)
        .property("x", &MyClass::getX, &MyClass::setX)
        .class_function("getCallback", &MyClass::getCallback)
        .class_function("getStringFromInstance", &MyClass::getStringFromInstance);
}

EMSCRIPTEN_BINDINGS(speedtester) {
    class_<Speedtester>("Speedtester")
        // .constructor<string>(allow_raw_pointers())
        //.constructor<>(allow_raw_pointers())
        .class_function("getStringFromInstance", &Speedtester::getStringFromInstance)
        .class_function("getCallback", &Speedtester::getCallback)
        .class_function("cppTestFn", &Speedtester::cppTestFn)
        .class_function("makeTestEL_static", &Speedtester::makeTestEL_static, allow_raw_pointers())
        .class_function("makeTestEL_static_i", &Speedtester::makeTestEL_static_i,
                        allow_raw_pointers())
        // .class_function("destroyEverything_this", &Speedtest::destroyEverything_this,
        //                 allow_raw_pointers())
        // .class_function("destroyEverything_shell", &Speedtester::destroyEverything_shell,
        //                 allow_raw_pointers())
        .function("destroyEverything", &Speedtester::destroyEverything, allow_raw_pointers());
}

struct Speedtest : public PageContent {
    std::function<void(val ev)> updateTotalFields_st;

    Speedtester *st = new Speedtester();

    // Speedtest(const string &name) { this->name_ = name; }

    // static val destroyEverything_this(Speedtest * speedtest) {
    //     using namespace std::placeholders;
    //     cout << "Speedtest::destroyEverything_this()\n";
    //     return val(std::bind(&Speedtest::destroyEverything, speedtest, _1));
    // }

    //     void runLambda(val ev) { lbd(); }
    // static void runUpdateTotalFields(val ev) { updateTotalFields(ev); }
    // void runUpdateTotalFields_st(val ev) { updateTotalFields_st(ev); }
    // void showname(val ev) { cout << this->name_ << endl; }

    ClarityNode *content(ClarityNode *innerContent = nullptr) {
        ClarityNode::setClogSilent();

        val Module = val::global("Module");
        val MyClassJS = Module["MyClass"];
        val Speedtest = Module["Speedtest"];
        val Speedtester = Module["Speedtester"];
        val makeTestEL_static_fn = Speedtester["makeTestEL_static"];
        val makeTestEL_static_i_fn = Speedtester["makeTestEL_static_i"];

        val mycfn = MyClassJS["getCallback"];
        val stfn = Speedtester["getCallback"];
        // Speedtest.call<void>("cppTestFn", val::null());
        val cppTestFn = Speedtester["cppTestFn"];
        // val destroyEverything_this = Speedtest["destroyEverything_this"];
        // val destroyEverything = Speedtest["destroyEverything"];
        // val destroyEverything_el = destroyEverything_this(this);
        val destroyEverything_shell = Speedtester["destroyEverything_shell"];
        // val runLambda = Speedtest["runLambda"];
        // val runUpdateTotalFields = Speedtest["runUpdateTotalFields"];
        val blackbody_st = ClarityNode::JSProxyNode_["blackbody_st"];
        val listNodes = ClarityNode::JSProxyNode_["listNodes_int"];

        CLNodeFactory<HybridNode, int, double> builder("div", "maindiv");
        auto *maindiv = builder.build();
        CLNodeFactory<HybridNode, int, double> childOfMaindivBuilder =
            builder.withChildrenOf(maindiv);

        CLNodeFactory<HybridNode, int, int> childOfMaindivBuilder_int(childOfMaindivBuilder);

        CLNodeFactory<HybridNode, struct Speedtest, int> childOfMaindivBuilder_speedtest(
            childOfMaindivBuilder);

        // //auto *nameLabel =
        // childOfMaindivBuilder_speedtest.withName("st").withTag("div").withCppVal(this).build();
        // auto *nameLabel = childOfMaindivBuilder_speedtest.withName("speedtest")
        //                       .withTag("div")
        //                       .withCppVal(this)
        //                       .build();

        auto *nInputFields_inp = childOfMaindivBuilder_int.withName("nInputFields_inp")
                                     .withCppVal(st->nInputFields)
                                     .textInput();
        auto *labelled_nInputFields_inp =
            childOfMaindivBuilder_int.labelGivenNode(nInputFields_inp, "Fields per set");

        // nInputFields_inp->addEventListener(runUpdateTotalFields, string("change"));

        auto *nFieldsets_inp = childOfMaindivBuilder_int.withName("fieldsets_inp")
                                   .withCppVal(st->nFieldsets)
                                   .textInput();
        auto *labelled_fieldsets_inp =
            childOfMaindivBuilder_int.labelGivenNode(nFieldsets_inp, "Number of sets");

        // nFieldsets_inp->addEventListener(runUpdateTotalFields, string("change"));

        auto *nSetGroups_inp = childOfMaindivBuilder_int.withName("nSetGroups_inp")
                                   .withCppVal(st->nSetGroups)
                                   .textInput();
        auto *labelled_nSetGroups_inp =
            childOfMaindivBuilder_int.labelGivenNode(nSetGroups_inp, "Number of set groups");

        // val eltest = val(makeTestEL(this));
        int *iptr = new int(88);
        MyClass foo(3, "threefoo");
        auto r = stfn(*st);


        auto mycr = mycfn(foo);
        //cout << "FOO: " << s.as<string>() << endl;
        // auto eltest = makeTestEL_static_i_fn();
        //  val stPrintState = val(st->makeTestEL());

        // auto stPrintState = Speedtester::makeTestEL_static(st);
        // nSetGroups_inp->addEventListener(val(stPrintState), string("change"));

        auto *nTotalFields_inp = childOfMaindivBuilder_int.withName("nTotalFields_inp")
                                     .withCppVal(st->nTotalFields)
                                     .textInput();
        auto *labelled_nTotalFields_inp =
            childOfMaindivBuilder_int.labelGivenNode(nTotalFields_inp, "Total Fields");

        // auto *boomButton =
        //     childOfMaindivBuilder.button("boomButton", "BOOM!", destroy_everything_cpp);

        // auto *showname_btn = childOfMaindivBuilder.button("showname", "Show name", showname);

        auto *boomButton =
            childOfMaindivBuilder.button("boomButton", "BOOM!", destroyEverything_shell);

        // boomButton->addEventListener(destroyEverything, "click");

        auto *helloButton = childOfMaindivBuilder.button("helloButton", "Say Hello!", cppTestFn);
        // auto *makeTrsButton =
        //     childOfMaindivBuilder.button("makeTrsButton", "Make the fields!", make_trs_ints);

        // auto *makeTrsButton =
        //     childOfMaindivBuilder.button("makeTrsButton", "Make the fields!", runLambda);

        auto *listNodes_btn =
            childOfMaindivBuilder.button("listNodes_btn", "List Nodes", listNodes);

        CLNodeFactory<HybridNode, bool, int> checkboxBuilder(childOfMaindivBuilder);

        auto *destroyFieldsImmediately_cb = checkboxBuilder.withCppVal(st->destroyFieldsImmediately)
                                                .withName("destroyFieldsImmediately_cb")
                                                .checkbox();

        auto *labelled_destroyFieldsImmediately_cb = childOfMaindivBuilder.label(
            destroyFieldsImmediately_cb, "Destroy fields as we go", true);

        // auto *graphCanvas =
        //     childOfMaindivBuilder.withName("graphCanvas").canvas("canvasTestPattern");

        // auto *labelled_destroyFieldsImmediately_cb = childOfMaindivBuilder.labelGivenNode(
        //     destroyFieldsImmediately_cb, "Destroy fields as we go");

        // clarity::ClarityNode::callbackMap["destroyEverything"] = [=] { destroyEverything(); };

        // updateTotalFields_st = [&, this](val ev) { printState(); };

        // updateTotalFields = [&, this](val ev) {
        //     printState();

        //     *this->nTotalFields = *this->nInputFields * *this->nFieldsets * *this->nSetGroups;
        //     if (*this->nTotalFields > 10000)
        //         *this->destroyFieldsImmediately = true;
        //     else
        //         *this->destroyFieldsImmediately = false;
        //     cout << "UPDATED TOTAL FIELDS: " << *this->nTotalFields << endl;
        //     nTotalFields_inp->refresh();
        //     destroyFieldsImmediately_cb->refresh();
        // };

        // lbd = [=] { makeTrs(childOfMaindivBuilder_int, graphCanvas); };

        cout << "Setup complete!\n";
        return maindiv;
    }
};

#endif
