#include "ClarityNode.hpp"

using clarity::ClarityNode;

using std::string;

EMSCRIPTEN_BINDINGS(clarity) {
    class_<ClarityNode>("ClarityNode")
        //.function("toggleClean", &ClarityNode::toggleClean,
        // allow_raw_pointers()) .function("getVal", &ClarityNode::getVal,
        // allow_raw_pointers()) .function("pushValToPeers",
        //&ClarityNode::pushValToPeers, allow_raw_pointers())
        .class_function("pushValToPeersById", &ClarityNode::pushValToPeersById,
                        allow_raw_pointers())
        .class_function("updateNodeFromDomById", &ClarityNode::updateNodeFromDomById,
                        allow_raw_pointers())
        // .function("pullValFromPeers", &ClarityNode::pullValFromPeers,
        //           allow_raw_pointers())

        .function("getId", &ClarityNode::getId, allow_raw_pointers())
        .function("getName", &ClarityNode::getName, allow_raw_pointers())
        .function("getTag", &ClarityNode::getTag, allow_raw_pointers())
        .function("getBoundField", &ClarityNode::getBoundField, allow_raw_pointers())
        .function("getNodeTypeCode", &ClarityNode::getNodeTypeCode, allow_raw_pointers())
        // .class_function("pullValFromPeersById",
        //                 &ClarityNode::pullValFromPeersById,
        //                 allow_raw_pointers())
        .class_function("getClarityNodeById", &ClarityNode::getClarityNodeById,
                        allow_raw_pointers())
        //.class_function("markNodeDirtyById", &ClarityNode::markNodeDirtyById,
        //                allow_raw_pointers())
        //.constructor<string, string, const CppType>(allow_raw_pointers())
        .property("tag", &ClarityNode::getTag)
        .property("id", &ClarityNode::getId)
        // .property("storedValueType", &ClarityNode::getStoredValueType,
        //           &ClarityNode::setStoredValueType)
        .class_function("runCallbackById", &ClarityNode::runCallbackById, allow_raw_pointers());

    class_<CanvasGrid<unsigned char>>("CanvasGrid")
        .function("setValXY", &CanvasGrid<unsigned char>::setValXY, allow_raw_pointers())
        .function("doNothing", &CanvasGrid<unsigned char>::doNothing, allow_raw_pointers());

    class_<HybridNode<int>>("HybridNode_i")
        .class_function("listNodes_int", &HybridNode<int>::listNodes, allow_raw_pointers())
        .function("updateNodeFromDom", &HybridNode<int>::updateNodeFromDom, allow_raw_pointers());

    //.class_function("listNodes_double", &HybridNode<double>::listNodes, allow_raw_pointers());

    class_<HybridNode<double>>("HybridNode_d")
        //.class_function("listNodes_int2", &HybridNode<int>::listNodes, allow_raw_pointers())
        .function("updateNodeFromDom", &HybridNode<double>::updateNodeFromDom, allow_raw_pointers())
        .class_function("listNodes_double", &HybridNode<double>::listNodes, allow_raw_pointers());

    class_<HybridNode<string>>("HybridNode_str")
        // The type code that comes back from typeid.name() is literally
        // 'NSt3__212basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE' so we're just going to
        // use _str as the suffix for this one.
        .function("doNothing", &HybridNode<string>::doNothing, allow_raw_pointers())
        .function("updateNodeFromDom", &HybridNode<string>::updateNodeFromDom,
                  allow_raw_pointers());
    // .class_function("listNodes_int3", &HybridNode<int>::listNodes, allow_raw_pointers())
    // .class_function("listNodes_double3", &HybridNode<double>::listNodes,
    // allow_raw_pointers());

    class_<HybridNode<unsigned char>>("HybridNode_h")
        .function("updateNodeFromDom", &HybridNode<unsigned char>::updateNodeFromDom,
                  allow_raw_pointers())
        .function("doNothing", &HybridNode<unsigned char>::doNothing, allow_raw_pointers());

    class_<SimpleSelect<int>>("SimpleSelect")
        .function("getSelectedLabel", &SimpleSelect<int>::getSelectedLabel, allow_raw_pointers())
        .function("doNothing", &Checkbox<void>::doNothing, allow_raw_pointers());

    class_<Select<vector<pair<int, string>>>>("Select_vector_pair_int_string")
        .function("doNothing", &Select<void>::doNothing, allow_raw_pointers());

    class_<Checkbox<bool>>("Checkbox_bool")
        .function("doNothing", &Checkbox<void>::doNothing, allow_raw_pointers());

    //

    enum_<ClarityNode::AttachmentMode>("AttachmentMode")
        .value("NEW", ClarityNode::AttachmentMode::NEW)
        .value("REPLACE_ID", ClarityNode::AttachmentMode::REPLACE_ID)
        .value("REPLACE_NAME", ClarityNode::AttachmentMode::REPLACE_NAME)
        .value("ATTACH_ID", ClarityNode::AttachmentMode::ATTACH_ID)
        .value("ATTACH_NAME", ClarityNode::AttachmentMode::ATTACH_NAME);
}
