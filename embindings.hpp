using clarity::ControlNetworkNode;

using std::string;

EMSCRIPTEN_BINDINGS(clarity) {
    class_<ControlNetworkNode>("ControlNetworkNode")
        .function("toggleClean", &ControlNetworkNode::toggleClean,
                  allow_raw_pointers())
        .function("pushValToPeers", &ControlNetworkNode::pushValToPeers,
                  allow_raw_pointers())
        .function("getVal", &ControlNetworkNode::getVal, allow_raw_pointers())
        .class_function("pushValToPeersById",
                        &ControlNetworkNode::pushValToPeersById,
                        allow_raw_pointers())
        .class_function("getCLElementById",
                        &ControlNetworkNode::getCLElementById,
                        allow_raw_pointers())
        .class_function("markNodeDirtyById",
                        &ControlNetworkNode::markNodeDirtyById,
                        allow_raw_pointers())
        .constructor<string, string, const CppType>(allow_raw_pointers())
        .property("tag", &ControlNetworkNode::getTag)
        .property("id", &ControlNetworkNode::getId)
        .property("storedValueType", &ControlNetworkNode::getStoredValueType,
                  &ControlNetworkNode::setStoredValueType)
        .class_function("runCallbackById", &ControlNetworkNode::runCallbackById,
                        allow_raw_pointers());

    enum_<CppType>("CppType")
        .value("Int", CppType::Int)
        .value("Float", CppType::Float)
        .value("Double", CppType::Double)
        .value("String", CppType::String)
        .value("NoData", CppType::NoData);
}

