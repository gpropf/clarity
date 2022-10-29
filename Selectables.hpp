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
          class Allocator = std::allocator<ItemType> >
class Test {
   public:
    CollectionType<ItemType, Allocator> m_collection;
};

template <typename V>
class Select : public HybridNode<V> {
   public:
    Select(const string &name, const string &tag, bool useExistingDOMElement,
           ClarityNode::AttachmentMode attachmentMode, const string &attachmentId = "")
        : HybridNode<V>(name, tag, useExistingDOMElement, attachmentMode, attachmentId) {
        val domElement = ClarityNode::getDomElement();

        val Selectables = val::global("Selectables");
        //if (domElement != val::null()) {            
        Selectables.call<void>("installEventListeners", domElement, val("select"), val(""));            
        //}        
    }

    ~Select() { cout << "DESTROYING Select with id: " << this->id_ << "\n"; }

    inline string cppValToString() const {
        if (this->cppVal_ == nullptr) return "Select NULLPTR";
        // return clto_str(*(reinterpret_cast<V *>(this->cppVal_)));
        return string("FIXME");
    }

    void populateOptions() {
        for (auto opt : *this->cppVal_) {
            auto [optFirst, optSecond] = opt;
            this->getCLE().template call<void>("addOptionElementFromString", val(optFirst),
                                               val(optSecond));
        }
    }

    virtual void refreshDOMValueFromModel(){};

    // The "V" is for vector.
    INLINE virtual string getNodeTypeCode() { return string("SV"); }

   protected:
};

template <typename V>
class Checkbox : public HybridNode<V> {
   public:
    Checkbox(const string &name, const string &tag, bool useExistingDOMElement,
           ClarityNode::AttachmentMode attachmentMode, const string &attachmentId = "")
        : HybridNode<V>(name, tag, useExistingDOMElement, attachmentMode, attachmentId) {
        val domElement = ClarityNode::getDomElement();

        val Selectables = val::global("Selectables");
        //if (domElement != val::null()) {            
        Selectables.call<void>("installEventListeners", domElement, val("input"), val("checkbox"));            
        //}        
    }

    ~Checkbox() { cout << "DESTROYING Checkbox with id: " << this->id_ << "\n"; }

    inline string cppValToString() const {
        if (this->cppVal_ == nullptr) return "Checkbox NULLPTR";
        // return clto_str(*(reinterpret_cast<V *>(this->cppVal_)));
        return string("FIXME");
    }
    
    virtual void refreshDOMValueFromModel(){};

    // The "V" is for vector.
    INLINE virtual string getNodeTypeCode() { return string("CB"); }

   protected:
};








}  // namespace clarity

#endif
