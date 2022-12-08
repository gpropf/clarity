#ifndef showcase_hpp
#define showcase_hpp

#include "ActiveVector.hpp"
#include "CLNodeFactory.hpp"
#include "CanvasElement.hpp"
#include "ClarityNode.hpp"
#include "PageContent.hpp"
#include "clarity.hpp"

namespace clarity {
// using namespace clarity;

// class AVString;

class AVString : public ActiveVector<HybridNode, string, int> {
   public:
    using ActiveVector::ActivePairT;

    AVString(HybridNode<string> *rootNode) : ActiveVector(rootNode) {}

    virtual val deleteFirstFn();

    void countElements() {
        // ActiveVector<HybridNode, string, int>::removeChecked();
        ActiveVector<HybridNode, string, int>::countElements();
    }

    void removeChecked() {
        ActiveVector<HybridNode, string, int>::removeChecked();
    }

    HybridNode<string> *makeElementRepresentation(string *s) {
        auto *reprNode = builder_.withName("av_element")
                             .withCppVal(s)
                             .withHoverText(string("Edit this element"))
                             .textInput();
        return reprNode;
    }

    StorageVectorIteratorT erase(StorageVectorIteratorT position) {
        return ActiveVector<HybridNode, string, int>::erase(position);
    }

    StorageVectorIteratorT eraseNth(int n) {
        // StorageVectorIteratorT nIter = storageVector_.begin() + n;
        // StorageVectorIteratorT erasedIter = this->erase(nIter);
        // this->countElements(100);
        StorageVectorIteratorT erasedIter = ActiveVector<HybridNode, string, int>::eraseNth(n);

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
                  allow_raw_pointers())
        .function("removeChecked", &ActiveVector<HybridNode, string, int>::removeChecked,
                  allow_raw_pointers());
    // .class_function("eraseFrom", &ActiveVector<HybridNode, string, int>::eraseFrom,
    //                 allow_raw_pointers());

    class_<ActiveVector<HybridNode, string, int>::StorageVectorIteratorT>("StorageVectorIteratorT");

    class_<AVString>("AVString")
        .constructor<HybridNode<string> *>()
        .function("erase", &AVString::erase, allow_raw_pointers())
        .function("eraseNth", &AVString::eraseNth, allow_raw_pointers())
        // .function("deleteFirstFn", &AVString::deleteFirstFn, allow_raw_pointer<arg<0>>())
        .function("deleteFirstFn", &AVString::deleteFirstFn, allow_raw_pointers())
        .function("countElements", &AVString::countElements, allow_raw_pointers())
        .function("removeChecked", &AVString::removeChecked, allow_raw_pointers());
    //.function("eraseFn", &AVString::eraseFn, allow_raw_pointers());
}

val AVString::deleteFirstFn() {
    cout << "AVString::Creating deleter function for index: " << currentIndex_ << endl;
    val deleteFirstEL = val::global("eraseNth")(*this, val(currentIndex_));
    return deleteFirstEL;
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

        CLNodeFactory<HybridNode, string, int> builder("div", "showcase_root");
        auto *maindiv = builder.build();

        builder = builder.withChildrenOf(maindiv);

        auto *avstringsDiv = builder.withTag("div").withName("AVStringsDiv").build();
        AVString *avstring = new AVString(avstringsDiv);
        string *s1 = new string("Jan");
        avstring->push_back(s1);
        string *s2 = new string("Tyler");
        avstring->push_back(s2);
        string *s3 = new string("Greg");
        avstring->push_back(s3);
        string *s4 = new string("Abby");
        avstring->push_back(s4);

        val countElementsELG = val::global("countElementsELG")(avstring);

        auto *countElementsBtn =
            builder.button("countElementsBtn", "Count Elements", countElementsELG);

        val removeChecked = val::global("removeChecked")(avstring);

        auto *removeCheckedBtn =
            builder.button("removeCheckedBtn", "Remove Checked", removeChecked);

        val deleteFirstEL = val::global("eraseNth")(avstring, val(0));

        auto *deleteFirstBtn = builder.button("deleteFirstBtn", "Delete First", deleteFirstEL);

        auto ff = [](pair<string *, HybridNode<string> *> p) {
            if (*p.first == "Greg") return true;
            return false;
        };

        auto greg = avstring->find(ff);

        cout << "Greg node id = " << (greg.second)->getId() << endl;

        printf("Setup complete!\n");
        return maindiv;
    }
};

}  // namespace clarity
#endif
