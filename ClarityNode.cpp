#include "ClarityNode.hpp"

bool ClarityNode::appendChild(ClarityNode *child) {
    children_.push_back(child);
    child->setParent(this);
    cle_.call<void>("appendChild", child->getCLE());
    return true;  // FIXME: need to check for duplicate ids.
}

inline string clarity::ClarityNode::nodeStats(const string &msg) const {
    string s = "NS: Node name: " + name_ + ", Node id: " + to_string(id_) +
               ", Node type: " + typeid(*this).name() +
               ", peer count = " + to_string(countPeers()) + msg + "\n";
    return s;
}

void clarity::ClarityNode::pushValToPeersById(int id) {
    ClarityNode *cnn = getCLElementById(id);
    cnn->pushValToPeers(cnn);
}

inline void clarity::ClarityNode::init() {
    id_ = tm.getNext();
    // Set up all nodes as single valued by default.
    dataDimensionality_[0] = 1;
    dataDimensionality_[1] = 0;
    ClarityNode::switchboard[id_] = this;
}

inline clarity::ClarityNode::ClarityNode(const string &name,
                                         const CppType storedValueType)
    : name_(name), storedValueType_(storedValueType) {
    init();
    cle_.set("cpptype", val(storedValueType));
}

inline clarity::ClarityNode::ClarityNode(const CppType storedValueType)
    : storedValueType_(storedValueType) {
    init();
    cle_.set("cpptype", val(storedValueType));
}

ClarityNode::ClarityNode(const string &name, const string &tag,
                         const CppType storedValueType,
                         bool useExistingDOMElement)
    : name_(name), tag_(tag), storedValueType_(storedValueType) {
    init();
    if (!useExistingDOMElement)
        cle_.call<void>("createDOMElement", id_, tag, storedValueType, name);
    cle_.set("name", val(name));
    // For some reason the code that sets the name in clarity.js doesn't "take"
    // so we re-set it here.

    domElement_ = cle_["domElement"];

    boundField_ = "value";
    ClarityNode::switchboard[id_] = this;
}

/**
 * @brief The most critical way this is used is to move data to/from ModelNodes.
 * In moving data from a ModelNode the initial getVal call occurs in a ModelNode
 * and the setVal calls are in the corresponding ClarityNodes.
 *
 * @param dl
 */
void clarity::ClarityNode::pushValToPeer(DualLink &dl) {
    val internalVal = getVal();

    auto [peer, xfmr] = dl.getOtherNode(this);
    if (internalVal.isNumber()) {
        val transformedVal = ClarityNode::DualLink::CLElement_.call<val>(
            "applyTransformFn", xfmr, internalVal);
        peer->setVal(transformedVal);
    } else {
        peer->setVal(internalVal);
    }
}

void clarity::ClarityNode::pushValToPeers(ClarityNode *excludedPeer) {
    if (clean_) {
        return;
    }

    clean_ = true;
    if (excludedPeer == nullptr) {
        for (auto dl : dlpeers_) {
            auto [peer, xfmr] = dl->getOtherNode(this);
            peer->pushValToPeers(this);
            pushValToPeer(*dl);
        }
    } else {
        for (auto dl : dlpeers_) {
            auto [peer, xfmr] = dl->getOtherNode(this);
            if (peer != excludedPeer) {
                peer->pushValToPeers(this);
                pushValToPeer(*dl);
            }
        }
    }
    clean_ = false;
}

void clarity::ClarityNode::pullValFromPeer(DualLink &dl) {
    if (clean_) {
    }

    auto [peer, xfmr] = dl.getOtherNode(this);
    val internalVal = peer->getVal();
    if (internalVal.isNumber()) {
        val transformedVal = ClarityNode::DualLink::CLElement_.call<val>(
            "applyTransformFn", xfmr, internalVal);
        setVal(transformedVal);
    } else {
        setVal(internalVal);
    }

    clean_ = true;
}

void clarity::ClarityNode::pullValFromPeers(ClarityNode *excludedPeer) {
    if (excludedPeer == nullptr) {
        for (auto dl : dlpeers_) {
            pullValFromPeer(*dl);
        }
    } else {
        for (auto dl : dlpeers_) {
            auto [peer, xfmr] = dl->getOtherNode(this);
            if (peer != excludedPeer) {
                pullValFromPeer(*dl);
            }
        }
    }
}

void clarity::ClarityNode::pullValFromPeersById(int id) {
    ClarityNode *cnn = getCLElementById(id);
    cnn->pullValFromPeers(cnn);
}

void clarity::ClarityNode::addPeer(ClarityNode *peer, val a2b_xfmr,
                                   val b2a_xfmr) {
    auto dl = make_shared<DualLink>(this, peer, a2b_xfmr, b2a_xfmr);
    dlpeers_.push_back(dl);
    peer->appendDualLink(dl);
}

inline void ClarityNode::setAttribute(const string &attr, const val &value) {
    val domElement = cle_["domElement"];
    domElement.call<void>("setAttribute", attr, value);
}

inline void ClarityNode::setAttributes(const map<string, val> &attrs) {
    for (auto [attrName, value] : attrs) {
        setAttribute(attrName, value);
    }
}

inline void ClarityNode::setStoredValueType(CppType cppType) {
    storedValueType_ = cppType;
    cle_.set("cpptype", cppType);
}

inline void ClarityNode::addEventListenerByName(const string &eventName,
                                                const string &callbackName) {
    cle_.call<void>("addEventListenerById", eventName, callbackName);
}

inline void ClarityNode::addJSEventListener(const string &eventName,
                                            val eventCallback) {
    cle_.call<void>("addEventListener", eventName, eventCallback);
}
