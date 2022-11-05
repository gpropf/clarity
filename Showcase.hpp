#ifndef showcase_hpp
#define showcase_hpp

#include "CLNodeFactory.hpp"
#include "CanvasElement.hpp"
#include "ClarityNode.hpp"
#include "clarity.hpp"

using namespace clarity;

/**
 * @brief Used to test all the major types of web controls.
 *
 */
struct Showcase : public PageContent {
    ClarityNode *content(ClarityNode *innerContent = nullptr) {
        // val CLElement = val::global("CLElement");
        val blackbody_st = ClarityNode::CLElement_["blackbody_st"];

        double *temp = new double(330);
        double *cir1Radius_value = new double(88.4);

        CLNodeFactory<HybridNode, double, double> builder("div", "showcase_root");
        auto *maindiv = builder.build();

        CLNodeFactory<HybridNode, double, double> childOfMaindivBuilder =
            builder.withChildrenOf(maindiv);

        auto *cir1Radius_tinp = childOfMaindivBuilder.withLinkMultiplierConstant(1)
                                    .withName("cir1Radius_tinp")
                                    .withCppVal(cir1Radius_value)
                                    .textInput();

        auto *cir1Radius_rinp = childOfMaindivBuilder.withLinkMultiplierConstant(1)
                                    .withName("cir1Radius_rinp")
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
                                       .textInput();
        childOfMaindivBuilder.br();

        childOfMaindivBuilder.hr();
        double *d1 = new double(1.2);
        auto *daisyChain1_trinp = childOfMaindivBuilder.withCppVal(d1).textInput();
        auto *daisyChain2_trinp = childOfMaindivBuilder.withPeer(daisyChain1_trinp)
                                      .withLinkMultiplierConstant(0.1)
                                      .textInput();
        auto *daisyChain3_trinp = childOfMaindivBuilder.withPeer(daisyChain2_trinp)
                                      .withLinkMultiplierConstant(0.1)
                                      .textInput();

        auto *daisies =
            childOfMaindivBuilder.group({daisyChain1_trinp, daisyChain2_trinp, daisyChain3_trinp});
        auto *daisies_lbl = childOfMaindivBuilder.labelGivenNode(
            daisies,
            "These three nodes are linked in a daisy chain to illustrate intragraph value "
            "transformations and the ability to have nodes that do not themselves contain a model "
            "value. Only the leftmost has a cppVal_");

        childOfMaindivBuilder.br();
        childOfMaindivBuilder.br();

        // Demonstrates the withEventListenerGenerator method.
        auto *elgDemo_tinp =
            CLNodeFactory<HybridNode, string, double>(childOfMaindivBuilder).withName("elgDemo_tinp")
                //.withCppVal(canvas1->getPtr2CurrentCellVal())
                .withAttributes({{"style", val("border: 3px solid #4055aa")}, {"size", val(15)}})
                .withEventListenerGenerator("mousedown", ClarityNode::CLElement_["elgInputAlltypesMousedown"])
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
                .canvasGrid(30, 20, 600, 400);

        canvas1->setCurrentCellVal(5);

        auto *canvas1CurrentCellColor_tinp =
            canvasBuilder.withName("currentCellColor_tinp")
                .withCppVal(canvas1->getPtr2CurrentCellVal())
                .withAttributes({{"style", val("border: 3px dashed purple")}, {"size", val(2)}})
                .textInput();

        maindiv->appendChild(canvas1);
        maindiv->appendChild(canvas1CurrentCellColor_tinp);
        childOfMaindivBuilder.br();
        childOfMaindivBuilder.hr();

        CLNodeFactory<Select, vector<pair<int, string>>, int> selectBuilder(childOfMaindivBuilder);
        vector<pair<int, string>> *carOptions = new vector<pair<int, string>>;
        carOptions->push_back({1, "Chevy"});
        carOptions->push_back({2, "Ford"});
        carOptions->push_back({3, "Toyota"});
        carOptions->push_back({4, "Honda"});

        Select<vector<pair<int, string>>> *carSelect =
            selectBuilder.withName("cars").withCppVal(carOptions).select();

        // carSelect->getCLE().template call<void>("addOptionElementFromString");
        //  carSelect->getCLE().template call<void>("addOptionElementFromString",
        //  val((*carSelect->getCppVal())[0])); carSelect->getCLE().template
        //  call<void>("addOptionElementFromString", val((*carSelect->getCppVal())[1]));

        CLNodeFactory<HybridNode, bool, int> checkboxBuilder(childOfMaindivBuilder);
        // string * checkme = new string("checked");
        bool *checkme = new bool(false);
        checkboxBuilder.withCppVal(checkme).withName("checkme").checkbox();

        printf("Setup complete!\n");
        return maindiv;
    }
};

#endif
