#include "testbed.hpp"

#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"

map<const int, ControlNetworkNode *> ControlNetworkNode::switchboard;
map<string, std::function<void()>> ControlNetworkNode::callbackMap;
TicketMachine ControlNetworkNode::tm;
val ControlNetworkNode::ActiveLink::CLElement_ = val::global("CLElement");

int main() {
    using ActiveLink = ControlNetworkNode::ActiveLink;

    // double *n = new double(50);
    // double *pi = new double(3.14159);
    double *a = new double(2.78);

    // CLNodeFactory<ControlNetworkNode, double> builder("div", "maindiv",
    //                                                   CppType::NoData, a);

    // ControlNetworkNode *maindiv =
    //     new ControlNetworkNode("maindiv", "div", CppType::NoData);

    CLNodeFactory<double> builder("div", "maindiv",
                                                      CppType::NoData);

    ControlNetworkNode *maindiv = builder.build();

    CLNodeFactory<double> childOfMaindivBuilder =
        builder.createChildrenOf(maindiv);

    // WebElemNode *input1 = builder.buildWithModelNode();
    //  WebElemNode *input2 = builder.withStoredValue(n).buildWithModelNode();
    //  WebElemNode *button1 = builder.button("button1", "Press me!");
    //  WebElemNode *labelInput1 = builder.label(input1, "PI");
    //  button1->addEventListenerByName("click", "iterateModel");

    map<string, val> inputFieldAttrs = {{"type", val("text")}};
    ControlNetworkNode *input_a = childOfMaindivBuilder.withStoredValue(a)
                                      .withStoredValueType(CppType::Double)
                                      .withName("input_a_text")
                                      .withTag("input")
                                      .withAttributes(inputFieldAttrs)
                                      .build();

    ControlNetworkNode *svgarea =
        childOfMaindivBuilder.withName("svgarea")
            .withTag("svg")
            .withAttributes({{"width", val("300")},
                             {"height", val("200")},
                             {"viewBox", val("0 0 200 200")},
                             {"style", val("border: 1px solid black")}})
            .build();

    ControlNetworkNode *cir1 =
        childOfMaindivBuilder.withName("cir1")
            .withParent(svgarea)
            .withTag("circle")
            .withAttributes({{"r", val("30")},
                             {"cx", val(100)},
                             {"cy", val(100)},
                             {"stroke", val("green")},
                             {"fill", val("rgb(50,199,77)")},
                             {"stroke-width", val(4)}})
            .build();

    // ControlNetworkNode *cir1Radius = childOfMaindivBuilder.withName("cir1Radius")
    //         .withParent(cir1).withBoundField("r").build();

    //         cir1->setAttribute();
    // cir1->setAttribute();
    // cir1->setAttribute();
    // cir1->setAttribute("r", val(80));

    svgarea->appendChild(cir1);

    //     new clarity::WebElemNode("svgarea", "svg", clarity::CppType::NoData);
    // clarity::WebElemNode *cir1 =
    //     new clarity::WebElemNode("cir1", "circle", clarity::CppType::Double);
    // clarity::WebAttrNode *cir1Radius =
    //     new clarity::WebAttrNode("r", clarity::CppType::Double, cir1);

    // ControlNetworkNode *input_a =
    //     new ControlNetworkNode("input_a_text", "input", CppType::Double);

    // ModelNode<double> *mn = new ModelNode(a, CppType::Double);
    // mn->addPeer(ActiveLink(input_a, 1));
    // mn->pushValToPeers(mn);

    // cout << "CPPType for input_a is: " << (int)input_a->getStoredValueType()
    //      << "\n";

    // WebElemNode *labelled_input_a =
    //     builder.labelGivenNode(input_a, "Label for A");

    // maindiv->appendChild(input1);
    //  maindiv->appendChild(labelInput1);
    //  maindiv->appendChild(input2);
    //  maindiv->appendChild(button1);
    // maindiv->appendChild(svgarea);
    // maindiv->appendChild(input_a);

    // maindiv->appendChild(labelled_input_a);
    // input1->setAttribute("type", val("text"));
    // input2->setAttribute("type", val("text"));

    // WebElemNode::callbackMap["iterateModel"] = [=] {
    //     cout << "callbackMap[\"iterateModel\"]\n";
    // };

    printf("Setup complete!\n");
    return 0;
}
