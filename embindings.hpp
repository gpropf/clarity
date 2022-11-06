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
        .function("setValXY", &CanvasGrid<unsigned char>::setValXY, allow_raw_pointers());

    class_<HybridNode<int>>("HybridNode")
        .class_function("listNodes_int", &HybridNode<int>::listNodes, allow_raw_pointers())
        .class_function("listNodes_double", &HybridNode<double>::listNodes, allow_raw_pointers());
        //.function("setDOMVal", &HybridNode<int>::setDOMVal, allow_raw_pointers());


    class_<SimpleSelect<int>>("SimpleSelect")
        .function("setCppVal", &SimpleSelect<int>::setCppVal, allow_raw_pointers());

    enum_<ClarityNode::AttachmentMode>("AttachmentMode")
        .value("NEW", ClarityNode::AttachmentMode::NEW)
        .value("REPLACE_ID", ClarityNode::AttachmentMode::REPLACE_ID)
        .value("REPLACE_NAME", ClarityNode::AttachmentMode::REPLACE_NAME)
        .value("ATTACH_ID", ClarityNode::AttachmentMode::ATTACH_ID)
        .value("ATTACH_NAME", ClarityNode::AttachmentMode::ATTACH_NAME);
}
