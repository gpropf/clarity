#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "globals.hpp"
#include "testbed.hpp"

template <>
const array<string, 8> CanvasGrid<unsigned char>::colors = {"#F5F5DC", "#00FF00", "#00AA00", "#FF00FF",
                                                            "#AA00AA", "#00AAFF", "#9090AA", "#888888"};

int main() {   

    val CLE = val::global("CLElement");
    val doNothing = CLE["doNothing"];
    val square = CLE["square"];
    val blackbody_st = CLE["blackbody_st"];
    val nodeAudit = CLE["nodeAudit_double"];

    double *a = new double(27.8);
    double *t2 = new double(600);
    double *temp = new double(88.4);

    //ModelNode<double> *amn = new ModelNode<double>(a, "independently_created_modelnode");

    CLNodeFactory<HybridNode, double, double> builder("div", "maindiv");

    HybridNode<double> *maindiv = builder.build();

    CLNodeFactory<HybridNode, double, double> childOfMaindivBuilder = builder.createChildrenOf(maindiv);

    map<string, val> inputFieldAttrs = {{"type", val("text")}};

    CLNodeFactory<HybridNode, double, double> inputBuilder =
        childOfMaindivBuilder.withTag("input").withBoundField("value").withAttributes(inputFieldAttrs);

    HybridNode<double> *hybridTemp_tinp =
        childOfMaindivBuilder.withLinkMultiplierConstant(1).withName("hybridTemp_tinp").withCppVal(temp).textInput();

    hybridTemp_tinp->setCppVal(temp);

    HybridNode<double> *temp_rinp =
        childOfMaindivBuilder.withLinkMultiplierConstant(1).withName("temp_rinp").rangeInput();

    temp_rinp->addPeer(hybridTemp_tinp, 0.5);
    hybridTemp_tinp->refreshDOMValueFromModel();
    hybridTemp_tinp->pushValToPeers(hybridTemp_tinp);

    CLNodeFactory<CanvasGrid, unsigned char, double> canvasBuilder("div", "canvasDiv");

    CanvasGrid<unsigned char> *canvas1 =
        canvasBuilder.withName("canvas1")
            .withTag("canvas")
            .withAttributes({{"style", val("border: 1px solid green")}, {"width", val(400)}, {"height", val(300)}})
            .canvasGrid(30, 20, 400, 300);

    canvas1->setCurrentCellVal(5);

    CLNodeFactory<HybridNode, string, int> childOfMaindivBuilder_str(childOfMaindivBuilder);

    string *flexLabelText = new string("Flex Text");

    HybridNode<string> *flexLabel = childOfMaindivBuilder_str.withCppVal(flexLabelText)
                                        .withName("flexLabel")
                                        .label(hybridTemp_tinp, *flexLabelText);

    HybridNode<string> *inputFlexTextLabel = childOfMaindivBuilder_str.withModelNode(flexLabel).textInput();

    HybridNode<int> *statusButton = (CLNodeFactory<HybridNode, int, int>(childOfMaindivBuilder))
                                        .button("statusButton", "Node Audit", nodeAudit);

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

    HybridNode<double> *temp2_rinp =
        childOfMaindivBuilder.withLinkMultiplierConstant(1).withName("temp2_rinp").withCppVal(t2).rangeInput();

    val blackbody = temp2_rinp->getCLE()["blackbody"];

    HybridNode<double> *circleFill = childOfMaindivBuilder.withModelNode(temp2_rinp)
                                         .withName("CIRCLEFILL")
                                         .withTransformFns(blackbody, blackbody)
                                         .withAttributes({})
                                         .attributeNode("fill", cir1);

    temp2_rinp->setCppVal(t2);
    temp2_rinp->refreshDOMValueFromModel();
    temp2_rinp->pushValToPeers(temp2_rinp);

    string *textarea_val = new string("This is a textarea.");
    HybridNode<string> *textarea1 = childOfMaindivBuilder_str.textarea(textarea_val, 3, 40);
    textarea1->refreshDOMValueFromModel();
    textarea1->pushValToPeers(textarea1);

    printf("Setup complete!\n");

    return 0;
}
