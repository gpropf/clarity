#include <sys/time.h>
#include <unistd.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"
#include "speedtest.hpp"

map<const int, ClarityNode *> ClarityNode::switchboard;
map<string, std::function<void()>> ClarityNode::callbackMap;
TicketMachine ClarityNode::tm_;
template <>
const array<string, 8> TranslatorCanvasGrid<unsigned char>::colors = {
    "#F5F5DC", "#00FF00", "#00AA00", "#FF00FF",
    "#AA00AA", "#00AAFF", "#9090AA", "#888888"};
// static val clarity::CLElement_ = val::global("CLElement");

int main() {
    val utils_instance = val::global("Util").new_();
    val CLE = val::global("CLElement");
    val doNothing = CLE["doNothing"];
    val blackbody_st = CLE["blackbody_st"];

    double *junkval = new double(0);
    double *d1 = new double(54.4);
    double *d2 = new double(600.4);

    const int dims[2] = {1, 0};
    Datum<double> *d1_dtm = new Datum<double>(CppType::Double, d1, dims);
    // Datum<double> *d2_dtm = new Datum<double>(CppType::Double, d2, dims);

    CLNodeFactory<ClarityNode, double, double> builder("div", "maindiv");

    ClarityNode *maindiv = builder.build();

    CLNodeFactory<ClarityNode, double,double> childOfMaindivBuilder =
        builder.createChildrenOf(maindiv);

    ClarityNode *d1_tinp = childOfMaindivBuilder.withDatum(d1_dtm)
                               .withName("d1_textinput")
                               .textInput();

    TranslatorInput<double> *d1_tr_tinp =
        new TranslatorInput<double>(d1_dtm, d1_tinp->getDomElement());

    ClarityNode *d1_rinp =
        childOfMaindivBuilder.withName("d1_rangeinput").rangeInput();

    Datum<double> *d1_blank_dtm =
        new Datum<double>(CppType::Double, junkval, dims);

    d1_tinp->addPeer<double>(d1_rinp, 2.0);
    TranslatorInput<double> *d1_tr_rinp =
        new TranslatorInput<double>(d1_blank_dtm, d1_rinp->getDomElement());

    // ClarityNode *d2_inp =
    //     childOfMaindivBuilder.withDatum(d2_dtm).withName("d2").trInput();

    // TranslatorInput<double> *d2_tr =
    //     new TranslatorInput<double>(d2_dtm, d2_inp->getDomElement());

    d1_tinp->setTranslator(d1_tr_tinp);
    d1_rinp->setTranslator(d1_tr_rinp);
    // d2_inp->setTranslator(d2_tr);

    //   d2_tr->datum2js();

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

    ClarityNode *circleRadius = childOfMaindivBuilder.withName("RADIUS")
                                    .withLinkMultiplierConstant(1)
                                    .withAttributes({})
                                    .attributeNode("r", cir1);

    d1_tinp->addPeer<double>(circleRadius, 1.0);

    // Canvas Grid Test ----------------------------------------------

    int width = 30;
    int height = 20;
    int totalCels = width * height;
    unsigned char *raster = new unsigned char[totalCels];
    const int rasterDims[3] = {width, height, 0};
    Datum<unsigned char> *raster_dtm =
        new Datum<unsigned char>(CppType::Int, raster, rasterDims);
    for (int i = 0; i < totalCels; i++) raster[i] = 0;

    ClarityNode *canvas1 =
        childOfMaindivBuilder.withName("canvas1")
            .withAttributes({{"width", val(400)},
                             {"height", val(300)},
                             {"style", val("border: 2px solid blue")}})
            .canvas();

    TranslatorCanvasGrid<unsigned char> *canvas1_tr =
        new TranslatorCanvasGrid<unsigned char>(raster_dtm,
                                                canvas1->getDomElement());

    canvas1_tr->setCurrentCellVal(3);
    canvas1_tr->datum2js();

    d1_tr_tinp->datum2js();
    d1_tinp->pushValToPeers(d1_tinp);

    // ----------------- Stuff from the old testbed.cpp program
    // -----------------

    // Datum<string> flexText_dtm =
    //     new Datum(CppType::String, new string("Flex Text"), dims);
    
    // double *input_temp = new double(320);

    // ClarityNode *temp_tinp = childOfMaindivBuilder.withName("temp").textInput();

    // ClarityNode *flexLabel =
    //     childOfMaindivBuilder.label(temp_tinp, "Temperature");

    // ClarityNode *inputFlexTextLabel =
    //     childOfMaindivBuilder.withDatum(flexText_dtm)
    //         //.withTransformFns(passthru, passthru)
    //         .textInput();

    return 0;
}
