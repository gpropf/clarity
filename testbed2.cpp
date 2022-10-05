#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "testbed.hpp"

map<const int, ClarityNode *> ClarityNode::switchboard;
map<string, std::function<void()>> ClarityNode::callbackMap;
TicketMachine ClarityNode::tm;
// val ClarityNode::ActiveLink::CLElement_ = val::global("CLElement");
val ClarityNode::DualLink::CLElement_ = val::global("CLElement");

template <>
const array<string, 8> CanvasGrid<unsigned char>::colors = {
    "#F5F5DC", "#00FF00", "#00AA00", "#FF00FF",
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

    double *a = new double(27.8);

    double *temp = new double(88.4);
    ModelNode<double> *a_mn =
        new ModelNode(a, "independently_created_modelnode");

    // val blackbody_st = a_mn->getCLE()["blackbody_st"];

    CLNodeFactory<HybridNode<double>, double, double> builder("div", "maindiv",
                                                              CppType::NoData);

    HybridNode<double> *maindiv = builder.build();

    CLNodeFactory<HybridNode<double>, double, double> childOfMaindivBuilder =
        builder.createChildrenOf(maindiv);

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
        childOfMaindivBuilder.withTag("input")
            .withBoundField("value")
            .withAttributes(inputFieldAttrs);

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
        childOfMaindivBuilder.withLinkMultiplierConstant(1)
            .withName("hybridTemp_tinp")
            .withCppVal(temp)
            .textInput();

    hybridTemp_tinp->setCppVal(temp);

    HybridNode<double> *temp_rinp =
        childOfMaindivBuilder.withLinkMultiplierConstant(1)
            .withName("temp_rinp")
            //  .withModelNode(temp_mn2)
            .rangeInput();

    temp_rinp->addPeer(hybridTemp_tinp, 1);
    // temp_rinp->pushValToPeers(temp_rinp);
    // hybridTemp_tinp->addPeer(temp_rinp, 1);
    hybridTemp_tinp->refreshDOMValueFromModel();
    hybridTemp_tinp->pushValToPeers(hybridTemp_tinp);

    CLNodeFactory<CanvasGrid<unsigned char>, unsigned char, double>
        canvasBuilder("div", "canvasDiv", CppType::NoData);

    CanvasGrid<unsigned char> *canvas1 =
        canvasBuilder.withName("canvas1")
            .withTag("canvas")
            .withAttributes({{"style", val("border: 1px solid green")},
                             {"width", val(400)},
                             {"height", val(300)}})
            .canvasGrid(30, 20, 400, 300);
    //.canvas();

    CLNodeFactory<HybridNode<string>, string, int> childOfMaindivBuilder_str(
        childOfMaindivBuilder);

    string *flexLabelText = new string("Flex Text");

    HybridNode<string> *flexLabel =
        childOfMaindivBuilder_str
            .withCppVal(flexLabelText)
            // .extractModelNode<string>(flexLabel_mn)
            .label(hybridTemp_tinp, *flexLabelText);

    val passthru = flexLabel->getCLE()["passthru"];

    HybridNode<string> *inputFlexTextLabel =
        childOfMaindivBuilder_str
            .withModelNode(flexLabel)

            //.withTransformFns(passthru, passthru)
            .textInput();

    ClarityNode *statusButton =
        childOfMaindivBuilder.button("statusButton", "Print Status", doNothing);

    printf("Setup complete!\n");

    return 0;
}
