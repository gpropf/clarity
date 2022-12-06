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

    void countElements() {
        cout << "This vector has " << this->storageVector_.size() << " elements" << endl;
        cout << "this pointer = " << this << endl;
        for (auto [element, node] : this->storageVector_) {
            cout << "ELEM: " << *element << " : " << node->getId() << endl;
        }
    }

    virtual val deleteLastFn() {
        cout << "ActiveVector::Creating deleter function for index: " << currentIndex_ << endl;
        val deleteLastEL = val::global("eraseNth")(*this, val(currentIndex_));
        return deleteLastEL;
    }

    storageVectorIterator find(std::function<bool(pair<V*,ClarityNode*>)> findFunction) {

    }

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

        // val deleteLastEL = val::global("eraseNth")(val(this), val(0));
        val deleteLastEL = deleteLastFn();
        // val deleteLastEL = val::null();

        auto* deleteButton =
            builder_.withAttributes({{"class", val("buttonDelete")}})
                .button("delete_btn_" + clto_str(reprNode->getId()), "X", deleteLastEL);
        auto* grp = builder_.group({reprNode, deleteButton});
        // deleteEL = ClarityNode::JSProxyNode_["makeDeleteNodeFn"](grp->getId());
        // deleteButton->addEventListener(deleteLastFn, "click");
        return grp;
    }

    virtual ClarityNode* makeElementRepresentation(V*) { return nullptr; };  // FIXME!
    virtual void push_back(V* v) {
        ClarityNode* node = makeElementControl(v);

        storageVector_.push_back(pair(v, node));
        currentIndex_++;
    }

    storageVectorIterator erase(storageVectorIterator position) {
        return storageVector_.erase(position);
    }

    storageVectorIterator eraseNth(int n) {
        countElements();
        storageVectorIterator nIter = storageVector_.begin() + n;
        auto [element, node] = storageVector_[n];
        delete element;
        delete node;
        return storageVector_.erase(nIter);
    }

    // val eraseFn(storageVectorIterator pos) {
    //     return val([this, pos]() { this->erase(pos); });
    // }

    //static void eraseFrom(ActiveVector<Nc, V, N> v, storageVectorIterator i) { v.erase(i); }
    // erase(const iterator<pair<V*, ClarityNode*>> first, const iterator<pair<V*, ClarityNode*>>
    // last) {
    //     return storageVector_.erase(first, last);
    // }

    // protected:
    CLNodeFactory<Nc, V, N> builder_;
    vector<pair<V*, ClarityNode*>> storageVector_;
    int currentIndex_ = 0;
};


#endif