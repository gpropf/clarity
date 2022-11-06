/**
 * @file Selectables.hpp
 * @author your name (you@domain.com)
 * @brief Controls that feature groups of things that can be selected are here. This includes things
 * like <select>, checkbox groups, and radio button groups.
 * @version 1.0
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef Selectables_hpp
#define Selectables_hpp

#include "ClarityNode.hpp"

namespace clarity {

// Example from Stack Overflow about collection template might be relevant here.
// https://stackoverflow.com/questions/956658/can-you-use-c-templates-to-specify-a-collection-type-and-the-specialization-of
template <template <typename, typename> class CollectionType, class ItemType,
          class Allocator = std::allocator<ItemType>>
class Test {
   public:
    CollectionType<ItemType, Allocator> m_collection;
};

template <typename V>
class Select : public HybridNode<V> {
   public:
    Select(const string &name, const string &tag, bool useExistingDOMElement,
           ClarityNode::AttachmentMode attachmentMode, const string &attachmentId = "")
        : HybridNode<V>(name, tag, useExistingDOMElement, attachmentMode, attachmentId) {}

    ~Select() { cout << "DESTROYING Select with id: " << this->id_ << "\n"; }

    INLINE virtual void finalize() {
        cout << "Select::finalize()\n";
        val Selectables = val::global("Selectables");
        val expListenerGens = Selectables["expListenerGens"];
        ClarityNode::CLElement_.call<void>("installEventListenersByTagAndType", this->cle_,
                                           expListenerGens);
    }

    virtual string cppValToString() const;

    //  {
    //     if (this->cppVal_ == nullptr) return "Select NULLPTR";
    //     // return clto_str(*(reinterpret_cast<V *>(this->cppVal_)));
    //     return string("FIXME");
    // }

    void populateOptions() {
        for (auto opt : *this->cppVal_) {
            auto [optFirst, optSecond] = opt;
            this->getCLE().template call<void>("addOptionElementFromValueLabelPair", val(optFirst),
                                               val(optSecond));
        }
    }

    inline virtual void refreshDOMValueFromModel() { populateOptions(); };

    virtual void updateNodeFromDom() {
        cout << "Select::updateNodeFromDom() FIXME! This method does nothing.\n";
    }

    // The "V" is for vector.
    INLINE virtual string getNodeTypeCode() { return string("SV"); }

   protected:
};

template <typename V>
class SimpleSelect : public HybridNode<V> {
   public:
    SimpleSelect(const string &name, const string &tag, bool useExistingDOMElement,
                 ClarityNode::AttachmentMode attachmentMode, const string &attachmentId = "")
        : HybridNode<V>(name, tag, useExistingDOMElement, attachmentMode, attachmentId) {}

    ~SimpleSelect() { cout << "DESTROYING SimpleSelect with id: " << this->id_ << "\n"; }

    INLINE virtual void finalize() {
        cout << "SimpleSelect::finalize()\n";
        val Selectables = val::global("Selectables");
        val simpleSelectGens = Selectables["simpleSelectGens"];
        ClarityNode::CLElement_.call<void>("installEventListenersByTagAndType", this->cle_,
                                           simpleSelectGens);
    }

    virtual string cppValToString() const;

    /**
     * @brief Make <option> tags for each option. If any option's value is equal to the current
     * modelVal we show it as pre-selected.
     *
     */
    void populateOptions() {
        for (auto opt : options_) {
            auto [optFirst, optSecond] = opt;
            this->getCLE().template call<void>("addOptionElementFromValueLabelPair", val(optFirst),
                                               val(optSecond), val(optFirst == *this->cppVal_));
        }
    }

    inline virtual void refreshDOMValueFromModel() { populateOptions(); };

    virtual void updateNodeFromDom() {
        int currentSelection = this->cle_["currentSelection"].template as<int>();
        cout << "SimpleSelect::updateNodeFromDom() currentSelection = " << currentSelection << endl;
        *this->cppVal_ = currentSelection;
    }

    // The "V" is for vector.
    INLINE virtual string getNodeTypeCode() { return string("SS"); }

    inline void setOptions(vector<pair<V, string>> &options) { options_ = options; }

    virtual void setDOMVal(const val &inval);

   protected:
    vector<pair<V, string>> options_;
};

template <typename V>
class Checkbox : public HybridNode<V> {
   public:
    Checkbox(const string &name, const string &tag, bool useExistingDOMElement,
             ClarityNode::AttachmentMode attachmentMode, const string &attachmentId = "")
        : HybridNode<V>(name, tag, useExistingDOMElement, attachmentMode, attachmentId) {
        // val domElement = ClarityNode::getDomElement();

        // if (domElement != val::null()) {

        //}
    }

    inline virtual void finalize() {
        cout << "Checkbox::finalize()\n";
        val Selectables = val::global("Selectables");
        val expListenerGens = Selectables["expListenerGens"];
        ClarityNode::CLElement_.call<void>("installEventListenersByTagAndType", this->cle_,
                                           expListenerGens);
    }

    ~Checkbox() { cout << "DESTROYING Checkbox with id: " << this->id_ << "\n"; }

    inline string cppValToString() const {
        if (this->cppVal_ == nullptr) return "Checkbox NULLPTR";
        // return clto_str(*(reinterpret_cast<V *>(this->cppVal_)));
        return string("FIXME");
    }

    virtual void refreshDOMValueFromModel() {
        cout << "Checkbox::refreshDOMValueFromModel()\n";
        HybridNode<V>::refreshDOMValueFromModel();
    };

    // The "V" is for vector.
    INLINE virtual string getNodeTypeCode() { return string("CB"); }

    virtual val getVal() const;

    virtual void setDOMVal(const val &inval);

   protected:
};

}  // namespace clarity

#endif
