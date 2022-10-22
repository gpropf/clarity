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

template <typename V>
class Select : public HybridNode<V> {
   public:
    Select(const string &name, const string &tag, bool useExistingDOMElement,
                  ClarityNode::AttachmentMode attachmentMode, const string &attachmentId = "")
        : HybridNode<V>(name, tag, useExistingDOMElement, attachmentMode, attachmentId) {
          //this->cle_.template call<void>("addOptionElementFromString", val((*this->cppVal_)[0])); 
           //this->cle_.template call<void>("addOptionElementFromString"); 

        }

    ~Select() { cout << "DESTROYING Select with id: " << this->id_ << "\n"; }

    inline string cppValToString() const {
        if (this->cppVal_ == nullptr) return "Select NULLPTR";
        //return clto_str(*(reinterpret_cast<V *>(this->cppVal_)));
        return string("FIXME");
    }

    virtual void refreshDOMValueFromModel(){};
  

    

   protected:

};


}  // namespace clarity

#endif
