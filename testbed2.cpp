#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "testbed.hpp"

map<const int, ClarityNode *> ClarityNode::switchboard;
map<string, std::function<void()>> ClarityNode::callbackMap;
TicketMachine ClarityNode::tm;
val ClarityNode::DualLink::CLElement_ = val::global("CLElement");

template <>
const array<string, 8> CanvasGrid<unsigned char>::colors = {"#F5F5DC", "#00FF00", "#00AA00", "#FF00FF",
                                                            "#AA00AA", "#00AAFF", "#9090AA", "#888888"};

// double *testmem(double *dptr) {
//     dptr = new double(37);
//     return dptr;
// }

void testmem(double *&dptr) {
    dptr = new double(37);
    // return dptr;
}

int main() {
    // using ActiveLink = ClarityNode::ActiveLink;

    val CLE = val::global("CLElement");
    val doNothing = CLE["doNothing"];
    val square = CLE["square"];
    val blackbody_st = CLE["blackbody_st"];
    val nodeAudit = CLE["nodeAudit_double"];

    double *a = new double(27.8);
    double *t2 = new double(600);

    double *temp = new double(88.4);
    // ModelNode<double> *a_mn =
    ModelNode<double> *amn = new ModelNode<double>(a, "independently_created_modelnode");

    // val blackbody_st = a_mn->getCLE()["blackbody_st"];

    CLNodeFactory<HybridNode<double>, double, double> builder("div", "maindiv", CppType::NoData);

    HybridNode<double> *maindiv = builder.build();

    CLNodeFactory<HybridNode<double>, double, double> childOfMaindivBuilder = builder.createChildrenOf(maindiv);

    // CLNodeFactory<CanvasGrid<unsigned char>, unsigned char, double>
    // canvasBuilder("div", "canvasDiv",
    //                                                    CppType::NoData);

    // CanvasGrid<unsigned char> *canvas1 =
    //     canvasBuilder.withName("canvas1")
    //         .withTag("canvas")
    //         .withAttributes({{"style", val("border: 1px solid green")},
    //                          {"width", val(400)},
    //                          {"height", val(300)}})
    //         .canvasGrid(30, 20, 400, 300);
    //         //.canvas();

    map<string, val> inputFieldAttrs = {{"type", val("text")}};

    CLNodeFactory<HybridNode<double>, double, double> inputBuilder =
        childOfMaindivBuilder.withTag("input").withBoundField("value").withAttributes(inputFieldAttrs);

    // ClarityNode *input_a =
    //     inputBuilder.withName("input_a_text").withModelNode(a_mn).build();

    double *ival;
    // ival = testmem(ival);
    testmem(ival);
    cout << "Value created in CLNF is: " << *ival << "\n";

    // ModelNode<double> *temp_mn2;
    // HybridNode<double> *input_temp =
    //     childOfMaindivBuilder.withStoredValueType(CppType::Double)
    //         .withName("input_temp_text")
    //         .withStoredValue(temp)
    //         //.withCppVal(temp)
    //         .extractModelNode<double>(temp_mn2)
    //         .textInput();

    // HybridNode<double>  *input_temp_tr =
    //     childOfMaindivBuilder.withStoredValueType(CppType::Double)
    //         .withLinkMultiplierConstant(10)
    //         .withName("input_temp_range")
    //         .withModelNode(temp_mn2)
    //       //  .withCppVal(temp)
    //         .trInput();

    HybridNode<double> *hybridTemp_tinp =
        childOfMaindivBuilder.withLinkMultiplierConstant(1).withName("hybridTemp_tinp").withCppVal(temp).textInput();

    hybridTemp_tinp->setCppVal(temp);

    HybridNode<double> *temp_rinp = childOfMaindivBuilder.withLinkMultiplierConstant(1)
                                        .withName("temp_rinp")
                                        //  .withModelNode(temp_mn2)
                                        .rangeInput();

    temp_rinp->addPeer(hybridTemp_tinp, 0.5);
    // temp_rinp->pushValToPeers(temp_rinp);
    // hybridTemp_tinp->addPeer(temp_rinp, 1);
    hybridTemp_tinp->refreshDOMValueFromModel();
    hybridTemp_tinp->pushValToPeers(hybridTemp_tinp);

    CLNodeFactory<CanvasGrid<unsigned char>, unsigned char, double> canvasBuilder("div", "canvasDiv", CppType::NoData);

    CanvasGrid<unsigned char> *canvas1 =
        canvasBuilder.withName("canvas1")
            .withTag("canvas")
            .withAttributes({{"style", val("border: 1px solid green")}, {"width", val(400)}, {"height", val(300)}})
            .canvasGrid(30, 20, 400, 300);

    CLNodeFactory<HybridNode<string>, string, int> childOfMaindivBuilder_str(childOfMaindivBuilder);

    string *flexLabelText = new string("Flex Text");

    HybridNode<string> *flexLabel = childOfMaindivBuilder_str.withCppVal(flexLabelText)
                                        .withName("flexLabel")
                                        .label(hybridTemp_tinp, *flexLabelText);

    val passthru = flexLabel->getCLE()["passthru"];

    HybridNode<string> *inputFlexTextLabel = childOfMaindivBuilder_str.withModelNode(flexLabel).textInput();

    HybridNode<int> *statusButton = (CLNodeFactory<HybridNode<int>, int, int>(childOfMaindivBuilder))
                                        .button("statusButton", "Node Audit", nodeAudit);

    // HybridNode<double> *a_tinp =
    //     childOfMaindivBuilder.withLinkMultiplierConstant(1)
    //         .withName("a_tinp")
    //         .withCppVal(a)
    //         .textInput();

    HybridNode<double> *svgarea = childOfMaindivBuilder.withName("svgarea")
                                      .withTag("svg")
                                      .withAttributes({{"width", val("300")},
                                                       {"height", val("200")},
                                                       {"viewBox", val("0 0 200 200")},
                                                       {"style", val("border: 1px solid black")}})
                                      .build();

    HybridNode<double> *cir1 = childOfMaindivBuilder.withName("cir1")
                                   .withParent(svgarea)
                                   .withTag("circle")
                                   .withAttributes({{"r", val("30")},
                                                    {"cx", val(100)},
                                                    {"cy", val(100)},
                                                    {"stroke", val("green")},
                                                    {"fill", val("rgb(50,199,77)")},
                                                    {"stroke-width", val(4)}})
                                   .build();

    HybridNode<double> *circleRadius = childOfMaindivBuilder.withModelNode(hybridTemp_tinp)
                                           .withName("RADIUS")
                                           .withLinkMultiplierConstant(1)
                                           .withAttributes({})
                                           .attributeNode("r", cir1);

    HybridNode<double> *temp2_rinp = childOfMaindivBuilder.withLinkMultiplierConstant(1)
                                         .withName("temp2_rinp")
                                         .withCppVal(t2)
                                         //  .withModelNode(temp_mn2)
                                         .rangeInput();

    val blackbody = temp2_rinp->getCLE()["blackbody"];

    HybridNode<double> *circleFill = childOfMaindivBuilder
                                         .withModelNode(temp2_rinp)
                                         //.withStoredValueType(CppType::String)
                                         .withName("CIRCLEFILL")
                                         .withTransformFns(blackbody, blackbody)
                                         .withAttributes({})
                                         .attributeNode("fill", cir1);

    temp2_rinp->setCppVal(t2);
    temp2_rinp->refreshDOMValueFromModel();
    temp2_rinp->pushValToPeers(temp2_rinp);

    printf("Setup complete!\n");

    return 0;
}
