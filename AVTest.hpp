#ifndef showcase_hpp
#define showcase_hpp

#include "ActiveVector.hpp"
#include "CLNodeFactory.hpp"
#include "CanvasElement.hpp"
#include "ClarityNode.hpp"
#include "PageContent.hpp"
#include "clarity.hpp"

using namespace clarity;

class AVString : public ActiveVector<HybridNode, string, int> {
   public:
    AVString(ClarityNode *rootNode) : ActiveVector(rootNode) {}

    virtual val deleteLastFn() {
        return val::null();
        //return val::global("Module")["AVString"].call<void>("eraseLast");
    }

    ClarityNode *makeElementRepresentation(string *s) {
        auto *reprNode = builder_.withName("av_element")
                             .withCppVal(s)
                             .withHoverText(string("Edit this element"))
                             .textInput();
        return reprNode;
    }
};

EMSCRIPTEN_BINDINGS(AVString) {
    class_<AVString>("AVString")
        .function("erase", &AVString::erase, allow_raw_pointers());
       // .function("eraseLast", &AVString::eraseLast, allow_raw_pointers());
}

/**
 * @brief Testbed for ActiveVector class
 *
 */
struct AVTest : public PageContent {
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
        auto *maindiv = builder.build();

        CLNodeFactory<HybridNode, double, double> childOfMaindivBuilder =
            builder.withChildrenOf(maindiv);

        auto *cir1Radius_tinp = childOfMaindivBuilder.withLinkMultiplierConstant(1)
                                    .withName("cir1Radius_tinp")
                                    .withCppVal(cir1Radius_value)
                                    .textInput();

        // ClarityNode * clroot = &*maindiv;
        auto *avstringsDiv = childOfMaindivBuilder.withTag("div").withName("AVStringsDiv").build();
        AVString avstring(static_cast<ClarityNode *>(avstringsDiv));
        string *s = new string("FOO_String");
        avstring.push_back(s);
        string *s2 = new string("BOO_String");
        avstring.push_back(s2);

        printf("Setup complete!\n");
        return maindiv;
    }
};

#endif
