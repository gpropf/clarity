#ifndef showcase_hpp
#define showcase_hpp

#include "CLNodeFactory.hpp"
#include "CanvasElement.hpp"
#include "ClarityNode.hpp"
#include "clarity.hpp"

using namespace clarity;

struct Showcase : public PageContent {
    ClarityNode *content() {
        val CLElement = val::global("CLElement");
        val blackbody_st = CLElement["blackbody_st"];

        double *temp = new double(330);
        double *cir1Radius_value = new double(88.4);

        CLNodeFactory<HybridNode, double, double> builder("div", "maindiv");
        HybridNode<double> *maindiv = builder.build();

        CLNodeFactory<HybridNode, double, double> childOfMaindivBuilder =
            builder.createChildrenOf(maindiv);

        HybridNode<double> *cir1Radius_tinp = childOfMaindivBuilder.withLinkMultiplierConstant(1)
                                                  .withName("cir1Radius_tinp")
                                                  .withCppVal(cir1Radius_value)
                                                  .textInput();

        HybridNode<double> *cir1Radius_rinp =
            childOfMaindivBuilder.withLinkMultiplierConstant(1).withName("temp_rinp").rangeInput();

        cir1Radius_rinp->addPeer(cir1Radius_tinp, 0.5);

        auto *g1 = childOfMaindivBuilder.withName("cir1Radius_grp")
                       .group({cir1Radius_tinp, cir1Radius_rinp});

        CLNodeFactory<HybridNode, string, int> childOfMaindivBuilder_str(childOfMaindivBuilder);

        string *flexLabelText = new string("Edit this label text");

        HybridNode<string> *flexLabel = childOfMaindivBuilder_str.withCppVal(flexLabelText)
                                            .withName("flexLabel")
                                            .label(cir1Radius_tinp, *flexLabelText);

        childOfMaindivBuilder.br();

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

        HybridNode<double> *temp_rinp = childOfMaindivBuilder.withLinkMultiplierConstant(1)
                                            .withName("temp_rinp")
                                            .withCppVal(temp)
                                            .rangeInput(0, 2000);

        HybridNode<double> *circleFill = childOfMaindivBuilder.withModelNode(temp_rinp)
                                             .withName("circleFill")
                                             .withTransformFns(blackbody_st, blackbody_st)
                                             .attributeNode("fill", cir1);
        childOfMaindivBuilder.br();

        string *textarea_val = new string("This is a textarea.");
        HybridNode<string> *textarea1 = childOfMaindivBuilder_str.textarea(textarea_val, 3, 40);
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

        HybridNode<unsigned char> *canvas1CurrentCellColor_tinp =
            canvasBuilder.withName("currentCellColor_tinp")
                .withCppVal(canvas1->getPtr2CurrentCellVal())
                .withAttributes({{"style", val("border: 3px dashed purple")}, {"size", val(2)}})
                .textInput();

        maindiv->appendChild(canvas1);
        maindiv->appendChild(canvas1CurrentCellColor_tinp);
        childOfMaindivBuilder.br();

        HybridNode<string> *inputFlexTextLabel =
            childOfMaindivBuilder_str.withModelNode(flexLabel).textInput();

        printf("Setup complete!\n");
        return maindiv;
    }
    ClarityNode *content(ClarityNode *innerContent) { return content(); }
};

#endif
