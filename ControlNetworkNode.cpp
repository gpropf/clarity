//#include "clarity.hpp"
#include "ControlNetworkNode.hpp"

bool ControlNetworkNode::appendChild(ControlNetworkNode *child) {
    children_.push_back(child);
    child->setParent(this);
    cle_.call<void>("appendChild", child->getCLE());
    return true;  // FIXME: need to check for duplicate ids.
}

clarity::ControlNetworkNode::ActiveLink::ActiveLink(ControlNetworkNode *peer,
                                                    val scalarConst)
    : peer_(peer), scalarConst_(scalarConst) {
    transformFn_ = CLElement_.call<val>("generateTransformFn", scalarConst_);
}

inline string clarity::ControlNetworkNode::nodeStats(const string &msg) const {
    string s = "Node name: " + name_ + ", Node id: " + to_string(id_) +
               ", Node type: " + typeid(*this).name() + msg + "\n";
    return s;
}

void clarity::ControlNetworkNode::pushValToPeersById(int id) {
    ControlNetworkNode *cnn = getCLElementById(id);
    cnn->pushValToPeers(cnn);
}

inline void clarity::ControlNetworkNode::init() {
    id_ = tm.getNext();
    ControlNetworkNode::switchboard[id_] = this;
}

inline clarity::ControlNetworkNode::ControlNetworkNode(
    const string &name, const CppType storedValueType)
    : name_(name), storedValueType_(storedValueType) {
    init();
    cle_.set("cpptype", val(storedValueType));
}

inline clarity::ControlNetworkNode::ControlNetworkNode(
    const CppType storedValueType)
    : storedValueType_(storedValueType) {
    init();
    cle_.set("cpptype", val(storedValueType));
}

ControlNetworkNode::ControlNetworkNode(const string &name, const string &tag,
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
    ControlNetworkNode::switchboard[id_] = this;
}

void clarity::ControlNetworkNode::pushValToPeer(ActiveLink &al) {
    if (clean_) {
    }

    val internalVal = getVal();

    al.printAL();
    // cout << "void clarity::ControlNetworkNode::pushValToPeer\n";
    //cout << nodeStats("pushValToPeer");

    if (internalVal.isNumber()) {
        val product = al.CLElement_.call<val>("applyTransformFn",
                                              al.transformFn_, internalVal);
        al.peer_->setVal(product);
    } else {
        al.peer_->setVal(internalVal);
    }

    clean_ = true;
}

void clarity::ControlNetworkNode::pushValToPeers(
    ControlNetworkNode *excludedPeer) {
    if (excludedPeer == nullptr) {
        for (auto peer : peers_) {
            pushValToPeer(peer);
        }
    } else {
        for (auto peer : peers_) {
            if (peer.peer_ != excludedPeer) {
                pushValToPeer(peer);
            }
        }
    }
}

void clarity::ControlNetworkNode::addPeer(ControlNetworkNode::ActiveLink al,
                                          bool alreadyAdded) {
    peers_.push_back(al);
    if (alreadyAdded) {
        return;
    }
    al.peer_->addPeer(
        ActiveLink(this, cle_.call<val>("invertValue", al.scalarConst_)), true);
}

inline void ControlNetworkNode::setAttribute(const string &attr,
                                             const val &value) {
    val domElement = cle_["domElement"];
    domElement.call<void>("setAttribute", attr, value);
}

inline void ControlNetworkNode::setAttributes(const map<string, val> &attrs) {
    for (auto [attrName, value] : attrs) {
        setAttribute(attrName, value);
    }
}

inline void ControlNetworkNode::setStoredValueType(CppType cppType) {
    storedValueType_ = cppType;
    cle_.set("cpptype", cppType);
}

inline void ControlNetworkNode::addEventListenerByName(
    const string &eventName, const string &callbackName) {
    cle_.call<void>("addEventListenerById", eventName, callbackName);
}

inline void ControlNetworkNode::addJSEventListener(const string &eventName,
                                                   val eventCallback) {
    cle_.call<void>("addEventListener", eventName, eventCallback);
}


