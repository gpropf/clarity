#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "testbed.hpp"

map<const int, ClarityNode *> ClarityNode::switchboard;
map<string, std::function<void()>> ClarityNode::callbackMap;
TicketMachine ClarityNode::tm;
val ClarityNode::DualLink::CLElement_ = val::global("CLElement");

int main() {
    val CLE = val::global("CLElement");
    val doNothing = CLE["doNothing"];
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

    int *n_input_fields = new int(10);

    vector<int *> ns;
    vector<ModelNode<int> *> mns;
    vector<ClarityNode *> clns;
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

    // string *flexLabelText = new string("Flex Text");

    // ModelNode<string> *flexLabel_mn;
    // ClarityNode *flexLabel =
    //     childOfMaindivBuilder_str.withStoredValueType(CppType::String)
    //         .withStoredValue(flexLabelText)
    //         .extractModelNode<string>(flexLabel_mn)
    //         .label(input_temp, *flexLabelText);

    // val passthru = flexLabel_mn->getCLE()["passthru"];

    // ClarityNode *inputFlexTextLabel =
    //     childOfMaindivBuilder_str.withModelNode(flexLabel_mn)
    //         .withStoredValueType(CppType::String)
    //         //.withTransformFns(passthru, passthru)
    //         .textInput();

    // //<class Nc_in, class Nc_out, typename V_in, typename V_out>

    // ClarityNode *input_temp_tr =
    //     childOfMaindivBuilder.withStoredValueType(CppType::Double)
    //         .withLinkMultiplierConstant(10)
    //         .withName("input_temp_range")
    //         .withModelNode(temp_mn2)
    //         .trInput();

    // ClarityNode *svgarea =
    //     childOfMaindivBuilder.withName("svgarea")
    //         .withTag("svg")
    //         .withAttributes({{"width", val("300")},
    //                          {"height", val("200")},
    //                          {"viewBox", val("0 0 200 200")},
    //                          {"style", val("border: 1px solid black")}})
    //         .build();

    // ClarityNode *statusButton =
    //     childOfMaindivBuilder.button("statusButton", "Print Status",
    //     doNothing);

    // ClarityNode *cir1 = childOfMaindivBuilder.withName("cir1")
    //                         .withParent(svgarea)
    //                         .withTag("circle")
    //                         .withAttributes({{"r", val("30")},
    //                                          {"cx", val(100)},
    //                                          {"cy", val(100)},
    //                                          {"stroke", val("green")},
    //                                          {"fill", val("rgb(50,199,77)")},
    //                                          {"stroke-width", val(4)}})
    //                         .build();

    // ClarityNode *circleRadius = childOfMaindivBuilder.withModelNode(a_mn)
    //                                 .withName("RADIUS")
    //                                 .withLinkMultiplierConstant(1)
    //                                 .withAttributes({})
    //                                 .attributeNode("r", cir1);

    // val blackbody = a_mn->getCLE()["blackbody"];

    // ClarityNode *circleFill = childOfMaindivBuilder.withModelNode(temp_mn2)
    //                               .withStoredValueType(CppType::String)
    //                               .withName("CIRCLEFILL")
    //                               .withTransformFns(blackbody, blackbody)
    //                               .withAttributes({})
    //                               .attributeNode("fill", cir1);

    // ClarityNode *range_a = inputBuilder.withModelNode(a_mn)
    //                            .withName("range_a")
    //                            .withAttributes({{"type", val("range")}})
    //                            .build();

    // ClarityNode *range_a_lbld =
    //     childOfMaindivBuilder.labelGivenNode(input_temp_tr, "Temperature");

    // ClarityNode::callbackMap["printStats"] = [=] {
    //     cout << "callbackMap[\"iterateModel\"]\n";
    // };

    // cout << "FILLCOLOR\n\n";
    // val fillColor = circleFill->getVal();
    // circleFill->getCLE().set("originalColor", fillColor);

    printf("Setup complete!\n");

    return 0;
}
