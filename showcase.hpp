#ifndef showcase_hpp
#define showcase_hpp

#include "CLNodeFactory.hpp"
#include "CanvasElement.hpp"
#include "ClarityNode.hpp"
#include "clarity.hpp"

using namespace clarity;

struct Showcase : public PageContent {
    ClarityNode *content() {
        val CLE = val::global("CLElement");
        val blackbody_st = CLE["blackbody_st"];
        val nodeAudit = CLE["nodeAudit_double"];

        double *a = new double(27.8);
        double *t2 = new double(330);
        double *cir1Radius_value = new double(88.4);

        CLNodeFactory<HybridNode, double, double> builder("div", "maindiv");
        HybridNode<double> *maindiv = builder.build();

        CLNodeFactory<HybridNode, double, double> childOfMaindivBuilder =
            builder.createChildrenOf(maindiv);        

        HybridNode<double> *cir1Radius_tinp = childOfMaindivBuilder.withLinkMultiplierConstant(1)
                                                  .withName("cir1Radius_tinp")
                                                  .withCppVal(cir1Radius_value)
                                                  .textInput();

        cir1Radius_tinp->setCppVal(cir1Radius_value);

        HybridNode<double> *temp_rinp =
            childOfMaindivBuilder.withLinkMultiplierConstant(1).withName("temp_rinp").rangeInput();

        temp_rinp->addPeer(cir1Radius_tinp, 0.5);
        cir1Radius_tinp->refreshDOMValueFromModel();
        cir1Radius_tinp->pushValToPeers(cir1Radius_tinp);

        auto * g1 = childOfMaindivBuilder.withName("tempGroup").group({cir1Radius_tinp, temp_rinp});
        childOfMaindivBuilder.br();

        CLNodeFactory<HybridNode, unsigned char, double> canvasBuilder("div", "canvasDiv");

        CanvasGrid<unsigned char> *canvas1 =
            canvasBuilder.withName("canvas1")
                .withTag("canvas")
                .withAttributes({{"style", val("border: 1px solid green")},
                                 {"width", val(400)},
                                 {"height", val(300)}})
                .canvasGrid(30, 20, 400, 300);

        canvas1->setCurrentCellVal(5);

        // CLNodeFactory<HybridNode, unsigned char, double> canvasParamBuilder(canvasBuilder);
        HybridNode<unsigned char> *canvas1CurrentCellColor_tinp =
            canvasBuilder.withName("currentCellColor_tinp")
                .withCppVal(canvas1->getPtr2CurrentCellVal())
                .withAttributes({{"style", val("border: 3px dashed purple")}, {"size", val(2)}})
                .textInput();

        maindiv->appendChild(canvas1);
        maindiv->appendChild(canvas1CurrentCellColor_tinp);

        CLNodeFactory<HybridNode, string, int> childOfMaindivBuilder_str(childOfMaindivBuilder);

        string *flexLabelText = new string("Flex Text");

        HybridNode<string> *flexLabel = childOfMaindivBuilder_str.withCppVal(flexLabelText)
                                            .withName("flexLabel")
                                            .label(cir1Radius_tinp, *flexLabelText);

        HybridNode<string> *inputFlexTextLabel =
            childOfMaindivBuilder_str.withModelNode(flexLabel).textInput();        

        HybridNode<double> *svgarea =
            childOfMaindivBuilder.withName("svgarea")
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

        HybridNode<double> *cir1Radius = childOfMaindivBuilder.withModelNode(cir1Radius_tinp)
                                               .withName("cir1Radius")
                                               .withLinkMultiplierConstant(1)
                                               .withAttributes({})
                                               .attributeNode("r", cir1);

        HybridNode<double> *temp2_rinp = childOfMaindivBuilder.withLinkMultiplierConstant(1)
                                             .withName("temp2_rinp")
                                             .withCppVal(t2)
                                             .rangeInput(0, 2000);        

        HybridNode<double> *circleFill = childOfMaindivBuilder.withModelNode(temp2_rinp)
                                             .withName("circleFill")
                                             .withTransformFns(blackbody_st, blackbody_st)
                                             //.withAttributes({})
                                             .attributeNode("fill", cir1);

        temp2_rinp->setCppVal(t2);
        temp2_rinp->refreshDOMValueFromModel();
        temp2_rinp->pushValToPeers(temp2_rinp);

        // auto * g1 = childOfMaindivBuilder.withName("svgGroup").group({svgarea, cir1,
        // circleRadius, circleFill});

        string *textarea_val = new string("This is a textarea.");
        HybridNode<string> *textarea1 = childOfMaindivBuilder_str.textarea(textarea_val, 3, 40);
        textarea1->refreshDOMValueFromModel();
        textarea1->pushValToPeers(textarea1);

        printf("Setup complete!\n");
        return maindiv;
    }
};

#endif
