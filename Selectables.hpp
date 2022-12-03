/**
 * @file Selectables.hpp
 * @author Greg Propf
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
// Leaving this commented out since it's not actually used here.

// template <template <typename, typename> class CollectionType, class ItemType,
//           class Allocator = std::allocator<ItemType>>
// class Test {
//    public:
//     CollectionType<ItemType, Allocator> m_collection;
// };

/**
 * @brief Like SimpleSelect, represents a selection box. This variant expects the stored value to
 * represent the options as a vector of pairs.
 *
 * @tparam V
 */
template <typename V>
class Select : public HybridNode<V> {
   public:
    Select(const string &name, const string &tag, bool useExistingDOMElement,
           ClarityNode::AttachmentMode attachmentMode, const string &attachmentId = "")
        : HybridNode<V>(name, tag, useExistingDOMElement, attachmentMode, attachmentId) {}

    ~Select() { 
        this->nodelog("DESTROYING Select");
    }

    INLINE virtual void finalize() {
        //this->nodelog("Select::finalize()");
        this->jsProxyNode_.set("clarityNode", this);
        val Selectables = val::global("Selectables");
        val listenerGenerators = Selectables["listenerGenerators"];
        ClarityNode::JSProxyNode_.call<void>("installEventListenersByTagAndType2", val("Select"),
                                             this->jsProxyNode_, listenerGenerators);
    }

    virtual string cppValToString() const;

    void populateOptions() {
        for (auto opt : *this->cppVal_) {
            auto [optFirst, optSecond] = opt;
            this->jsProxyNode_.template call<void>("addOptionElementFromValueLabelPair",
                                                   val(optFirst), val(optSecond));
        }
    }

    inline virtual void refreshDOMValueFromModel() { populateOptions(); };

    virtual void updateNodeFromDom() {
        this->runStateFunction();
        this->nodelog("Select::updateNodeFromDom() FIXME! This method does nothing.");
    }

    INLINE virtual string getNodeTypeCode() { return string("SV"); }

    inline virtual void doNothing() {
        this->nodelog("This method exists so that Embind will create a type when this method in bound.");             
    }

   protected:
};

/**
 * @brief Basic select box control. The cppVal_ is the current selection, typically a small integer.
 *
 * @tparam V
 */
template <typename V>
class SimpleSelect : public HybridNode<V> {
   public:
    SimpleSelect(const string &name, const string &tag, bool useExistingDOMElement,
                 ClarityNode::AttachmentMode attachmentMode, const string &attachmentId = "")
        : HybridNode<V>(name, tag, useExistingDOMElement, attachmentMode, attachmentId) {}

    ~SimpleSelect() { this->nodelog("DESTROYING SimpleSelect"); }

    //inline void foo() { cout << "SimpleSelect says FOO!" << endl; }

    inline virtual void doNothing() {
        this->nodelog("This method exists so that Embind will create a type when this method is bound.");        
    }    

    INLINE virtual void finalize() {        
        this->jsProxyNode_.set("clarityNode", this);
        val Selectables = val::global("Selectables");
        val listenerGenerators = Selectables["listenerGenerators"];
        ClarityNode::JSProxyNode_.call<void>("installEventListenersByTagAndType2",
                                             val("SimpleSelect"), this->jsProxyNode_,
                                             listenerGenerators);
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
            this->jsProxyNode_.template call<void>("addOptionElementFromValueLabelPair",
                                                   val(optFirst), val(optSecond),
                                                   val(optFirst == *this->cppVal_));
        }
    }

    inline virtual void refreshDOMValueFromModel() { populateOptions(); };

    virtual void updateNodeFromDom() {
        int currentSelection = this->jsProxyNode_["currentSelection"].template as<int>();
        this->nodelog("SimpleSelect::updateNodeFromDom() currentSelection = " + clto_str(currentSelection));       
        *this->cppVal_ = currentSelection;
        this->runStateFunction();
    }

    INLINE virtual string getNodeTypeCode() { return string("SS"); }

    /**
     * @brief Get the label corresponding to the current selection.
     *
     * @return string
     */
    inline string getSelectedLabel() { return this->options_[*this->cppVal_].second; }

    inline void setOptions(vector<pair<V, string>> &options) { options_ = options; }

    virtual void setDOMVal(const val &inval);

   protected:
    vector<pair<V, string>> options_;  //!< vector of options for the selector element.
};

/**
 * @brief Basic checkbox control.
 *
 * @tparam V
 */
template <typename V>
class Checkbox : public HybridNode<V> {
   public:
    Checkbox(const string &name, const string &tag, bool useExistingDOMElement,
             ClarityNode::AttachmentMode attachmentMode, const string &attachmentId = "")
        : HybridNode<V>(name, tag, useExistingDOMElement, attachmentMode, attachmentId) {}   

    INLINE virtual void finalize() {        
        this->jsProxyNode_.set("clarityNode", this);
        val Selectables = val::global("Selectables");
        val listenerGenerators = Selectables["listenerGenerators"];
        ClarityNode::JSProxyNode_.call<void>("installEventListenersByTagAndType2", val("Checkbox"),
                                             this->jsProxyNode_, listenerGenerators);
    }

    ~Checkbox() { this->nodelog("DESTROYING Checkbox"); }

    inline string cppValToString() const {
        if (this->cppVal_ == nullptr) return "Checkbox NULLPTR";
        return string("FIXME");
    }

    virtual void refreshDOMValueFromModel() {        
        HybridNode<V>::refreshDOMValueFromModel();
    };

    INLINE virtual string getNodeTypeCode() { return string("CB"); }

    virtual val getVal() const;

    virtual void setDOMVal(const val &inval);

    inline virtual void doNothing() {
         this->nodelog("This method exists so that Embind will create a type when this method is bound.");
    }

   protected:
};

}  // namespace clarity

#endif
