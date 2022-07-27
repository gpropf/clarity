
//#include "clarity.hpp"
#include "simpleTest.hpp"

using clarity::ControlNetworkNode;
using clarity::WebElemNode;
using std::string;

EMSCRIPTEN_BINDINGS(clarity)
{
  class_<ControlNetworkNode>("ControlNetworkNode")
      .function("toggleClean", &ControlNetworkNode::toggleClean, allow_raw_pointers())
      .function("pushValToPeers", &ControlNetworkNode::pushValToPeers, allow_raw_pointers())
      .function("getVal", &ControlNetworkNode::getVal, allow_raw_pointers())
      .class_function("pushValToPeersById", &ControlNetworkNode::pushValToPeersById, allow_raw_pointers())
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
      .property("anyvalPtrType", &WebElemNode::getAnyvalPtrType, &WebElemNode::setAnyvalPtrType)
      //.function("updateModelFromView", &WebElemNode::updateModelFromView)
      .function("splicePtrs", &WebElemNode::splicePtrs, allow_raw_pointers())

      //.class_function("updateModelFromViewById", &WebElemNode::updateModelFromViewById, allow_raw_pointers())
      .class_function("runCallbackById", &WebElemNode::runCallbackById, allow_raw_pointers());
}

/**
 * @brief switchboard is where a map of all the WebElements is stored so that
 * they can be found by their id numbers.
 *
 */
// map<const int, clarity::WebElemNode *> clarity::WebElemNode::switchboard;
map<const int, clarity::ControlNetworkNode *> clarity::ControlNetworkNode::switchboard;

/**
 * @brief callbackMap holds C++ functions that can be triggered from JS
 * when events like a button press or timer tick require modification of
 * the C++ model state.
 *
 */
map<string, std::function<void()>> clarity::WebElemNode::callbackMap;
clarity::TicketMachine clarity::ControlNetworkNode::tm;

int main()
{
  val CLContext = val::global("CLElement");
  if (CLContext.as<bool>())
  {
    printf("Got CLElement!\n");
  }
  else
  {
    return -1;
  }

  double *n = new double(55.9);
  clarity::ModelNode *nm = new clarity::ModelNode(clarity::CppType::Double);
  nm->splicePtrs(n);
  clarity::WebElemNode *maindiv = new clarity::WebElemNode("maindiv", "div",
                                                           clarity::CppType::NoData);
  clarity::WebElemNode *ncntr = new clarity::WebElemNode("n", "input",
                                                         clarity::CppType::Double);
  ncntr->setAttribute("type", val("text"));

  maindiv->appendChild(ncntr);
  
  nm->addPeer(ncntr);
  
  ncntr->addEventListenerByName("change", "printNetworkState");
  nm->pushValToPeers(nm);

  clarity::WebElemNode::callbackMap["iterateModel"] = [=]
  {
    cout << "callbackMap[\"iterateModel\"]\n";
  };

  clarity::WebElemNode::callbackMap["syncModelView"] = [=]
  {
    cout << "callbackMap[\"syncModelView\"]\n";
  };

  clarity::WebElemNode::callbackMap["printNetworkState"] = [=]
  {
    cout << "callbackMap[\"printNetworkState\"]\n";
  };

  clarity::WebElemNode::callbackMap["tick"] = [=]
  {
    cout << "callbackMap[\"tick\"]\n";
    (*n)++;
    nm->pushValToPeers(nm);
  };

  printf("Setup complete!\n");
  return 0;
}