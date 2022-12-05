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

    virtual ClarityNode * makeElementControl(V * v) {
        auto *node = makeElementRepresentation(v);
        CLNodeFactory<HybridNode, bool, int> checkboxBuilder(builder_);
        auto *deleteCheckbox =
            checkboxBuilder.withName("delete_" + clto_str(node->getId())).checkbox();
        auto *grp = builder_.group({node, deleteCheckbox});
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
    // erase(const iterator<pair<V*, ClarityNode*>> first, const iterator<pair<V*, ClarityNode*>>
    // last) {
    //     return storageVector_.erase(first, last);
    // }

   protected:
    CLNodeFactory<Nc, V, N> builder_;
    vector<pair<V*, ClarityNode*>> storageVector_;
};

#endif