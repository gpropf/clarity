#include "testbed.hpp"

#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"

map<const int, ClarityNodeBase *> ClarityNodeBase::switchboard;
map<string, std::function<void()>> ClarityNodeBase::callbackMap;
TicketMachine ClarityNodeBase::tm_;
//val ClarityNode::ActiveLink::CLElement_ = val::global("CLElement");
val ClarityNodeBase::DualLink::CLElement_ = val::global("CLElement");

// double *testmem(double *dptr) {
//     dptr = new double(37);
//     return dptr;
// }

void testmem(double *&dptr) {
    dptr = new double(37);
    // return dptr;
}

int main() {
    //using ActiveLink = ClarityNode::ActiveLink;

    val CLE = val::global("CLElement");
    val doNothing = CLE["doNothing"];
    val square = CLE["square"];
    val blackbody_st = CLE["blackbody_st"];

    double *a = new double(27.8);

    int dims[2] = {1,0};

    Datum<double> a_dtm(CppType::Double, a, dims);

    double *temp = new double(600.4);
    ModelNode<double> *a_mn =
        new ModelNode(a, CppType::Double, "independently_created_modelnode");

    // val blackbody_st = a_mn->getCLE()["blackbody_st"];

    CLNodeFactory<ClarityNodeBase, double, double> builder("div", "maindiv",
                                                       CppType::NoData);

    ClarityNodeBase *maindiv = builder.build();
    maindiv->datum_ = a_dtm;

    CLNodeFactory<ClarityNodeBase, double, double> childOfMaindivBuilder =
        builder.createChildrenOf(maindiv);

    ClarityNodeBase *canvas1 =
        childOfMaindivBuilder.withName("canvas1")
            .withTag("canvas")
            .withAttributes({{"width", val(400)}, {"height", val(300)}})
            .build();

    map<string, val> inputFieldAttrs = {{"type", val("text")}};

    CLNodeFactory<ClarityNodeBase, double, double> inputBuilder =
        childOfMaindivBuilder.withStoredValueType(CppType::Double)
            .withTag("input")
            .withBoundField("value")
            .withAttributes(inputFieldAttrs);

    ClarityNodeBase *input_a =
        inputBuilder.withName("input_a_text").withModelNode(a_mn).build();

    double *ival;
    // ival = testmem(ival);
    testmem(ival);
    cout << "Value created in CLNF is: " << *ival << "\n";

    ModelNode<double> *temp_mn2;
    ClarityNodeBase *input_temp =
        childOfMaindivBuilder.withStoredValueType(CppType::Double)
            .withName("input_temp_text")
            .withStoredValue(temp)
            .extractModelNode<double>(temp_mn2)
            .textInput();

    CLNodeFactory<ClarityNodeBase, string, int> childOfMaindivBuilder_str;
    CLNodeFactory<ClarityNodeBase, string, int>::clone(childOfMaindivBuilder,
                                                   childOfMaindivBuilder_str);

    string *flexLabelText = new string("Flex Text");

    ModelNode<string> *flexLabel_mn;
    ClarityNodeBase *flexLabel =
        childOfMaindivBuilder_str.withStoredValueType(CppType::String)
            .withStoredValue(flexLabelText)
            .extractModelNode<string>(flexLabel_mn)
            .label(input_temp, *flexLabelText);

    val passthru = flexLabel_mn->getCLE()["passthru"];

    ClarityNodeBase *inputFlexTextLabel =
        childOfMaindivBuilder_str.withModelNode(flexLabel_mn)
            .withStoredValueType(CppType::String)
            //.withTransformFns(passthru, passthru)
            .textInput();

    //<class Nc_in, class Nc_out, typename V_in, typename V_out>

    ClarityNodeBase *input_temp_tr =
        childOfMaindivBuilder.withStoredValueType(CppType::Double)
            .withLinkMultiplierConstant(10)
            .withName("input_temp_range")
            .withModelNode(temp_mn2)
            .trInput();

    ClarityNodeBase *svgarea =
        childOfMaindivBuilder.withName("svgarea")
            .withTag("svg")
            .withAttributes({{"width", val("300")},
                             {"height", val("200")},
                             {"viewBox", val("0 0 200 200")},
                             {"style", val("border: 1px solid black")}})
            .build();

    ClarityNodeBase *statusButton =
        childOfMaindivBuilder.button("statusButton", "Print Status", doNothing);

    ClarityNodeBase *cir1 = childOfMaindivBuilder.withName("cir1")
                            .withParent(svgarea)
                            .withTag("circle")
                            .withAttributes({{"r", val("30")},
                                             {"cx", val(100)},
                                             {"cy", val(100)},
                                             {"stroke", val("green")},
                                             {"fill", val("rgb(50,199,77)")},
                                             {"stroke-width", val(4)}})
                            .build();

    ClarityNodeBase *circleRadius = childOfMaindivBuilder.withModelNode(a_mn)
                                    .withName("RADIUS")
                                    .withLinkMultiplierConstant(1)
                                    .withAttributes({})
                                    .attributeNode("r", cir1);

    val blackbody = a_mn->getCLE()["blackbody"];

    ClarityNodeBase *circleFill = childOfMaindivBuilder.withModelNode(temp_mn2)
                                  .withStoredValueType(CppType::String)
                                  .withName("CIRCLEFILL")
                                  .withTransformFns(blackbody, blackbody)
                                  .withAttributes({})
                                  .attributeNode("fill", cir1);

    ClarityNodeBase *range_a = inputBuilder.withModelNode(a_mn)
                               .withName("range_a")
                               .withAttributes({{"type", val("range")}})
                               .build();

    ClarityNodeBase *range_a_lbld =
        childOfMaindivBuilder.labelGivenNode(input_temp_tr, "Temperature");

    ClarityNodeBase::callbackMap["printStats"] = [=] {
        cout << "callbackMap[\"iterateModel\"]\n";
    };

    cout << "FILLCOLOR\n\n";
    val fillColor = circleFill->getVal();
    circleFill->getCLE().set("originalColor", fillColor);

    printf("Setup complete!\n");

    return 0;
}
