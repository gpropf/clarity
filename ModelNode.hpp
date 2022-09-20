
#ifndef ModelNode_hpp
#define ModelNode_hpp

#include "ClarityNode.hpp"
#include "clarity.hpp"

namespace clarity {

/**
 * @brief This class acts as the interface between the view we are building and
 * the data being modelled. These nodes do not have a representation on the screen.
 *
 * @tparam T
 */

template <typename T>
class ModelNode : public ClarityNode {
   public:
    ~ModelNode() {
        cout << "DESTROYING MODELNODE " << id_ << "\n";
        for (auto dl : dlpeers_) {
            dl.reset();
        }
    }

    ModelNode(CppType storedValueType) : ClarityNode(storedValueType) {}

    ModelNode(T *dynval) : dynval_(dynval) {}

    ModelNode(T *dynval, CppType storedValueType)
        : ClarityNode(storedValueType) {
        dynval_ = dynval;
    }

    ModelNode(T *dynval, CppType storedValueType, const string &name)
        : ModelNode(dynval, storedValueType) {
        name_ = name;
    }

    virtual val getVal() const {
        if (dynval_ == nullptr) {
            return val(NULL);
        }
        return val(cpp2js<T>(dynval_));
    }

    void setVal(const val &inval) {
        assert(dynval_ != nullptr);
        *reinterpret_cast<T *>(dynval_) =
            this->cle_.template call<T>("jsToCPPVal", inval);
        pushValToPeers(this);
    }

    T *dynval_;  //!< The C++ data object that acts as the 'model'
};
}  // namespace clarity

#endif
