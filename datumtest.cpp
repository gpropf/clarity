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

    double *d1 = new double(53.7);
    double *d2 = new double(600.4);

    int dims[2] = {1, 0};
    Datum<double> *d1_dtm = new Datum<double>(CppType::Double, d1, dims);

    CLNodeFactory<double> builder("div", "maindiv");

    ClarityNode *maindiv = builder.build();

    CLNodeFactory<double> childOfMaindivBuilder =
        builder.createChildrenOf(maindiv);

    ClarityNode *d1_inp =
        childOfMaindivBuilder.withDatum(d1_dtm).withName("d1").textInput();

    TranslatorInput<double> *d1_tr =
        new TranslatorInput<double>(d1_dtm, d1_inp->getDomElement());

    d1_inp->setTranslator(d1_tr);
    d1_tr->datum2js();

    // ClarityNode *labelled_d1_trinp =
    //     childOfMaindivBuilder.labelGivenNode(d1_trinp, "CONST LABEL");

    return 0;
}
