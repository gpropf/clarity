
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
 * 
 */


class ModelNode : public ClarityNode {
   public:
    ~ModelNode() {
        cout << "DESTROYING MODELNODE " << id_ << "\n";
        for (auto dl : dlpeers_) {
            dl.reset();
        }
    }

  

    ModelNode(DatumBase *datum, const string &name)
        : datum_(datum), name_(name) {
    
    }

    virtual val getVal() const {
        // if (dynval_ == nullptr) {
        //     return val(NULL);
        // }
        // if (*dataDimensionality_ != 1) {
        //     // We clearly have something with more than one value. We need to
        //     // construct a JS version of the dataDimensionality_ list and return
        //     // it. This will then be sent to the CG's setVal method and used to
        //     // initiate the data stream transfer.
        // }
        // // Single value case, proceed as usual.
        // return val(cpp2js<T>(dynval_));
    }

    void setVal(const val &inval) {
        // assert(dynval_ != nullptr);
        // *reinterpret_cast<T *>(dynval_) =
        //     this->cle_.template call<T>("jsToCPPVal", inval);
        pushValToPeers(this);
    }

    inline void setDatum(DatumBase *datum) {
        datum_ = datum;
    }

   protected:
    DatumBase *datum_;  //!< The native (C++) data this node controls.
   
};
}  // namespace clarity

#endif
