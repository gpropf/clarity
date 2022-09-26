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
val ClarityNode::DualLink::CLElement_ = val::global("CLElement");

int main() {
    val utils_instance = val::global("Util").new_();
    val CLE = val::global("CLElement");
    val doNothing = CLE["doNothing"];
    val blackbody_st = CLE["blackbody_st"];

    double *d1 = new double(84.4);
    double *d2 = new double(600.4);

    int dims[2] = {1, 0};
    Datum<double> *d1_dtm = new Datum<double>(CppType::Double, d1, dims);
    Datum<double> *d2_dtm = new Datum<double>(CppType::Double, d2, dims);

    CLNodeFactory<double> builder("div", "maindiv");

    ClarityNode *maindiv = builder.build();

    CLNodeFactory<double> childOfMaindivBuilder =
        builder.createChildrenOf(maindiv);

    ClarityNode *d1_inp =
        childOfMaindivBuilder.withDatum(d1_dtm).withName("d1").textInput();

    TranslatorInput<double> *d1_tr =
        new TranslatorInput<double>(d1_dtm, d1_inp->getDomElement());

    ClarityNode *d1_rinp =
        childOfMaindivBuilder.withName("range_d1").rangeInput();

    d1_inp->addPeer<double>(d1_rinp, 2.0);

    // ClarityNode *d2_inp =
    //     childOfMaindivBuilder.withDatum(d2_dtm).withName("d2").trInput();

    // TranslatorInput<double> *d2_tr =
    //     new TranslatorInput<double>(d2_dtm, d2_inp->getDomElement());

    d1_inp->setTranslator(d1_tr);
   // d2_inp->setTranslator(d2_tr);
    d1_tr->datum2js();
    d1_inp->pushValToPeers(d1_inp);
 //   d2_tr->datum2js();

   // d2_inp->pushValToPeers(d2_inp);
    // ClarityNode *labelled_d1_trinp =
    //     childOfMaindivBuilder.labelGivenNode(d1_trinp, "CONST LABEL");

    return 0;
}
