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

    virtual val deleteCurrentElementEL();

    void countElements() { ActiveVector<HybridNode, string, int>::countElements(); }

    void removeChecked() { ActiveVector<HybridNode, string, int>::removeChecked(); }

    virtual HybridNode<string> *makeElementRepresentation(string *s) {
        auto *reprNode = builder_.withName("av_element")
                             .withCppVal(s)
                             .withHoverText(string("Edit this element"))
                             .textInput();
        return reprNode;
    }

    /**
     * @brief Performs an "erase" operation on the underlying vector using `position` to specify the
     * element.
     *
     * @param position
     * @return StorageVectorIteratorT
     */
    StorageVectorIteratorT erase(StorageVectorIteratorT position) {
        return ActiveVector<HybridNode, string, int>::erase(position);
    }

    /**
     * @brief Performs an "erase" operation on the underlying vector at index `n` to specify the
     * element.
     *
     * @param n
     * @return StorageVectorIteratorT
     */
    StorageVectorIteratorT eraseNth(int n) {
        StorageVectorIteratorT erasedIter = ActiveVector<HybridNode, string, int>::eraseNth(n);
        return erasedIter;
    }

    /**
     * @brief We create a 'delete' checkbox for each string. Later, when the user clicks 'Remove
     * Checked', the checked elements are deleted.
     *
     * @param v
     * @return HybridNode<string>*
     */
    virtual HybridNode<string> *makeElementControl(string *v) {
        auto *reprNode = builder_.withName("av_element")
                             .withCppVal(v)
                             .withHoverText(string("Edit this element"))
                             .textInput();

        StorageVectorIteratorT currentFirst = storageVector_.end();
        CLNodeFactory<HybridNode, bool, int> checkboxBuilder(builder_);

        //val deleteCurrentElementEL = deleteCurrentElementEL();

        auto *deleteBox =
            checkboxBuilder.withName("delete_" + clto_str(reprNode->getId())).checkbox();

        auto *grp = builder_.group({reprNode, deleteBox});

        return grp;
    }
};

EMSCRIPTEN_BINDINGS(avstring) {
    // class_<ActiveVector<HybridNode, string, int>>("ActiveVector")
    //     .function("eraseNth", &ActiveVector<HybridNode, string, int>::eraseNth,
    //               allow_raw_pointers())
    //     .function("countElements", &ActiveVector<HybridNode, string, int>::countElements,
    //               allow_raw_pointers())
    //     .function("removeChecked", &ActiveVector<HybridNode, string, int>::removeChecked,
    //               allow_raw_pointers());
    // // .class_function("eraseFrom", &ActiveVector<HybridNode, string, int>::eraseFrom,
    // //                 allow_raw_pointers());

    class_<ActiveVector<HybridNode, string, int>::StorageVectorIteratorT>("StorageVectorIteratorT");

    class_<AVString>("AVString")
        .constructor<HybridNode<string> *>()
        .function("erase", &AVString::erase, allow_raw_pointers())
        .function("eraseNth", &AVString::eraseNth, allow_raw_pointers())
        // .function("deleteCurrentElementEL", &AVString::deleteCurrentElementEL,
        // allow_raw_pointer<arg<0>>())
        .function("deleteCurrentElementEL", &AVString::deleteCurrentElementEL, allow_raw_pointers())
        .function("countElements", &AVString::countElements, allow_raw_pointers())
        .function("removeChecked", &AVString::removeChecked, allow_raw_pointers());
    //.function("eraseFn", &AVString::eraseFn, allow_raw_pointers());
}

val AVString::deleteCurrentElementEL() {
    cout << "AVString::Creating deleter function for index: " << currentIndex_ << endl;
    val deleteCurrentElementEL = val::global("eraseNth")(*this, val(currentIndex_));
    return deleteCurrentElementEL;
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

        val deleteCurrentElementEL = val::global("eraseNth")(avstring, val(0));

        auto *deleteFirstBtn = builder.button("deleteFirstBtn", "Delete First", deleteCurrentElementEL);

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
