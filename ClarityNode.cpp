//#include "clarity.hpp"
#include "ClarityNode.hpp"

bool ClarityNode::appendChild(ClarityNode *child) {
    children_.push_back(child);
    child->setParent(this);
    cle_.call<void>("appendChild", child->getCLE());
    return true;  // FIXME: need to check for duplicate ids.
}

clarity::ClarityNode::ActiveLink::ActiveLink(ClarityNode *peer, val scalarConst)
    : peer_(peer), constantOrFunction_(scalarConst) {
    transformFn_ =
        CLElement_.call<val>("generateTransformFn", constantOrFunction_);
}

inline string clarity::ClarityNode::nodeStats(const string &msg) const {
    string s = "NS: Node name: " + name_ + ", Node id: " + to_string(id_) +
               ", Node type: " + typeid(*this).name() +
               ", peer count = " + to_string(countPeers()) + msg + "\n";
    return s;
}

void clarity::ClarityNode::pushValToPeersById2(int id) {
    ClarityNode *cnn = getCLElementById(id);
    //cnn->pushValToPeers(cnn);
    cnn->pushValToPeers2(cnn);
}

inline void clarity::ClarityNode::init() {
    id_ = tm.getNext();
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
                         bool useExistingDOMElement_)
    : name_(name), tag_(tag), storedValueType_(storedValueType) {
    init();
    if (!useExistingDOMElement_)
        cle_.call<void>("createDOMElement", id_, tag, storedValueType, name);
    cle_.set("name", val(name));
    // For some reason the code that sets the name in clarity.js doesn't "take"
    // so we re-set it here.

    // tag_ = tag;
    // name_ = name;
    boundField_ = "value";
    ClarityNode::switchboard[id_] = this;
}

void clarity::ClarityNode::pushValToPeer(ActiveLink &al, const string &tabs) {
    if (clean_) {
    }

    val internalVal = getVal();

    // al.printAL();
    cout << tabs << "void clarity::ControlNetworkNode::pushValToPeer\n";
    cout << tabs << al.peer_->nodeStats();

    if (internalVal.isNumber()) {
        val product = al.CLElement_.call<val>("applyTransformFn",
                                              al.transformFn_, internalVal);
        al.peer_->setVal(product);
    } else {
        al.peer_->setVal(internalVal);
    }

    clean_ = true;
}

void clarity::ClarityNode::pushValToPeer2(DualLink &dl, const string &tabs) {
    if (clean_) {
    }

    val internalVal = getVal();

    // al.printAL();
    cout << tabs << "pushValToPeer2(): ";
    cout << tabs << "nodeA: " << dl.nodeA_->getId()
         << ", nodeB: " << dl.nodeB_->getId() << "\n";

    auto [peer, xfmr] = dl.getOtherNode(this);
    if (internalVal.isNumber()) {
        val product = ClarityNode::DualLink::CLElement_.call<val>(
            "applyTransformFn", xfmr, internalVal);
        peer->setVal(product);
    } else {
        peer->setVal(internalVal);
    }

    clean_ = true;
}

// void clarity::ClarityNode::pushValToPeers(ClarityNode *excludedPeer) {
//     cout << "ControlNetworkNode::pushValToPeers, id = " << id_ << " Node has "
//          << to_string(countPeers()) << " peers!!!!!!!!!!!!!!!!!!!!!!!\n";
//     // cout << "ControlNetworkNode::pushValToPeers ==>> " << nodeStats();
//     if (excludedPeer == nullptr) {
//         cout << "\tNo peers excluded.\n";
//         for (auto peer : peers_) {
//             pushValToPeer(peer);
//         }
//     } else {
//         cout << "\tExcluded peer exists!\n";
//         for (auto peer : peers_) {
//             if (peer.peer_ != excludedPeer) {
//                 pushValToPeer(peer);
//             }
//         }
//     }
// }

void clarity::ClarityNode::pushValToPeers2(ClarityNode *excludedPeer) {
    cout << "ControlNetworkNode::pushValToPeers2, id = " << id_ << " Node has "
         << to_string(countPeers()) << " peers!!!!!!!!!!!!!!!!!!!!!!!\n";
    // cout << "ControlNetworkNode::pushValToPeers ==>> " << nodeStats();
    if (excludedPeer == nullptr) {
        cout << "\tNo peers excluded.\n";
        for (auto dl : dlpeers_) {
            pushValToPeer2(*dl);
        }
    } else {
        cout << "\tExcluded peer exists!\n";
        for (auto dl : dlpeers_) {
            auto [peer, xfmr] = dl->getOtherNode(this);
            if (peer != excludedPeer) {
                pushValToPeer2(*dl);
            }
        }
    }
}

// void clarity::ClarityNode::addPeer(ClarityNode::ActiveLink al,
//                                    bool alreadyAdded) {
//     peers_.push_back(al);
//     if (alreadyAdded) {
//         return;
//     }
//     al.peer_->addPeer(
//         ActiveLink(this, cle_.call<val>("invertValue", al.constantOrFunction_)),
//         true);
//     // cout << nodeStats(". ControlNetworkNode::addPeer: There are " +
//     //                   to_string(countPeers()) + " peer nodes.\n");
// }

void clarity::ClarityNode::addPeer2(ClarityNode *peer) {
    auto dl = make_shared<DualLink>(this, peer, 1);
    dlpeers_.push_back(dl);    
    peer->appendDualLink(dl);
}

void clarity::ClarityNode::addPeer2(ClarityNode *peer, val a2b_xfmr, val b2a_xfmr) {
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
