
//#include "clarity.hpp"
#include "nuke.hpp"

#include "embindings.hpp"

NukeModel::NukeModel(double s, double delta) : s_(s), delta_(delta)
{
  printState();
  controlRodSetting_ = 0.5;
  coreToWaterHeatingConstant_ = 2.0;
  controlRodSettingNode_ = new ModelNode<double>(&controlRodSetting_, CppType::Double);
  coreToWaterHeatingConstantNode_ = new ModelNode<double>(&coreToWaterHeatingConstant_, CppType::Double);
}

NukeControl::NukeControl(const string &name, const string &tag,
                         clarity::CppType anyvalPtrType)
    : clarity::WebElemNode(name, tag, anyvalPtrType)
{
  mainDiv_ = new clarity::WebElemNode("nukediv", "div", clarity::CppType::NoData);
  coreToWaterHeatingConstant_ = new clarity::WebElemNode("coreToWaterHeatingConstant_", "input",
                                                         clarity::CppType::Double);
  coreToWaterHeatingConstant_->setAttribute("type", val("text"));
  clarity::LabelledInput *coreToWaterHeatingConstant = new clarity::LabelledInput("coreToWaterHeatingConstant_",
                                                                                  "div",
                                                                                  clarity::CppType::Double,
                                                                                  coreToWaterHeatingConstant_);

  controlRodSetting_ = new clarity::WebElemNode("controlRodSetting_", "input",
                                                clarity::CppType::Double);
  controlRodSetting_->setAttribute("type", val("text"));
  clarity::LabelledInput *controlRodSetting = new clarity::LabelledInput("controlRodSetting_",
                                                                         "div",
                                                                         clarity::CppType::Double,
                                                                         controlRodSetting_);

  applyButton_ = new clarity::ButtonElement("applyButton_", "button", clarity::CppType::String);

  // mainDiv_->appendChild(applyButton_);
  mainDiv_->appendChild(controlRodSetting);
  mainDiv_->appendChild(coreToWaterHeatingConstant);
  
}

NukeControl::NukeControl(const string &name, const string &tag,
                         clarity::CppType anyvalPtrType, NukeModel &nm)
    : NukeControl(name, tag, anyvalPtrType)
{

  //controlRodSetting_ = new clarity::WebElemNode("controlRodSetting_", "input",
  //                                              clarity::CppType::Double);
  // nm.controlRodSettingNode_->addPeer(controlRodSetting_);
  nm.controlRodSettingNode_->addALPeer(ActiveLink(controlRodSetting_, val(1)));
  nm.controlRodSettingNode_->pushValToPeersThruAL(nm.controlRodSettingNode_);
  // nm.coreToWaterHeatingConstantNode_->addPeer(coreToWaterHeatingConstant_);
  nm.coreToWaterHeatingConstantNode_->addALPeer(ActiveLink(coreToWaterHeatingConstant_, val(1)));
  nm.coreToWaterHeatingConstantNode_->pushValToPeersThruAL(nm.coreToWaterHeatingConstantNode_);
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

  double *n = new double(5);
  clarity::ModelNode<double> *nm = new clarity::ModelNode(n, clarity::CppType::Double);

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
  // doubleTest->setAnyvalPtrType(CppType::Double);
  doubleTest->setAttribute("type", val("text"));
  clarity::ModelNode<double> *mdtest = new clarity::ModelNode(d, CppType::Double);

  // mdtest->addPeer(doubleTest);
  // mdtest->pushValToPeers(mdtest);
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

  using ActiveLink = ControlNetworkNode::ActiveLink;

  // nm->addPeer(cir1Radius);

  nm->addALPeer(ActiveLink(cir1Radius, val(1)));

  nslider->setAttribute("type", val("range"));
  maindiv->appendChild(ncntr);
  maindiv->appendChild(nslider);
  maindiv->appendChild(doubleTest);
  maindiv->appendChild(nc);
  svgarea->appendChild(cir1);
  ncntr->setAttribute("type", val("text"));

  // nm->addPeer(ncntr);
  nm->addALPeer(ActiveLink(ncntr, val(10)));
  // nm->addPeer(nslider);
  nm->addALPeer(ActiveLink(nslider, val(1)));

  ncntr->addEventListenerByName("change", "printNetworkState");
  nslider->addEventListenerByName("change", "printNetworkState");
  maindiv->appendChild(svgarea);

  std::string *buttonText = new std::string("CLICK ME!");

  nc->buttonText_ = new string("CLICK ME TOO!");
  nc->buttonModel_ = new clarity::ModelNode<string>(nc->buttonText_, clarity::CppType::String);

  nc->buttonModel_->addALPeer(ActiveLink(nc->applyButton_));
  nc->buttonModel_->pushValToPeersThruAL(nc->buttonModel_);

  // nm->pushValToPeers(nm);
  nm->pushValToPeersThruAL(nm);

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
    nm->pushValToPeersThruAL(nm);
  };

  printf("Setup complete!\n");
  return 0;
}
