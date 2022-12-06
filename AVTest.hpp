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
        // return val::null();
        storageVectorIterator currentLast = storageVector_.end();
        return val([this, currentLast]() { this->erase(currentLast); });
        // return val::global("Module")["AVString"].call<void>("eraseLast");
    }

    ClarityNode *makeElementRepresentation(string *s) {
        auto *reprNode = builder_.withName("av_element")
                             .withCppVal(s)
                             .withHoverText(string("Edit this element"))
                             .textInput();
        return reprNode;
    }

    storageVectorIterator erase(storageVectorIterator position) {
        return ActiveVector<HybridNode, string, int>::erase(position);
    }

    storageVectorIterator eraseNth(int n) {
        storageVectorIterator nIter = storageVector_.begin() + n;
        storageVectorIterator erasedIter = this->erase(nIter);
        this->countElements(100);
        return erasedIter;
    }

    void countElements(int lastSize) {
        cout << "This vector has " << this->storageVector_.size() << " elements" << endl;
        cout << "Last size was " << lastSize << " elements" << endl;
    }
};

EMSCRIPTEN_BINDINGS(avstring) {
    class_<ActiveVector<HybridNode, string, int>>("ActiveVector")
        .class_function("eraseFrom", &ActiveVector<HybridNode, string, int>::eraseFrom,
                        allow_raw_pointers());

    class_<ActiveVector<HybridNode, string, int>::storageVectorIterator>("storageVectorIterator");

    class_<AVString>("AVString")
        .function("erase", &AVString::erase, allow_raw_pointers())
        .function("eraseNth", &AVString::eraseNth, allow_raw_pointers())
        .function("deleteLastFn", &AVString::deleteLastFn, allow_raw_pointers())
        .function("countElements", &AVString::countElements, allow_raw_pointers())
        .function("eraseFn", &AVString::eraseFn, allow_raw_pointers());
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

        // val ActiveVectorCtx = val::global("Module.AVString.countElements");
        //  ActiveVector<HybridNode, string, int>::storageVectorIterator currentLast =
        //      avstring.storageVector_.end();
        //  ActiveVectorCtx(val(avstring),val(currentLast));

        val countElementsELG = val::global("countElementsELG")(avstring, val(0));
        auto *countElementsBtn =
            childOfMaindivBuilder.button("countElementsBtn", "Count Elements", countElementsELG);

        // ActiveVector<HybridNode, string, int>::storageVectorIterator currentLast =
        //     avstring.storageVector_.begin();

        val deleteLastEL = val::global("eraseNth")(avstring, val(0));

        auto *deleteLastBtn =
            childOfMaindivBuilder.button("deleteLastBtn", "Delete Last", deleteLastEL);

        // val deleteLastFn = val::global("Util")["callMethodByName"](avstring,
        // val("countElements"));
        //  val deleteLastFn = ActiveVectorCtx["eraseFrom"];
        printf("Setup complete!\n");
        return maindiv;
    }
};

#endif
