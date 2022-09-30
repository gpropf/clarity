#include "testbed.hpp"

#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"

map<const int, ClarityNode *> ClarityNode::switchboard;
map<string, std::function<void()>> ClarityNode::callbackMap;
TicketMachine ClarityNode::tm;
// val ClarityNode::ActiveLink::CLElement_ = val::global("CLElement");
val ClarityNode::DualLink::CLElement_ = val::global("CLElement");
const array<string, 8> CanvasGrid::colors = {
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

    double *temp = new double(600.4);
    ModelNode<double> *a_mn =
        new ModelNode(a, CppType::Double, "independently_created_modelnode");

    // val blackbody_st = a_mn->getCLE()["blackbody_st"];

    CLNodeFactory<ClarityNode, double, double> builder("div", "maindiv",
                                                       CppType::NoData);

    ClarityNode *maindiv = builder.build();

    CLNodeFactory<ClarityNode, double, double> childOfMaindivBuilder =
        builder.createChildrenOf(maindiv);

    CanvasGrid *canvas1 =
        childOfMaindivBuilder.withName("canvas1")
            .withTag("canvas")
            .withAttributes({{"style", val("border: 1px solid green")},
                             {"width", val(400)},
                             {"height", val(300)}})
            .canvasGrid(30, 20, 400, 300);
            //.canvas();

    map<string, val> inputFieldAttrs = {{"type", val("text")}};

    CLNodeFactory<ClarityNode, double, double> inputBuilder =
        childOfMaindivBuilder.withStoredValueType(CppType::Double)
            .withTag("input")
            .withBoundField("value")
            .withAttributes(inputFieldAttrs);

    ClarityNode *input_a =
        inputBuilder.withName("input_a_text").withModelNode(a_mn).build();

    double *ival;
    // ival = testmem(ival);
    testmem(ival);
    cout << "Value created in CLNF is: " << *ival << "\n";

    ModelNode<double> *temp_mn2;
    ClarityNode *input_temp =
        childOfMaindivBuilder.withStoredValueType(CppType::Double)
            .withName("input_temp_text")
            .withStoredValue(temp)
            .extractModelNode<double>(temp_mn2)
            .textInput();

    CLNodeFactory<ClarityNode, string, int> childOfMaindivBuilder_str;
    CLNodeFactory<ClarityNode, string, int>::clone(childOfMaindivBuilder,
                                                   childOfMaindivBuilder_str);

    string *flexLabelText = new string("Flex Text");

    ModelNode<string> *flexLabel_mn;
    ClarityNode *flexLabel =
        childOfMaindivBuilder_str.withStoredValueType(CppType::String)
            .withStoredValue(flexLabelText)
            .extractModelNode<string>(flexLabel_mn)
            .label(input_temp, *flexLabelText);

    val passthru = flexLabel_mn->getCLE()["passthru"];

    ClarityNode *inputFlexTextLabel =
        childOfMaindivBuilder_str.withModelNode(flexLabel_mn)
            .withStoredValueType(CppType::String)
            //.withTransformFns(passthru, passthru)
            .textInput();

    //<class Nc_in, class Nc_out, typename V_in, typename V_out>

    ClarityNode *input_temp_tr =
        childOfMaindivBuilder.withStoredValueType(CppType::Double)
            .withLinkMultiplierConstant(10)
            .withName("input_temp_range")
            .withModelNode(temp_mn2)
            .trInput();

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

    val blackbody = a_mn->getCLE()["blackbody"];

    ClarityNode *circleFill = childOfMaindivBuilder.withModelNode(temp_mn2)
                                  .withStoredValueType(CppType::String)
                                  .withName("CIRCLEFILL")
                                  .withTransformFns(blackbody, blackbody)
                                  .withAttributes({})
                                  .attributeNode("fill", cir1);

    ClarityNode *range_a = inputBuilder.withModelNode(a_mn)
                               .withName("range_a")
                               .withAttributes({{"type", val("range")}})
                               .build();

    ClarityNode *range_a_lbld =
        childOfMaindivBuilder.labelGivenNode(input_temp_tr, "Temperature");

    ClarityNode::callbackMap["printStats"] = [=] {
        cout << "callbackMap[\"iterateModel\"]\n";
    };

    cout << "FILLCOLOR\n\n";
    val fillColor = circleFill->getVal();
    circleFill->getCLE().set("originalColor", fillColor);

    printf("Setup complete!\n");

    return 0;
}
