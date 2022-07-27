
//#include "clarity.hpp"
#include "nuke.hpp"

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

NukeModel::NukeModel(double s, double delta) : s_(s), delta_(delta)
{
  printState();
  controlRodSetting_ = 0.5;
  coreToWaterHeatingConstant_ = 1.0;
  controlRodSettingNode_ = new ModelNode(CppType::Double);
  controlRodSettingNode_->splicePtrs(&controlRodSetting_);
  coreToWaterHeatingConstantNode_ = new ModelNode(CppType::Double);
  coreToWaterHeatingConstantNode_->splicePtrs(&coreToWaterHeatingConstant_);
}

NukeControl::NukeControl(const string &name, const string &tag,
                         clarity::CppType anyvalPtrType)
    : clarity::WebElemNode(name, tag, anyvalPtrType)
{
  mainDiv_ = new clarity::WebElemNode("nukediv", "div", clarity::CppType::NoData);
  coreToWaterHeatingConstant_ = new clarity::WebElemNode("coreToWaterHeatingConstant_", "input",
                                                         clarity::CppType::Double);
  clarity::CompoundElement *cpe_ = new clarity::CompoundElement("cpe", "div",
                                                                  clarity::CppType::Double, coreToWaterHeatingConstant_);
  applyButton_ = new clarity::ButtonElement("applyButton_", "button", clarity::CppType::String);

  //mainDiv_->appendChild(applyButton_);
  mainDiv_->appendChild(cpe_);
}

NukeControl::NukeControl(const string &name, const string &tag,
                         clarity::CppType anyvalPtrType, NukeModel &nm)
    : NukeControl(name, tag, anyvalPtrType)
{

  controlRodSetting_ = new clarity::WebElemNode("controlRodSetting_", "input",
                                                clarity::CppType::Double);
  nm.controlRodSettingNode_->addPeer(controlRodSetting_);
  nm.controlRodSettingNode_->pushValToPeers(nm.controlRodSettingNode_);
  nm.coreToWaterHeatingConstantNode_->addPeer(coreToWaterHeatingConstant_);
  nm.coreToWaterHeatingConstantNode_->pushValToPeers(nm.coreToWaterHeatingConstantNode_);
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

  double *n = new double(33);
  clarity::ModelNode *nm = new clarity::ModelNode(clarity::CppType::Double);
  nm->splicePtrs(n);
  clarity::WebElemNode *maindiv = new clarity::WebElemNode("maindiv", "div",
                                                           clarity::CppType::NoData);
  clarity::WebElemNode *ncntr = new clarity::WebElemNode("numerator", "input",
                                                         clarity::CppType::Double);
  clarity::WebElemNode *nslider = new clarity::WebElemNode("numerator", "input",
                                                           clarity::CppType::Double);
  clarity::WebElemNode *svgarea = new clarity::WebElemNode("svgarea", "svg",
                                                           clarity::CppType::NoData);
  clarity::WebElemNode *cir1 = new clarity::WebElemNode("cir1", "circle",
                                                        clarity::CppType::Double);

  clarity::WebAttrNode *cir1Radius = new clarity::WebAttrNode("r",
                                                              clarity::CppType::Double, cir1);

  double *d = new double(5.6);
  clarity::WebElemNode *doubleTest = new clarity::WebElemNode("d-test", "input",
                                                              clarity::CppType::Double);
  //doubleTest->setAnyvalPtrType(CppType::Double);
  doubleTest->setAttribute("type", val("text"));
  clarity::ModelNode *mdtest = new clarity::ModelNode(CppType::Double);
  mdtest->splicePtrs(d);
  mdtest->addPeer(doubleTest);
  mdtest->pushValToPeers(mdtest);
  NukeModel *nmod = new NukeModel(1, 5);
  NukeControl *nc = new NukeControl("nuke_control", "div",
                                    clarity::CppType::NoData, *nmod);
  // nc->applyButton_ = new clarity::ButtonElement("nuke_button", "button", clarity::CppType::String);
  svgarea->setAttribute("width", val("300"));
  svgarea->setAttribute("height", val("200"));
  svgarea->setAttribute("viewBox", val("0 0 200 200"));
  svgarea->setAttribute("style", val("border: 1px solid black"));
  cir1->setAttribute("cx", val(100));
  cir1->setAttribute("cy", val(100));
  cir1->setAttribute("stroke", val("green"));
  cir1->setAttribute("fill", val("lightblue"));
  cir1->setAttribute("stroke-width", val(4));
  cir1->setAttribute("r", val(80));
  nm->addPeer(cir1Radius);
  nslider->setAttribute("type", val("range"));
  maindiv->appendChild(ncntr);
  maindiv->appendChild(nslider);
  maindiv->appendChild(doubleTest);
  maindiv->appendChild(nc);
  svgarea->appendChild(cir1);
  ncntr->setAttribute("type", val("text"));
  nm->addPeer(ncntr);
  nm->addPeer(nslider);
  ncntr->addEventListenerByName("change", "printNetworkState");
  nslider->addEventListenerByName("change", "printNetworkState");
  maindiv->appendChild(svgarea);

  std::string *buttonText = new std::string("CLICK ME!");

  nc->buttonText_ = new string("CLICK ME TOO!");
  nc->buttonModel_ = new clarity::ModelNode(clarity::CppType::String);
  nc->buttonModel_->splicePtrs(nc->buttonText_);
  nc->buttonModel_->addPeer(nc->applyButton_);
  nc->buttonModel_->pushValToPeers(nc->buttonModel_);

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
