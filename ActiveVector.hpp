#ifndef ActiveVector_hpp
#define ActiveVector_hpp

#include <iostream>
#include <vector>

#include "CLNodeFactory.hpp"

template <template <typename V> class Nc, typename V, typename N>
class ActiveVector {
   public:
    typedef typename vector<pair<V*, ClarityNode*>>::iterator storageVectorIterator;
    // ActiveVector() {}
    ActiveVector(ClarityNode* rootNode) { builder_ = builder_.withChildrenOf(rootNode); }

    virtual val deleteLastFn() { return val::null(); }

    virtual ClarityNode* makeElementControl(V* v) {
        // val deleteLastFn =

        auto* reprNode = makeElementRepresentation(v);
        storageVectorIterator currentLast = storageVector_.end();
        // std::function<void(val ev)> deleteLastFn = [this,&currentLast](val ev) {
        // this->erase(currentLast); }; val deleteLastFn =
        // val::global("Util").call<val>("callMethodByName", this, val("erase"));
        // val deleteLastFn = this->deleteLastFn();
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

    virtual ClarityNode* makeElementRepresentation(V*) { return nullptr; };  // FIXME!
    virtual void push_back(V* v) {
        ClarityNode* node = makeElementControl(v);
        storageVector_.push_back(pair(v, node));
    }

    storageVectorIterator erase(storageVectorIterator position) {
        return storageVector_.erase(position);
    }

    storageVectorIterator eraseNth(int n) {
        storageVectorIterator nIter = storageVector_.begin() + n;
        return storageVector_.erase(nIter);
    }

    val eraseFn(storageVectorIterator pos) {
        return val([this, pos]() { this->erase(pos); });
    }

    static void eraseFrom(ActiveVector<Nc, V, N> v, storageVectorIterator i) { v.erase(i); }
    // erase(const iterator<pair<V*, ClarityNode*>> first, const iterator<pair<V*, ClarityNode*>>
    // last) {
    //     return storageVector_.erase(first, last);
    // }

   //protected:
    CLNodeFactory<Nc, V, N> builder_;
    vector<pair<V*, ClarityNode*>> storageVector_;
};

#endif