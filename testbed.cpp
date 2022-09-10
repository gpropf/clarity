#include "testbed.hpp"

#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"

map<const int, ClarityNode *> ClarityNode::switchboard;
map<string, std::function<void()>> ClarityNode::callbackMap;
TicketMachine ClarityNode::tm;
val ClarityNode::ActiveLink::CLElement_ = val::global("CLElement");
val ClarityNode::DualLink::CLElement_ = val::global("CLElement");

int main() {
    using ActiveLink = ClarityNode::ActiveLink;

    val CLE = val::global("CLElement");
    val doNothing = CLE["doNothing"];
    val square = CLE["square"];

    double *a = new double(27.8);
    ModelNode<double> *a_mn =
        new ModelNode(a, CppType::Double, "independently_created_modelnode");

    val blackbody_st = a_mn->getCLE()["blackbody_st"];

    CLNodeFactory<ClarityNode, double> builder("div", "maindiv",
                                               CppType::NoData);

    ClarityNode *maindiv = builder.build();

    CLNodeFactory<ClarityNode, double> childOfMaindivBuilder =
        builder.createChildrenOf(maindiv);

    map<string, val> inputFieldAttrs = {{"type", val("text")}};

    CLNodeFactory<ClarityNode, double> inputBuilder =
        childOfMaindivBuilder.withStoredValueType(CppType::Double)
            .withTag("input")
            .withBoundField("value")
            .withAttributes(inputFieldAttrs);

    ClarityNode *input_a =
        inputBuilder.withName("input_a_text").withModelNode(a_mn).build();

    ClarityNode *svgarea =
        childOfMaindivBuilder.withName("svgarea")
            .withTag("svg")
            .withAttributes({{"width", val("300")},
                             {"height", val("200")},
                             {"viewBox", val("0 0 200 200")},
                             {"style", val("border: 1px solid black")}})
            .build();

    ClarityNode *statusButton =
        childOfMaindivBuilder.button("statusButton", "Print Status", doNothing);

    ClarityNode *cir1 = childOfMaindivBuilder.withName("cir1")
                            .withParent(svgarea)
                            .withTag("circle")
                            .withAttributes({{"r", val("30")},
                                             {"cx", val(100)},
                                             {"cy", val(100)},
                                             {"stroke", val("green")},
                                             {"fill", val("rgb(50,199,77)")},
                                             {"stroke-width", val(4)}})
                            .build();

    ClarityNode *circleRadius = childOfMaindivBuilder.withModelNode(a_mn)
                                    .withName("RADIUS")
                                    .withLinkMultiplierConstant(1)
                                    .withAttributes({})
                                    .attributeNode("r", cir1);

    ClarityNode *circleFill = childOfMaindivBuilder.withModelNode(a_mn)
                                  .withName("CIRCLEFILL")
                                  .withTransformFns(blackbody_st, blackbody_st)
                                  .withAttributes({})
                                  .attributeNode("fill", cir1);

    ClarityNode *range_a = inputBuilder.withModelNode(a_mn)
                               .withName("range_a")
                               .withAttributes({{"type", val("range")}})
                               .build();

    ClarityNode::callbackMap["printStats"] = [=] {
        cout << "callbackMap[\"iterateModel\"]\n";
    };

    printf("Setup complete!\n");

    return 0;
}
