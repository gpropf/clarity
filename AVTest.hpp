#ifndef showcase_hpp
#define showcase_hpp

#include "ActiveVector.hpp"
#include "CLNodeFactory.hpp"
#include "CanvasElement.hpp"
#include "ClarityNode.hpp"
#include "PageContent.hpp"
#include "clarity.hpp"

using namespace clarity;

// class AVString;

class AVString : public ActiveVector<HybridNode, string, int> {
   public:
    AVString(ClarityNode *rootNode) : ActiveVector(rootNode) {}

    virtual val deleteLastFn();

    void countElements() { ActiveVector<HybridNode, string, int>::countElements(); }

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
        // storageVectorIterator nIter = storageVector_.begin() + n;
        // storageVectorIterator erasedIter = this->erase(nIter);
        // this->countElements(100);
        storageVectorIterator erasedIter = ActiveVector<HybridNode, string, int>::eraseNth(n);

        return erasedIter;
    }

    // virtual void countElements() {
    //     cout << "This vector has " << this->storageVector_.size() << " elements" << endl;
    //     cout << "this pointer = " << this << endl;
    //     for (auto [element, node] : this->storageVector_) {
    //         cout << "ELEM: " << *element << " : " << node->getId() << endl;
    //     }
    // }
};

EMSCRIPTEN_BINDINGS(avstring) {
    class_<ActiveVector<HybridNode, string, int>>("ActiveVector")
        .function("eraseNth", &ActiveVector<HybridNode, string, int>::eraseNth,
                  allow_raw_pointers())
        .function("countElements", &ActiveVector<HybridNode, string, int>::countElements,
                  allow_raw_pointers());
    // .class_function("eraseFrom", &ActiveVector<HybridNode, string, int>::eraseFrom,
    //                 allow_raw_pointers());

    class_<ActiveVector<HybridNode, string, int>::storageVectorIterator>("storageVectorIterator");

    class_<AVString>("AVString")
        .constructor<ClarityNode *>()
        .function("erase", &AVString::erase, allow_raw_pointers())
        .function("eraseNth", &AVString::eraseNth, allow_raw_pointers())
        // .function("deleteLastFn", &AVString::deleteLastFn, allow_raw_pointer<arg<0>>())
        .function("deleteLastFn", &AVString::deleteLastFn, allow_raw_pointers())
        .function("countElements", &AVString::countElements, allow_raw_pointers());
    //.function("eraseFn", &AVString::eraseFn, allow_raw_pointers());
}

val AVString::deleteLastFn() {
    cout << "AVString::Creating deleter function for index: " << currentIndex_ << endl;
    val deleteLastEL = val::global("eraseNth")(*this, val(currentIndex_));
    return deleteLastEL;
    // return val::null();
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

        CLNodeFactory<HybridNode, double, double> builder("div", "showcase_root");
        auto *maindiv = builder.build();

        CLNodeFactory<HybridNode, double, double> childOfMaindivBuilder =
            builder.withChildrenOf(maindiv);

        auto *avstringsDiv = childOfMaindivBuilder.withTag("div").withName("AVStringsDiv").build();
        AVString *avstring = new AVString(static_cast<ClarityNode *>(avstringsDiv));
        string *s1 = new string("FOO_String");
        avstring->push_back(s1);
        string *s2 = new string("BOO_String");
        avstring->push_back(s2);
        string *s3 = new string("Greg");
        avstring->push_back(s3);
        string *s4 = new string("Abby");
        avstring->push_back(s4);

        val countElementsELG = val::global("countElementsELG")(avstring);
        auto *countElementsBtn =
            childOfMaindivBuilder.button("countElementsBtn", "Count Elements", countElementsELG);

        val deleteLastEL = val::global("eraseNth")(avstring, val(0));

        auto *deleteLastBtn =
            childOfMaindivBuilder.button("deleteLastBtn", "Delete Last", deleteLastEL);

        printf("Setup complete!\n");
        return maindiv;
    }
};

#endif
