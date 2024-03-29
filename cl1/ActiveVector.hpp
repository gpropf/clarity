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
// class ActiveVector : public std::vector<std::pair<V*, HybridNode<V>*>> {
class ActiveVector {
   public:
    typedef typename std::pair<V*, HybridNode<V>*> ActivePairT;
    typedef typename vector<ActivePairT>::iterator StorageVectorIteratorT;
    typedef typename std::function<bool(pair<V*, HybridNode<V>*>)> ActivePairLambdaT;

    /**
     * @brief Lambda function that looks at the control node for a given element and returns true if
     * the delete box within the control node is checked or false if it is not.
     *
     */
    static ActivePairLambdaT removeCheckedFn_;

    /**
     * @brief Construct a new Active Vector object with all new nodes being added as children of the
     * provided `rootNode`.
     *
     * @param rootNode
     */
    ActiveVector(HybridNode<V>* rootNode) { builder_ = builder_.withChildrenOf(rootNode); }

    /**
     * @brief Utility method for debugging to print out some info on each element.
     *
     */
    void countElements() {
        cout << "This vector has " << this->storageVector_.size() << " elements" << endl;
        cout << "this pointer = " << this << endl;
        for (auto& p : storageVector_) {
            auto [element, node] = p;
            cout << "ELEM: " << *element << " : " << node->getId() << endl;
        }
    }

    /**
     * @brief Returns a JS function that deletes the current element (as defined by `currentIndex_`
     * in the AV.
     *
     * @return val A JS deleter function.
     */
    virtual val deleteCurrentElementEL() = 0;

    pair<V*, HybridNode<V>*> find(std::function<bool(pair<V*, HybridNode<V>*>)> findFunction) {
        int i = 0;
        for (StorageVectorIteratorT it = storageVector_.begin(); it != storageVector_.end();
             it++, i++) {
            if (findFunction(storageVector_[i])) return storageVector_[i];
        }
        return pair(nullptr, nullptr);
    }

    /**
     * @brief Generates the core representation element for each type of source element. As an
     * example, a subclass representing <option> tags would return the option tag for each element
     * here.
     *
     * @param v
     * @return HybridNode<V>*
     */
    virtual HybridNode<V>* makeElementRepresentation(V* v) = 0;

    /**
     * @brief This method is designed to construct the entire control element for a given source
     * element. This control may include things like a delete button just for this source element or
     * other things besided the control that manipulates the source element directly. This is a
     * virtual method because each subclass may require different subnodes. Most will want a
     * checkbox or button to delete the referenced element.
     *
     * @param v
     * @return HybridNode<V>*
     */

    virtual HybridNode<string>* makeElementControl(string* v) {
        auto* reprNode = makeElementRepresentation(v);

        StorageVectorIteratorT currentFirst = storageVector_.end();
        CLNodeFactory<HybridNode, bool, int> checkboxBuilder(builder_);

        val deleteFirstEL = deleteCurrentElementEL();

        auto* deleteBox =
            checkboxBuilder.withName("delete_" + clto_str(reprNode->getId())).checkbox();

        auto* grp = builder_.group({reprNode, deleteBox});

        return grp;
    }

    /**
     * @brief The method that generates the core control element for a given source element.
     *
     * @return HybridNode<V>*
     */
    virtual void addElementWithControls(V* v) {
        HybridNode<V>* node = makeElementControl(v);
        storageVector_.push_back(pair(v, node));
        currentIndex_++;
    }

    /**
     * @brief Adds a new element but simply uses the raw `makeElementRepresentation()` method to
     * create the representation. This is the method we use for things like <option> tags where we
     * are not intending to edit the collection through it representations.
     *
     * @param v
     */
    virtual void addElementWithoutControls(V* v) {
        HybridNode<V>* node = makeElementRepresentation(v);
        storageVector_.push_back(pair(v, node));
        currentIndex_++;
    }

    StorageVectorIteratorT erase(StorageVectorIteratorT position) {
        return storageVector_.erase(position);
    }

    /**
     * @brief Uses 'remove' on the storageVector_ to take out the emenent matching the provided
     * pair, 'val'.
     *
     * @param first
     * @param last
     * @param val
     * @return StorageVectorIteratorT
     */
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
        auto p = storageVector_[n];
        deletePair(p);
        StorageVectorIteratorT nIter = storageVector_.begin() + n;
        return storageVector_.erase(nIter);
    }

    void deletePair(ActivePairT& p) {
        auto [element, node] = p;
        auto* rootNode = builder_.getParent();
        if (rootNode != nullptr) rootNode->removeChild(node);
        delete element;
        delete node;
    }

    /**
     * @brief Copies elements for which filterFn returns false to a temporary vector while deleting
     * the rest. At the end we use std::move to re-assign the storageVector_ to the moved elements.
     * After experimenting with std::remove_copy_if and std::remove_if and others this is the
     * approach that I think is reasonably efficient and doesn't leak memory.
     *
     * @param filterFn
     */
    virtual void filterWithLambda(ActivePairLambdaT filterFn) {
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
    }

    virtual void removeChecked() { filterWithLambda(removeCheckedFn_); }

   protected:
    CLNodeFactory<Nc, V, N> builder_;
    vector<pair<V*, HybridNode<V>*>> storageVector_;
    int currentIndex_ = 0;
};

template <template <typename V> class Nc, typename V, typename N>
typename ActiveVector<Nc, V, N>::ActivePairLambdaT ActiveVector<Nc, V, N>::removeCheckedFn_ =
    [](ActiveVector<Nc, V, N>::ActivePairT ap) {
        auto [element, node] = ap;
        auto* deleteBox = node->getChildren()[1];  // Assumes the delete box is the second element.
        bool deleteChecked = deleteBox->getVal().template as<bool>();
        cout << "Checkbox: " << deleteChecked << endl;
        return deleteChecked;
    };

}  // namespace clarity
#endif