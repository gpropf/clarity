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

    ClarityNode *map(string *s) {
        ClarityNode *node = builder_.withName(*s).withCppVal(s).textInput();
        CLNodeFactory<HybridNode, bool, int> checkboxBuilder(builder_);
        ClarityNode *deleteCheckbox =
            checkboxBuilder.withName("delete_" + clto_str(node->getId())).checkbox();
        ClarityNode *grp = builder_.group({node, deleteCheckbox});
        return grp;
    }
};

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

        // Shows the use of the stateFunction_ member of HybridNode. Sets the field background to a
        // shade that goes from purple to light green depending on the value entered.
        std::function<void(HybridNode<double> *, double *v)> stateFn = [&](HybridNode<double> *hn,
                                                                           double *v) {
            cout << "stateFunction_: v = " << *hn->getCppVal() << endl;
            string colorString =
                "background-color: rgb(200," + clto_str(int(*hn->getCppVal())) + ",50)" + ";";
            hn->nodelog("Trying to set style: " + colorString);
            hn->getDomElement().template call<void>("setAttribute", val("style"), val(colorString));
        };

        auto *cir1Radius_tinp = childOfMaindivBuilder.withLinkMultiplierConstant(1)
                                    .withName("cir1Radius_tinp")
                                    .withCppVal(cir1Radius_value)
                                    .withStateFunction(stateFn)
                                    .textInput();

        // ClarityNode * clroot = &*maindiv;
        auto *avstringsDiv = childOfMaindivBuilder.withTag("div").withName("AVStringsDiv").build();
        AVString avstring(static_cast<ClarityNode *>(avstringsDiv));
        string *s = new string("FOO_String");
        avstring.push_back(s);

        printf("Setup complete!\n");
        return maindiv;
    }
};

#endif
