#ifndef ActiveVector_hpp
#define ActiveVector_hpp

#include <iostream>
#include <vector>

#include "CLNodeFactory.hpp"

template <template <typename V> class Nc, typename V, typename N>
class ActiveVector {
   public:
    typedef typename vector<pair<V*, HybridNode<V>*>>::iterator storageVectorIterator;
    // ActiveVector() {}
    ActiveVector(HybridNode<V>* rootNode) { builder_ = builder_.withChildrenOf(rootNode); }

    void countElements() {
        cout << "This vector has " << this->storageVector_.size() << " elements" << endl;
        cout << "this pointer = " << this << endl;
        for (auto [element, node] : this->storageVector_) {
            cout << "ELEM: " << *element << " : " << node->getId() << endl;
        }
    }

    virtual val deleteFirstFn() {
        cout << "ActiveVector::Creating deleter function for index: " << currentIndex_ << endl;
        val deleteFirstEL = val::global("eraseNth")(*this, val(currentIndex_));
        return deleteFirstEL;
    }

    pair<V*, HybridNode<V>*> find(std::function<bool(pair<V*, HybridNode<V>*>)> findFunction) {
        int i=0;
        for (storageVectorIterator it = storageVector_.begin(); it != storageVector_.end(); it++, i++) {
            if (findFunction(storageVector_[i])) return storageVector_[i];
        }
        return pair(nullptr, nullptr);
    }

    virtual HybridNode<V>* makeElementControl(V* v) {
        // val deleteFirstFn =

        auto* reprNode = makeElementRepresentation(v);
        storageVectorIterator currentFirst = storageVector_.end();
        // std::function<void(val ev)> deleteFirstFn = [this,&currentFirst](val ev) {
        // this->erase(currentFirst); }; val deleteFirstFn =
        // val::global("Util").call<val>("callMethodByName", this, val("erase"));
        // val deleteFirstFn = this->deleteFirstFn();
        CLNodeFactory<HybridNode, bool, int> checkboxBuilder(builder_);

        // auto* deleteCheckbox =
        //     checkboxBuilder.withName("delete_" + clto_str(reprNode->getId())).checkbox();

        // val deleteFirstEL = val::global("eraseNth")(val(this), val(0));
        val deleteFirstEL = deleteFirstFn();
        // val deleteFirstEL = val::null();

        auto* deleteButton =
            builder_.withAttributes({{"class", val("buttonDelete")}})
                .button("delete_btn_" + clto_str(reprNode->getId()), "X", deleteFirstEL);
        auto* grp = builder_.group({reprNode, deleteButton});
        // deleteEL = HybridNode<V>::JSProxyNode_["makeDeleteNodeFn"](grp->getId());
        // deleteButton->addEventListener(deleteFirstFn, "click");
        return grp;
    }

    virtual HybridNode<V>* makeElementRepresentation(V*) { return nullptr; };  // FIXME!
    virtual void push_back(V* v) {
        HybridNode<V>* node = makeElementControl(v);

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

    // static void eraseFrom(ActiveVector<Nc, V, N> v, storageVectorIterator i) { v.erase(i); }
    //  erase(const iterator<pair<V*, HybridNode<V>*>> first, const iterator<pair<V*,
    //  HybridNode<V>*>> last) {
    //      return storageVector_.erase(first, last);
    //  }

    // protected:
    CLNodeFactory<Nc, V, N> builder_;
    vector<pair<V*, HybridNode<V>*>> storageVector_;
    int currentIndex_ = 0;
};

#endif