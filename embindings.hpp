using clarity::ControlNetworkNode;
using clarity::WebElemNode;
using std::string;

EMSCRIPTEN_BINDINGS(clarity)
{
  class_<ControlNetworkNode>("ControlNetworkNode")
      .function("toggleClean", &ControlNetworkNode::toggleClean, allow_raw_pointers())      
      .function("pushValToPeersThruAL", &ControlNetworkNode::pushValToPeersThruAL, allow_raw_pointers())
      .function("getVal", &ControlNetworkNode::getVal, allow_raw_pointers())      
      .class_function("pushValToPeersThruALById", &ControlNetworkNode::pushValToPeersThruALById, allow_raw_pointers())
      .class_function("getCLElementById", &ControlNetworkNode::getCLElementById, allow_raw_pointers())
      .class_function("markNodeDirtyById", &ControlNetworkNode::markNodeDirtyById, allow_raw_pointers());

  enum_<CppType>("WebElementCppType")
      .value("Int", CppType::Int)
      .value("Float", CppType::Float)
      .value("Double", CppType::Double)
      .value("String", CppType::String)
      .value("NoData", CppType::NoData);
}

EMSCRIPTEN_BINDINGS(WebElemNode)
{

  class_<WebElemNode>("WebElement")
      .constructor<string, string, const CppType>(allow_raw_pointers())
      .property("tag", &WebElemNode::getTag)
      .property("id", &WebElemNode::getId)
      .property("storedValueType", &WebElemNode::getStoredValueType, &WebElemNode::setStoredValueType) 
      .class_function("runCallbackById", &WebElemNode::runCallbackById, allow_raw_pointers());
}