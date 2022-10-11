
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

template <typename T>
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
        return clto_str(*(reinterpret_cast<T *>(this->cppVal_)));
    }

    //virtual val mn_getVal() const { return val(NULL); };

    ModelNode(T *dynval) : ClarityNode() { cppVal_ = dynval; }

    ModelNode(T *dynval, const string &name) : ModelNode(dynval) { name_ = name; }

    virtual val getVal() const {
        if (cppVal_ == nullptr) {
            return val(NULL);
        }
        return val(cpp2js<T>(cppVal_));
    }

    T jsToCppVal(val jsval) {
        T cppVal = jsval.as<T>();
        *reinterpret_cast<T *>(cppVal_) = cppVal;
        return cppVal;
    }

    void setVal(const val &inval) {
        assert(cppVal_ != nullptr);
        // *reinterpret_cast<T *>(cppVal_) =
        //     this->cle_.template call<T>("jsToCPPVal", inval);
        T newCppVal = jsToCppVal(inval);
        cout << "New C++ value from JS val: " << newCppVal << "\n";
        pushValToPeers(this);
    }

    T *cppVal_;  //!< The C++ data object that acts as the 'model'
};
}  // namespace clarity

#endif
