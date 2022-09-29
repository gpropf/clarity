#include "ClarityNode.hpp"

bool ClarityNodeBase::appendChild(ClarityNodeBase *child) {
    children_.push_back(child);
    child->setParent(this);
    cle_.call<void>("appendChild", child->getCLE());
    return true;  // FIXME: need to check for duplicate ids.
}

inline string clarity::ClarityNodeBase::nodeStats(const string &msg) const {
    string s = "NS: Node name: " + name_ + ", Node id: " + to_string(id_) +
               ", Node type: " + typeid(*this).name() +
               ", peer count = " + to_string(countPeers()) + msg + "\n";
    return s;
}

void clarity::ClarityNodeBase::pushValToPeersById(int id) {
    ClarityNodeBase *cnn = getCLElementById(id);
    if (cnn->translator_ != nullptr) {
        // val newJSVal = cnn->translator_->text2jsval();
        val newJSVal = cnn->translator_->js2datum();
    }
    cnn->pushValToPeers(cnn);
}

void clarity::ClarityNodeBase::js2datumWithPushToPeersById(int id) {
    ClarityNodeBase *cnn = getCLElementById(id);
    cnn->js2datumWithPushToPeers();
}

void clarity::ClarityNodeBase::js2datumWithPushToPeers() {
    if (translator_ != nullptr) {
        // val newJSVal = cnn->translator_->text2jsval();
        val newJSVal = translator_->js2datum();
    }
    pushValToPeers(this);
}

inline void clarity::ClarityNodeBase::init() {
    id_ = tm_.getNext();
    // Set up all nodes as single valued by default.

    ClarityNodeBase::switchboard[id_] = this;
}

template <>
void ClarityNode<int>::fromString(string sval) {
    *cptr_ = stoi(sval);
}

/**
 * @brief The most critical way this is used is to move data to/from ModelNodes.
 * In moving data from a ModelNode the initial getVal call occurs in a ModelNode
 * and the setVal calls are in the corresponding ClarityNodes.
 *
 * @param dl
 */
void clarity::ClarityNodeBase::pushValToPeer(DualLink &dl) {
    val internalVal;
    if (translator_ != nullptr)
        internalVal = translator_->text2jsval();
    else
        internalVal = getJSVal();

    cle_.call<void>("printVal", internalVal,
                    val("pushValToPeer: id_ = " + to_string(id_)));

    auto [peer, xfmr] = dl.getOtherNode(this);
    if (internalVal.isNumber()) {
        val transformedVal =
            CLElement_.call<val>("applyTransformFn", xfmr, internalVal);
        peer->setVal(transformedVal);
    } else {
        peer->setVal(internalVal);
    }
}

void clarity::ClarityNodeBase::pushValToPeers(ClarityNodeBase *excludedPeer) {
    if (clean_) {
        return;
    }

    clean_ = true;
    if (excludedPeer == nullptr) {
        for (auto dl : dlpeers_) {
            auto [peer, xfmr] = dl->getOtherNode(this);
            pushValToPeer(*dl);
            peer->pushValToPeers(this);
        }
    } else {
        for (auto dl : dlpeers_) {
            auto [peer, xfmr] = dl->getOtherNode(this);
            if (peer != excludedPeer) {
                pushValToPeer(*dl);
                peer->pushValToPeers(this);
            }
        }
    }
    clean_ = false;
}

// void clarity::ClarityNode::pullValFromPeer(DualLink &dl) {
//     if (clean_) {
//     }

//     auto [peer, xfmr] = dl.getOtherNode(this);
//     val internalVal = peer->getVal();
//     if (internalVal.isNumber()) {
//         val transformedVal = ClarityNode::DualLink::CLElement_.call<val>(
//             "applyTransformFn", xfmr, internalVal);
//         setVal(transformedVal);
//     } else {
//         setVal(internalVal);
//     }

//     clean_ = true;
// }

// void clarity::ClarityNode::pullValFromPeers(ClarityNode *excludedPeer) {
//     if (excludedPeer == nullptr) {
//         for (auto dl : dlpeers_) {
//             pullValFromPeer(*dl);
//         }
//     } else {
//         for (auto dl : dlpeers_) {
//             auto [peer, xfmr] = dl->getOtherNode(this);
//             if (peer != excludedPeer) {
//                 pullValFromPeer(*dl);
//             }
//         }
//     }
// }

// void clarity::ClarityNode::pullValFromPeersById(int id) {
//     ClarityNode *cnn = getCLElementById(id);
//     cnn->pullValFromPeers(cnn);
// }

inline void ClarityNodeBase::setAttribute(const string &attr,
                                          const val &value) {
    val domElement = cle_["domElement"];
    domElement.call<void>("setAttribute", attr, value);
}

inline void ClarityNodeBase::setAttributes(const map<string, val> &attrs) {
    for (auto [attrName, value] : attrs) {
        setAttribute(attrName, value);
    }
}

// inline void ClarityNode::setStoredValueType(CppType cppType) {
//     storedValueType_ = cppType;
//     cle_.set("cpptype", cppType);
// }

inline void ClarityNodeBase::addEventListenerByName(
    const string &eventName, const string &callbackName) {
    cle_.call<void>("addEventListenerById", eventName, callbackName);
}

inline void ClarityNodeBase::addJSEventListener(const string &eventName,
                                                val eventCallback) {
    cle_.call<void>("addEventListener", eventName, eventCallback);
}
