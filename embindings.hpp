using clarity::ClarityNodeBase;

using std::string;

EMSCRIPTEN_BINDINGS(clarity) {
    class_<ClarityNodeBase>("ClarityNode")
        .function("toggleClean", &ClarityNodeBase::toggleClean,
                  allow_raw_pointers())
        //.function("getVal", &ClarityNode::getVal, allow_raw_pointers())
        .function("pushValToPeers", &ClarityNodeBase::pushValToPeers,
                  allow_raw_pointers())
        .class_function("pushValToPeersById", &ClarityNodeBase::pushValToPeersById,
                        allow_raw_pointers())
        .class_function("js2datumWithPushToPeersById",
                        &ClarityNodeBase::js2datumWithPushToPeersById,
                        allow_raw_pointers())
        // .function("pullValFromPeers", &ClarityNode::pullValFromPeers,
        //           allow_raw_pointers())
        // .class_function("pullValFromPeersById",
        //                 &ClarityNode::pullValFromPeersById,
        //                 allow_raw_pointers())
        .class_function("getCLElementById", &ClarityNodeBase::getCLElementById,
                        allow_raw_pointers())
        .class_function("markNodeDirtyById", &ClarityNodeBase::markNodeDirtyById,
                        allow_raw_pointers())
        .constructor<string, string>(allow_raw_pointers())
        .property("tag", &ClarityNodeBase::getTag)
        .property("id", &ClarityNodeBase::getId)
        // .property("storedValueType", &ClarityNode::getStoredValueType,
        //           &ClarityNode::setStoredValueType)
        .class_function("runCallbackById", &ClarityNodeBase::runCallbackById,
                        allow_raw_pointers());

    class_<TranslatorCanvasGrid<unsigned char>>("TranslatorCanvasGrid")
        .function("setValXY", &TranslatorCanvasGrid<unsigned char>::setValXY,
                  allow_raw_pointers())
        .function("datum2js", &TranslatorCanvasGrid<unsigned char>::datum2js,
                  allow_raw_pointers());

    enum_<CppType>("CppType")
        .value("Int", CppType::Int)
        .value("Float", CppType::Float)
        .value("Double", CppType::Double)
        .value("String", CppType::String)
        .value("JSFunc", CppType::JSFunc)
        .value("NoData", CppType::NoData);
}
