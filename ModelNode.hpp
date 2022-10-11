
#ifndef ModelNode_hpp
#define ModelNode_hpp

#include "ClarityNode.hpp"
#include "clarity.hpp"

namespace clarity {

/**
 * @brief This class acts as the interface between the view we are building and
 * the data being modelled. These nodes do not have a representation on the
 * screen.
 *
 * @tparam T
 */

template <typename V>
class ModelNode : public ClarityNode {
   public:
    ~ModelNode() {
        cout << "DESTROYING MODELNODE " << id_ << "\n";        
    }

    ModelNode() : ClarityNode() {}

    virtual void refreshDOMValueFromModel(){};
    virtual void updateNodeFromDom(){};

    inline string cppValToString() const {
        if (cppVal_ == nullptr) return "ModelNode NULLPTR";
        return clto_str(*(reinterpret_cast<V *>(this->cppVal_)));
    }

    //virtual val mn_getVal() const { return val(NULL); };

    ModelNode(V *dynval) : ClarityNode() { cppVal_ = dynval; }

    ModelNode(V *dynval, const string &name) : ModelNode(dynval) { name_ = name; }

    virtual val getVal() const {
        if (cppVal_ == nullptr) {
            return val(NULL);
        }
        return val(cpp2js<V>(cppVal_));
    }

    V jsToCppVal(val jsval) {
        V cppVal = jsval.as<V>();
        *reinterpret_cast<V *>(cppVal_) = cppVal;
        return cppVal;
    }

    void setVal(const val &inval) {
        assert(cppVal_ != nullptr);
        // *reinterpret_cast<V *>(cppVal_) =
        //     this->cle_.template call<V>("jsToCPPVal", inval);
        V newCppVal = jsToCppVal(inval);
        cout << "New C++ value from JS val: " << newCppVal << "\n";
        pushValToPeers(this);
    }

    V *cppVal_;  //!< The C++ data object that acts as the 'model'
};
}  // namespace clarity

#endif
