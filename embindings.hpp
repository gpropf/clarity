using clarity::ClarityNode;

using std::string;

EMSCRIPTEN_BINDINGS(clarity) {
    class_<ClarityNode>("ClarityNode")
        .function("toggleClean", &ClarityNode::toggleClean,
                  allow_raw_pointers())
        .function("pushValToPeers2", &ClarityNode::pushValToPeers2,
                  allow_raw_pointers())
        .function("getVal", &ClarityNode::getVal, allow_raw_pointers())
        .class_function("pushValToPeersById2",
                        &ClarityNode::pushValToPeersById2,
                        allow_raw_pointers())
        .class_function("getCLElementById",
                        &ClarityNode::getCLElementById,
                        allow_raw_pointers())
        .class_function("markNodeDirtyById",
                        &ClarityNode::markNodeDirtyById,
                        allow_raw_pointers())
        .constructor<string, string, const CppType>(allow_raw_pointers())
        .property("tag", &ClarityNode::getTag)
        .property("id", &ClarityNode::getId)
        .property("storedValueType", &ClarityNode::getStoredValueType,
                  &ClarityNode::setStoredValueType)
        .class_function("runCallbackById", &ClarityNode::runCallbackById,
                        allow_raw_pointers());

    enum_<CppType>("CppType")
        .value("Int", CppType::Int)
        .value("Float", CppType::Float)
        .value("Double", CppType::Double)
        .value("String", CppType::String)
        .value("JSFunc", CppType::JSFunc)
        .value("NoData", CppType::NoData);
  
}

