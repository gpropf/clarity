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

int main() {
    val utils_instance = val::global("Util").new_();
    val CLE = val::global("CLElement");
    val doNothing = CLE["doNothing"];
    // val canvasDrawFunction = CLE["canvasDrawFunction"];
    // val draw_test_pattern = CLE["draw_test_pattern"];

    CLNodeFactory<ClarityNode, double, double> builder("div", "maindiv",
                                                       CppType::NoData);

    ClarityNode *maindiv = builder.build();

    CLNodeFactory<ClarityNode, double, double> builder_children =
        builder.createChildrenOf(maindiv);

    CLNodeFactory<CanvasElement, string, int> builder_cnvs;
    CLNodeFactory<CanvasElement, string, int>::clone(builder_children,
                                                     builder_cnvs);

    CanvasElement *canvas1 =
        builder_children
            .withName("canvas1")
            // .withTag("canvas")
            .withAttributes({{"width", val(400)}, {"height", val(300)}})
            .canvas2();

    // ClarityNode *statusButton = childOfMaindivBuilder.button(
    //     "canvasButton", "Draw!", canvasDrawFunction);

    printf("Setup complete!\n");

    // val del = canvas1->getCLE()["domElement"];
    // CLE.call<void>("canvasDrawFunction", del);

    //  canvas1_cle.call<void>("canvasDrawFunction");
    // canvasDrawFunction(canvas1_cle);
    // draw_test_pattern();

    return 0;
}
