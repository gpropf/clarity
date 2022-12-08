#ifndef ActiveVector_hpp
#define ActiveVector_hpp

#include <iostream>
#include <vector>

#include "CLNodeFactory.hpp"

namespace clarity {

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
class ActiveVector : public std::vector<std::pair<V*, HybridNode<V>*>> {
   public:
    typedef typename std::pair<V*, HybridNode<V>*> ActivePairT;
    typedef typename vector<ActivePairT>::iterator StorageVectorIteratorT;
    typedef typename std::function<bool(pair<V*, HybridNode<V>*>)> ActivePairLambdaT;

    static ActivePairLambdaT removeCheckedFn_;

    ActiveVector(HybridNode<V>* rootNode) { builder_ = builder_.withChildrenOf(rootNode); }

    void countElements() {
        cout << "This vector has " << this->storageVector_.size() << " elements" << endl;
        cout << "this pointer = " << this << endl;
        for (auto& p : storageVector_) {
            auto [element, node] = p;
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
        for (StorageVectorIteratorT it = storageVector_.begin(); it != storageVector_.end();
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
        StorageVectorIteratorT currentFirst = storageVector_.end();
        CLNodeFactory<HybridNode, bool, int> checkboxBuilder(builder_);

        val deleteFirstEL = deleteFirstFn();

        auto* deleteBox =
            checkboxBuilder.withName("delete_" + clto_str(reprNode->getId())).checkbox();

        auto* grp = builder_.group({reprNode, deleteBox});

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

    StorageVectorIteratorT erase(StorageVectorIteratorT position) {
        return storageVector_.erase(position);
    }

    StorageVectorIteratorT remove(StorageVectorIteratorT first, StorageVectorIteratorT last,
                                  const ActivePairT& val) {
        auto [element, node] = val;
        delete element;
        auto* rootNode = builder_.getParent();
        if (rootNode != nullptr) rootNode->removeChild(node);
        delete node;
        return storageVector_.remove(val);
    }

    /**
     * @brief Erase the nth element, deleting the content of the paired pointers as well.
     *
     * @param n
     * @return StorageVectorIteratorT
     */
    StorageVectorIteratorT eraseNth(int n) {
        // countElements();
        StorageVectorIteratorT nIter = storageVector_.begin() + n;
        auto [element, node] = storageVector_[n];
        delete element;
        auto* rootNode = builder_.getParent();
        if (rootNode != nullptr) rootNode->removeChild(node);
        delete node;
        return storageVector_.erase(nIter);
    }

    void deletePair(ActivePairT& p) {
        auto [element, node] = p;
        auto* rootNode = builder_.getParent();
        if (rootNode != nullptr) rootNode->removeChild(node);
        delete element;
        delete node;
        // element = nullptr;
        // node = nullptr;
        countElements();
    }

    // StorageVectorIteratorT remove_if(StorageVectorIteratorT begin, StorageVectorIteratorT end,
    // ActivePairLambdaT filterFn) {
    //     return remove_if(storageVector_.begin(), storageVector_.end(), filterFn);
    // }

    /**
     * @brief Copies elements for which filterFn returns false to a temporary vector while deleting
     * the rest. At the end we use std::move to re-assign the storageVector_ to the moved elements.
     * After experimenting with std::remove_copy_if and std::remove_if and others this is the approach
     * that I think is reasonably efficient and doesn't leak memory.
     *
     * @param filterFn
     */
    void filterWithLambda(ActivePairLambdaT filterFn) {
        int vectorSize = storageVector_.size();
        vector<ActivePairT> tmpVec;
        for (auto& p : storageVector_) {
            if (filterFn(p)) {
                deletePair(p);
                // p.second = 0;
            } else {
                tmpVec.push_back(p);
            }
        }
        storageVector_ = std::move(tmpVec);

        // vector<ActivePairT> tmpVec(vectorSize);
        // StorageVectorIteratorT filterEnd =
        //     remove_copy_if(storageVector_.begin(), storageVector_.end(), tmpVec.begin(),
        //     filterFn);
        // auto* rootNode = builder_.getParent();

        // if (rootNode != nullptr) {
        //     for (auto [element, node] : tmpVec) {
        //         if (element == nullptr) break;
        //         delete element;
        //         delete node;
        //         rootNode->removeChild(node);
        //     }
        // } else {
        //     for (auto [element, node] : tmpVec) {
        //         if (element == nullptr) break;
        //         delete element;
        //         delete node;
        //     }
        // }
        // // storageVector_.shrink_to_fit();

        // return filterEnd;
    }

    void removeChecked() { filterWithLambda(removeCheckedFn_); }

    // protected:
    CLNodeFactory<Nc, V, N> builder_;
    vector<pair<V*, HybridNode<V>*>> storageVector_;
    int currentIndex_ = 0;
};

template <template <typename V> class Nc, typename V, typename N>
typename ActiveVector<Nc, V, N>::ActivePairLambdaT ActiveVector<Nc, V, N>::removeCheckedFn_ =
    [](ActiveVector<Nc, V, N>::ActivePairT ap) {
        auto [element, node] = ap;
        // auto *deleteBox = static_cast<ClarityNode*>(node)->getChildren()[1];
        auto* deleteBox = node->getChildren()[1];
        bool deleteChecked = deleteBox->getVal().template as<bool>();
        cout << "Checkbox: " << deleteChecked << endl;
        // if (deleteChecked) {
        //     delete element;
        //     delete node;
        // }

        return deleteChecked;
    };

}  // namespace clarity
#endif