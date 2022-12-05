#ifndef ActiveVector_hpp
#define ActiveVector_hpp

#include <iostream>
#include <vector>

#include "CLNodeFactory.hpp"

template <template <typename V> class Nc, typename V, typename N>
class ActiveVector {
    typedef typename vector<pair<V*, ClarityNode*>>::iterator storageVectorIterator;

   public:
    // ActiveVector() {}
    ActiveVector(ClarityNode* rootNode) { builder_ = builder_.withChildrenOf(rootNode); }

    virtual val deleteLastFn() = 0;

    virtual ClarityNode* makeElementControl(V* v) {
        //val deleteLastFn =
            

        auto* reprNode = makeElementRepresentation(v);
        storageVectorIterator currentLast = storageVector_.end();
        // std::function<void(val ev)> deleteLastFn = [&](val ev) { this->erase(currentLast); };
        // val deleteEL = val::null();
        CLNodeFactory<HybridNode, bool, int> checkboxBuilder(builder_);
        // auto* deleteCheckbox =
        //     checkboxBuilder.withName("delete_" + clto_str(reprNode->getId())).checkbox();
        auto* deleteButton = builder_.withAttributes({{"class", val("buttonDelete")}})
                                 .button("delete_btn_" + clto_str(reprNode->getId()), "X");
        auto* grp = builder_.group({reprNode, deleteButton});
        // deleteEL = ClarityNode::JSProxyNode_["makeDeleteNodeFn"](grp->getId());
       // deleteButton->addEventListener(deleteLastFn, "click");
        return grp;
    }

    virtual ClarityNode* makeElementRepresentation(V*) = 0;
    virtual void push_back(V* v) {
        ClarityNode* node = makeElementControl(v);
        storageVector_.push_back(pair(v, node));
    }

    storageVectorIterator erase(storageVectorIterator position) {
        return storageVector_.erase(position);
    }

    // static void eraseFrom(decltype (this) v, storageVectorIterator i) {
    //     v.erase(i);
    // }
    // erase(const iterator<pair<V*, ClarityNode*>> first, const iterator<pair<V*, ClarityNode*>>
    // last) {
    //     return storageVector_.erase(first, last);
    // }

   protected:
    CLNodeFactory<Nc, V, N> builder_;
    vector<pair<V*, ClarityNode*>> storageVector_;
};

#endif