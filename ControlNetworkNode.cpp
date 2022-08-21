//#include "clarity.hpp"
#include "ControlNetworkNode.hpp"

clarity::ControlNetworkNode::ActiveLink::ActiveLink(ControlNetworkNode *peer, val scalarConst)
    : peer_(peer), scalarConst_(scalarConst)

{
    CLElement_ = val::global("CLElement");
    transformFn_ = CLElement_.call<val>("generateTransformFn", scalarConst_);
}

