//#include "clarity.hpp"
#include "ControlNetworkNode.hpp"

bool ControlNetworkNode::appendChild(ControlNetworkNode *child)
{
    children_.push_back(child);
    cle_.call<void>("appendChild", child->getCLE());
    return true; // FIXME: need to check for duplicate ids.
}

clarity::ControlNetworkNode::ActiveLink::ActiveLink(ControlNetworkNode *peer, val scalarConst)
    : peer_(peer), scalarConst_(scalarConst)
{    
    transformFn_ = CLElement_.call<val>("generateTransformFn", scalarConst_);
}

inline string clarity::ControlNetworkNode::nodeStats() const
{
    string s = "Node name: " + name_ + ", Node id: " + to_string(id_) + ", Node type: " + typeid(*this).name() + "\n";
    return s;
}

void clarity::ControlNetworkNode::pushValToPeersThruALById(int id)
{
    ControlNetworkNode *cnn = getCLElementById(id);
    cnn->pushValToPeersThruAL(cnn);
}

inline void clarity::ControlNetworkNode::init()
{
    id_ = tm.getNext();
    ControlNetworkNode::switchboard[id_] = this;
}

inline clarity::ControlNetworkNode::ControlNetworkNode(const string &name, const CppType storedValueType)
    : name_(name),
      storedValueType_(storedValueType)
{
    init();
    cle_.set("cpptype", val(storedValueType));
}

inline clarity::ControlNetworkNode::ControlNetworkNode(const CppType storedValueType) : storedValueType_(storedValueType)
{
    init();
    cle_.set("cpptype", val(storedValueType));
}

void clarity::ControlNetworkNode::pushValToPeerThruAL(ActiveLink &al)
{
    if (clean_)
    {
    }

    val internalVal = getVal();

    if (internalVal.isNumber())
    {
        val product = al.CLElement_.call<val>("applyTransformFn", al.transformFn_, internalVal);
        al.peer_->setVal(product);
    }
    else
    {
        al.peer_->setVal(internalVal);
    }

    clean_ = true;
}

void clarity::ControlNetworkNode::pushValToPeersThruAL(ControlNetworkNode *excludedPeer)
{
    if (excludedPeer == nullptr)
    {
        for (auto alpeer : alpeers_)
        {
            pushValToPeerThruAL(alpeer);
        }
    }
    else
    {
        for (auto alpeer : alpeers_)
        {
            if (alpeer.peer_ != excludedPeer)
            {
                pushValToPeerThruAL(alpeer);
            }
        }
    }
}

void clarity::ControlNetworkNode::addALPeer(ControlNetworkNode::ActiveLink al, bool alreadyAdded)
{
    alpeers_.push_back(al);
    if (alreadyAdded)
    {
        return;
    }
    al.peer_->addALPeer(ActiveLink(this, cle_.call<val>("invertValue", al.scalarConst_)), true);
}

// clarity::ControlNetworkNode::