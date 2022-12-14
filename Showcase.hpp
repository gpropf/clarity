#ifndef showcase_hpp
#define showcase_hpp

#include "CLNodeFactory.hpp"
#include "CanvasElement.hpp"
#include "ClarityNode.hpp"
#include "PageContent.hpp"
#include "clarity.hpp"

using namespace clarity;

/**
 * @brief Used to test all the major types of web controls.
 *
 */
struct Showcase : public PageContent {
    ClarityNode *content(ClarityNode *innerContent = nullptr) {
        ClarityNode::addJSAuxScript("showcaseAux.js");
        ClarityNode::runJSAuxScripts();

        val elgInputAlltypesMouseover = ClarityNode::JSProxyNode_["elgInputAlltypesMouseover"];

        ClarityNode::installListenerGenerators(
            "JSProxyNode", "HybridNode", elgInputAlltypesMouseover, "input", "range", "mouseover");

        val blackbody_st = ClarityNode::JSProxyNode_["blackbody_st"];

        double *temp = new double(330);
        double *cir1Radius_value = new double(88.4);

        CLNodeFactory<HybridNode, double, double> builder("div", "showcase_root");

#ifdef USETF
        auto *maindiv = builder.build();
#else
        auto *maindiv = builder.withAttachmentId("hookid")
                            .withAttachmentMode(clarity::ClarityNode::AttachmentMode::REPLACE_ID)
                            .build();
#endif

        CLNodeFactory<HybridNode, double, double> childOfMaindivBuilder =
            builder.withChildrenOf(maindiv);

        // Shows the use of the stateFunction_ member of HybridNode. Sets the field background to a
        // shade that goes from purple to light green depending on the value entered.
        std::function<void(HybridNode<double> *, double *v)> stateFn = [&](HybridNode<double> *hn,
                                                                           double *v) {
            cout << "stateFunction_: hn->getCppVal = " << *hn->getCppVal() << ", new value = " << *v
                 << endl;
            string colorString = "background-color: rgb(50," + clto_str(int(*v)) + ",50)" + ";";
            hn->nodelog("Trying to set style: " + colorString);
            hn->getDomElement().template call<void>("setAttribute", val("style"), val(colorString));
        };

        auto *cir1Radius_tinp =
            childOfMaindivBuilder.withLinkMultiplierConstant(1)
                .withName("cir1Radius_tinp")
                .withCppVal(cir1Radius_value)
                .withStateFunction(stateFn)
                .withHoverText(
                    "Radius of circle, also demonstrates use of State Functions. Shade of green "
                    "depends on value between 0 and 255.")
                .textInput();

        auto *cir1Radius_rinp = childOfMaindivBuilder.withLinkMultiplierConstant(1)
                                    .withName("cir1Radius_rinp")
                                    .withHoverText("Radius of circle (range input)")
                                    .rangeInput();

        cir1Radius_rinp->addPeer(cir1Radius_tinp, 0.5);

        auto *g1 = childOfMaindivBuilder.withName("cir1Radius_grp")
                       .group({cir1Radius_tinp, cir1Radius_rinp});

        auto g1_lbl = childOfMaindivBuilder.labelGivenNode(g1, "Circle radius");

        CLNodeFactory<HybridNode, string, int> childOfMaindivBuilder_str(childOfMaindivBuilder);
        // CLNodeFactory<HybridNode, void, int> childOfMaindivBuilder_void(childOfMaindivBuilder);

        childOfMaindivBuilder.br();

        auto *svgarea = childOfMaindivBuilder.withName("svgarea")
                            .withTag("svg")
                            .withAttributes({{"width", val("300")},
                                             {"height", val("200")},
                                             {"viewBox", val("0 0 200 200")},
                                             {"style", val("border: 1px solid black")}})
                            .withHoverText("SVG Control")
                            .build();

        auto *cir1 = childOfMaindivBuilder.withName("cir1")
                         .withParent(svgarea)
                         .withTag("circle")
                         .withAttributes({{"r", val("30")},
                                          {"cx", val(100)},
                                          {"cy", val(100)},
                                          {"stroke", val("green")},
                                          {"fill", val("rgb(50,199,77)")},
                                          {"stroke-width", val(4)}})
                         .build();

        auto *cir1Radius = childOfMaindivBuilder.withPeer(cir1Radius_tinp)
                               .withName("cir1Radius")
                               .withLinkMultiplierConstant(1)
                               .withAttributes({})
                               .attributeNode("r", cir1);

        auto *temp_rinp = childOfMaindivBuilder.withLinkMultiplierConstant(1)
                              .withName("temp_rinp")
                              .withCppVal(temp)
                              .withHoverText("Circle 'temperature'")
                              .rangeInput(0, 2000);

        auto *circleFill = childOfMaindivBuilder.withPeer(temp_rinp)
                               .withName("circleFill")
                               .withTransformFns(blackbody_st, blackbody_st)
                               .attributeNode("fill", cir1);
        childOfMaindivBuilder.br();

        string *flexLabelText =
            new string("This is the label for 'svgarea'. You can edit the text below.");
        auto *flexLabel = childOfMaindivBuilder_str.withCppVal(flexLabelText)
                              .withName("flexLabel")
                              .label(svgarea, *flexLabelText);
        childOfMaindivBuilder.br();

        auto *inputFlexTextLabel = childOfMaindivBuilder_str.withName("inputFlexTextLabel")
                                       .withPeer(flexLabel)
                                       .withHoverText("You can change the label value")
                                       .textInput();
        childOfMaindivBuilder.br();

        childOfMaindivBuilder.hr();
        double *d1 = new double(1.2);
        auto *daisyChain1_trinp =
            childOfMaindivBuilder.withHoverText("Actual Stored Value").withCppVal(d1).textInput();

        auto *daisyChain2_trinp = childOfMaindivBuilder.withPeer(daisyChain1_trinp)
                                      .withLinkMultiplierConstant(0.1)
                                      .withHoverText("Value derived from field to the left")
                                      .textInput();
        auto *daisyChain3_trinp = childOfMaindivBuilder.withPeer(daisyChain2_trinp)
                                      .withLinkMultiplierConstant(0.1)
                                      .withHoverText("Value derived from field to the left")
                                      .textInput();

        auto *daisies =
            childOfMaindivBuilder.group({daisyChain1_trinp, daisyChain2_trinp, daisyChain3_trinp});
        auto *daisies_lbl = childOfMaindivBuilder.label(
            daisies,
            "These three nodes are linked in a daisy chain to illustrate intragraph value "
            "transformations and the ability to have nodes that do not themselves contain a model "
            "value. Only the leftmost has a cppVal_",
            true);

        childOfMaindivBuilder.br();
        childOfMaindivBuilder.br();

        double *d2 = new double(5.3);
        double *lrConstant = new double(11.1);
        auto *daisyChain4_trinp =
            childOfMaindivBuilder.withCppVal(d2).withHoverText("Actual Stored Value").textInput();
        auto *daisyChain5_trinp = childOfMaindivBuilder.withCppVal(lrConstant)
                                      .withAttributes({{"disabled", val("disabled")}})
                                      .withHoverText("Modifiable multiplier - not yet implemented")
                                      //.withLinkMultiplierConstant(0.1)
                                      .textInput();
        auto *daisyChain6_trinp =
            childOfMaindivBuilder.withPeer(daisyChain4_trinp)
                .withLinkMultiplierVariable(lrConstant)
                .withHoverText(
                    "Value derived by dividing value in leftmost field by value in middle field.")
                .textInput();

        auto *daisies2 =
            childOfMaindivBuilder.group({daisyChain4_trinp, daisyChain5_trinp, daisyChain6_trinp});
        auto *daisies2_lbl = childOfMaindivBuilder.label(
            daisies2,
            "These nodes are similar to the ones above except that the middle value is the "
            "multiplier used to determine the right side value from the left side one. This "
            "feature is not fully implemented yet and the multiplier value cannot be changed, "
            "hence the field is disabled.",
            true);

        childOfMaindivBuilder.br();
        childOfMaindivBuilder.br();

        // string *clickme = new string("CLICK ME");
        string clickme2("CLICK ME TOO");
        // Demonstrates the withEventListenerGenerator method.
        auto *elgDemo_tinp =
            CLNodeFactory<HybridNode, string, double>(childOfMaindivBuilder)
                .withName("elgDemo_tinp")
                //.withCppVal(canvas1->getPtr2CurrentCellVal())
                .withAttributes({{"style", val("border: 3px solid #4055aa")}, {"size", val(15)}})
                .withEventListenerGenerator("mousedown",
                                            ClarityNode::JSProxyNode_["elgInputAlltypesMousedown"])
                .withInitVal(clickme2)
                .withHoverText("Demonstrates user-provided custom listener generator")
                .textInput();

        childOfMaindivBuilder.br();

        string *textarea_val = new string("This is a textarea.");
        auto *textarea1 =
            childOfMaindivBuilder_str.withName("textarea1").textarea(textarea_val, 3, 40);
        childOfMaindivBuilder.br();

        CLNodeFactory<HybridNode, unsigned char, double> canvasBuilder("div", "canvasDiv");

        CanvasGrid<unsigned char> *canvas1 =
            canvasBuilder.withName("canvas1")
                .withTag("canvas")
                .withAttributes({{"style", val("border: 1px solid green")},
                                 {"width", val(600)},
                                 {"height", val(400)}})
                .withHoverText("CanvasGrid Control")
                .canvasGrid(30, 20, 600, 400);

        canvas1->setCurrentCellVal(5);

        auto *canvas1CurrentCellColor_tinp =
            canvasBuilder.withName("currentCellColor_tinp")
                .withCppVal(canvas1->getPtr2CurrentCellVal())
                //.withAttributes({{"style", val("border: 3px dashed purple")}, {"size", val(2)}})
                .withAttributes({{"class", val("small_width")}})
                .withHoverText("Index of current color for canvas (between 0-7)")
                .textInput();

        maindiv->appendChild(canvas1);
        maindiv->appendChild(canvas1CurrentCellColor_tinp);
        childOfMaindivBuilder.br();
        childOfMaindivBuilder.hr();

        CLNodeFactory<Select, vector<pair<int, string>>, int> selectBuilder(childOfMaindivBuilder);
        vector<pair<int, string>> *carOptions = new vector<pair<int, string>>;
        carOptions->push_back({0, "Chevy"});
        carOptions->push_back({1, "Ford"});
        carOptions->push_back({2, "Toyota"});
        carOptions->push_back({3, "Honda"});

        Select<vector<pair<int, string>>> *carSelect =
            selectBuilder.withName("cars")
                .withCppVal(carOptions)
                .withHoverText("Demonstrates Select subclass")
                .select();

        CLNodeFactory<SimpleSelect, int, int> simpleSelectBuilder(childOfMaindivBuilder);
        vector<pair<int, string>> *carOptions2 = new vector<pair<int, string>>;
        carOptions2->push_back({0, "BMW"});
        carOptions2->push_back({1, "Lambo"});
        carOptions2->push_back({2, "Lada"});
        carOptions2->push_back({3, "Ferrari"});

        int *carSelection = new int(2);

        SimpleSelect<int> *carSelect_ss = simpleSelectBuilder.withName("carSelect_ss")
                                              .withCppVal(carSelection)
                                              .withHoverText("Demonstrates SimpleSelect subclass")
                                              .withLabelText("Pick a better car")
                                              .simpleSelect<string>(*carOptions2);

        CLNodeFactory<HybridNode, bool, int> checkboxBuilder(childOfMaindivBuilder);
        // string * checkme = new string("checked");
        bool *checkme = new bool(false);
        auto *cbTest = checkboxBuilder.withCppVal(checkme)
                           .withName("checkme")
                           .withHoverText("Demonstrates Checkbox subclass")
                           .checkbox();

        auto *lblTest = childOfMaindivBuilder_str.label(cbTest, "Label should engulf CB", true);

        int *cv = new int(0);
        CLNodeFactory<HybridNode, int, int> childOfMaindivBuilder_int(childOfMaindivBuilder);
        auto *cycleButton =
            childOfMaindivBuilder_int.withName("cycleButton").withCppVal(cv).cycleButton("INC", cv);

        printf("Setup complete!\n");
        return maindiv;
    }
};

#endif
