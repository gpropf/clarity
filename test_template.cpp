#include <sys/time.h>
#include <unistd.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "testbed.hpp"

map<const int, ClarityNode *> ClarityNode::switchboard;
map<string, std::function<void()>> ClarityNode::callbackMap;
TicketMachine ClarityNode::tm;
val ClarityNode::DualLink::CLElement_ = val::global("CLElement");

vector<int *> ns;
vector<ModelNode<int> *> mns;
vector<ClarityNode *> clns;

time_t msecs_time() {
    struct timeval time_now {};
    gettimeofday(&time_now, nullptr);
    time_t msecs_time = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

    cout << "seconds since epoch: " << time_now.tv_sec << endl;
    cout << "milliseconds since epoch: " << msecs_time << endl << endl;

    return msecs_time;
}

void destroy_everything() {
    for (auto cln : clns) {
        delete cln;
    }

    for (auto i : ns) {
        delete i;
    }
    for (auto mn : mns) {
        delete mn;
    }
}

int main() {
    val CLE = val::global("CLElement");
    val doNothing = CLE["doNothing"];
    val destroy_everything_cpp = CLE["destroy_everything"];
    val square = CLE["square"];
    val blackbody_st = CLE["blackbody_st"];

    double *d1 = new double(27.8);
    double *d2 = new double(600.4);

    CLNodeFactory<ClarityNode, double, double> builder("div", "maindiv",
                                                       CppType::NoData);

    ClarityNode *maindiv = builder.build();

    CLNodeFactory<ClarityNode, double, double> childOfMaindivBuilder =
        builder.createChildrenOf(maindiv);

    ModelNode<double> *d1_mn;
    ClarityNode *d1_trinp =
        childOfMaindivBuilder.withStoredValueType(CppType::Double)
            .withName("d1")
            .withStoredValue(d1)
            .extractModelNode<double>(d1_mn)
            .trInput();

    ClarityNode *labelled_d1_trinp =
        childOfMaindivBuilder.labelGivenNode(d1_trinp, "CONST LABEL");

    CLNodeFactory<ClarityNode, int, int> childOfMaindivBuilder_int;
    CLNodeFactory<ClarityNode, int, int>::clone(childOfMaindivBuilder,
                                                childOfMaindivBuilder_int);


    time_t t1 = msecs_time();
    for (int j = 0; j < 50; j++) {
        int *n_input_fields = new int(90);

        for (int i = 0; i < *n_input_fields; i++) {
            int *iptr = new int(i);
            ModelNode<int> *mn = nullptr;
            ClarityNode *cln =
                childOfMaindivBuilder_int.withStoredValueType(CppType::Int)
                    .withName("cln_" + to_string(i))
                    .withStoredValue(iptr)
                    .extractModelNode<int>(mn)
                    .trInput();
            ns.push_back(iptr);
            mns.push_back(mn);
            clns.push_back(cln);
        }
        // destroy_everything();
    }
    time_t t2 = msecs_time();
    //msecs_time();
    time_t del_t =  t2 - t1;
    cout << "Elapsed time: " << del_t;
    
    ClarityNode *statusButton = childOfMaindivBuilder.button(
        "statusButton", "BOOM!", destroy_everything_cpp);

    printf("Setup complete!\n");

    clarity::ClarityNode::callbackMap["destroy_everything"] = [=] {
        destroy_everything();
    };

    return 0;
}
