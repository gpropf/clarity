#ifndef ActiveVector_hpp
#define ActiveVector_hpp

#include <iostream>
#include <vector>

#include "CLNodeFactory.hpp"

/**
 * @brief An experiment in creating a sort of "active mapping" container. The basic idea is that we
 * maintain a GUI representation of each element in a collection that grows or shrinks automatically
 * as the source collection does. So if an element is added, a new GUI element appears and if a
 * source element is deleted, the representation of it also vanishes. This is essentially a
 * classical Functor as thought of in category theory but without the notion of mapping the "arrows"
 * that exist amongst the source category. The intended applications are pretty broad. One thing I'm
 * hoping to do here is automatically manage the <option> tags that exist in <select> dropdowns but
 * I'd also like to use this to facilitate writing GUI builder and monitoring tools.
 *
 * @tparam Nc
 * @tparam V
 * @tparam N
 */
template <template <typename V> class Nc, typename V, typename N>
class ActiveVector {
   public:
    typedef typename vector<pair<V*, HybridNode<V>*>>::iterator storageVectorIterator;

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
        int i = 0;
        for (storageVectorIterator it = storageVector_.begin(); it != storageVector_.end();
             it++, i++) {
            if (findFunction(storageVector_[i])) return storageVector_[i];
        }
        return pair(nullptr, nullptr);
    }

    /**
     * @brief This method is designed to construct the entire control element for a given source
     * element. This control may include things like a delete button just for this source element or
     * other things besided the control that manipulates the source element directly.
     *
     * @param v
     * @return HybridNode<V>*
     */
    virtual HybridNode<V>* makeElementControl(V* v) {
        auto* reprNode = makeElementRepresentation(v);
        storageVectorIterator currentFirst = storageVector_.end();
        CLNodeFactory<HybridNode, bool, int> checkboxBuilder(builder_);

        val deleteFirstEL = deleteFirstFn();

        auto* deleteButton =
            builder_.withAttributes({{"class", val("buttonDelete")}})
                .button("delete_btn_" + clto_str(reprNode->getId()), "X", deleteFirstEL);

        auto* grp = builder_.group({reprNode, deleteButton});

        return grp;
    }

    /**
     * @brief The method that generates the core control element for a given source element. This
     * method should be overridden for each subclass.
     *
     * @return HybridNode<V>*
     */
    virtual HybridNode<V>* makeElementRepresentation(V*) { return nullptr; };  // FIXME!

    virtual void push_back(V* v) {
        HybridNode<V>* node = makeElementControl(v);
        storageVector_.push_back(pair(v, node));
        currentIndex_++;
    }

    storageVectorIterator erase(storageVectorIterator position) {
        return storageVector_.erase(position);
    }

    /**
     * @brief Erase the nth element, deleting the content of the paired pointers as well.
     *
     * @param n
     * @return storageVectorIterator
     */
    storageVectorIterator eraseNth(int n) {
        countElements();
        storageVectorIterator nIter = storageVector_.begin() + n;
        auto [element, node] = storageVector_[n];
        delete element;
        auto* rootNode = builder_.getParent();
        rootNode->removeChild(node);
        delete node;
        return storageVector_.erase(nIter);
    }

    // protected:
    CLNodeFactory<Nc, V, N> builder_;
    vector<pair<V*, HybridNode<V>*>> storageVector_;
    int currentIndex_ = 0;
};

#endif