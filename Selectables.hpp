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
        // if (domElement != val::null()) {
        // Selectables.call<void>("installEventListeners", domElement, val("select"), val(""));
        //}
    }

    ~Select() { cout << "DESTROYING Select with id: " << this->id_ << "\n"; }

    inline virtual void finalize() {
        cout << "Select::finalize()\n";
        val Selectables = val::global("Selectables");
        val expListenerGens = Selectables["expListenerGens"];
        ClarityNode::CLElement_.call<void>("installEventListenersByTagAndType", this->cle_,
                                           expListenerGens);
    }

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

    virtual void updateNodeFromDom() {
        cout << "Select::updateNodeFromDom() FIXME! This method does nothing.\n";
        // val jsval = this->getVal();
        // if (this->cppVal_ != nullptr) {
        //     cout << "cppVal_ exists!\n";
        //     //*cppVal_ = jsval.as<V>();
        // }
    }

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

    // virtual void updateNodeFromDom() {
    //     cout << "************************************ Checkbox::updateNodeFromDom()\n";
    //     val jsval = this->getVal();
    //     val checked = this->domElement_["checked"];
    //     cout << "this->domElement_['checked'] = " << checked.as<bool>() << "\n";
    //     bool v = this->domElement_.template call<bool>("getAttribute", val("checked"));
    //     cout << "Box is: " << v << "\n";

    //     if (this->cppVal_ != nullptr) {
    //         cout << "cppVal_ exists!\n";
    //         *this->cppVal_ = jsval.as<V>();
    //         cout << "Checkbox cppVal_ is now: " << *this->cppVal_ << "\n";
    //     }
    // }

   protected:
};

}  // namespace clarity

#endif
