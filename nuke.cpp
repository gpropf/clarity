
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

    

    enum_<ControlNetworkNode::CppType>("WebElementCppType")
        .value("Int", ControlNetworkNode::CppType::Int)
        .value("Float", ControlNetworkNode::CppType::Float)
        .value("Double", ControlNetworkNode::CppType::Double)
        .value("String", ControlNetworkNode::CppType::String)
        .value("NoData", ControlNetworkNode::CppType::NoData);
  }


  EMSCRIPTEN_BINDINGS(WebElemNode)
  {
    

    class_<WebElemNode>("WebElement")
        .constructor<string, string, const ControlNetworkNode::CppType>(allow_raw_pointers())
        .property("tag", &WebElemNode::getTag)
       .property("id", &WebElemNode::getId)
        .property("anyvalPtrType", &WebElemNode::getAnyvalPtrType, &WebElemNode::setAnyvalPtrType)
        //.function("updateModelFromView", &WebElemNode::updateModelFromView)
        .function("splicePtrs", &WebElemNode::splicePtrs, allow_raw_pointers())

        //.class_function("updateModelFromViewById", &WebElemNode::updateModelFromViewById, allow_raw_pointers())
        .class_function("runCallbackById", &WebElemNode::runCallbackById, allow_raw_pointers());

    
  }



NukeControl::NukeControl(const string &name, const string &tag, clarity::ControlNetworkNode::CppType anyvalPtrType) 
: clarity::WebElemNode(name, tag, anyvalPtrType)
{
  inputA_ = new clarity::WebElemNode("inputA_", "input", CppType::Double);
  inputB_ = new clarity::WebElemNode("inputB_", "input", CppType::Double);
  sliderA_ = new clarity::WebElemNode("sliderA_", "input", CppType::Double);
  applyButton_ = new clarity::ButtonElement("applyButton_", "button", clarity::ControlNetworkNode::CppType::String);
  inputA_->setAttribute("type", val("text"));
  inputB_->setAttribute("type", val("text"));
  sliderA_->setAttribute("type", val("range"));
  this->appendChild(inputA_);
  this->appendChild(inputB_);
  this->appendChild(applyButton_);
  this->appendChild(sliderA_);
}

// clarity::WebElemNode *mainDiv_;
// clarity::WebElemNode *inputA_;
// clarity::WebElemNode *sliderA_;
// clarity::WebElemNode *inputB_;
// clarity::WebElemNode *applyButton_;


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
  clarity::ModelNode *nm = new clarity::ModelNode(clarity::ControlNetworkNode::CppType::Double);
  nm->splicePtrs(n);
  clarity::WebElemNode *ratioDiv = new clarity::WebElemNode("maindiv", "div",
                                                            clarity::ControlNetworkNode::CppType::NoData);
  clarity::WebElemNode *ncntr = new clarity::WebElemNode("numerator", "input",
                                                         clarity::ControlNetworkNode::CppType::Double);
  clarity::WebElemNode *nslider = new clarity::WebElemNode("numerator", "input",
                                                           clarity::ControlNetworkNode::CppType::Double);
  clarity::WebElemNode *svgarea = new clarity::WebElemNode("svgarea", "svg",
                                                           clarity::ControlNetworkNode::CppType::NoData);
  clarity::WebElemNode *cir1 = new clarity::WebElemNode("cir1", "circle",
                                                        clarity::ControlNetworkNode::CppType::Double);

  clarity::WebAttrNode *cir1Radius = new clarity::WebAttrNode("r",
                                                              clarity::ControlNetworkNode::CppType::Double, cir1);

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
  ratioDiv->appendChild(ncntr);
  ratioDiv->appendChild(nslider);
  svgarea->appendChild(cir1);
  ncntr->setAttribute("type", val("text"));
  nm->addPeer(ncntr);
  nm->addPeer(nslider);
  ncntr->addEventListenerByName("change", "printNetworkState");
  nslider->addEventListenerByName("change", "printNetworkState");
  ratioDiv->appendChild(svgarea);
  NukeControl *tc = new NukeControl("tc1", "div", clarity::ControlNetworkNode::CppType::NoData);
  NukeModel *tm = new NukeModel(0, 1);
  std::string *buttonText = new std::string("CLICK ME!");
  nm->pushValToPeers(nm);

  clarity::WebElemNode::callbackMap["iterateModel"] = [=]
  {
    cout << "callbackMap[\"iterateModel\"]\n";
    tm->iterate();
    tm->printState();
    // tc->updateViewFromModel();
    // tc->updateModelFromView();
  };

  clarity::WebElemNode::callbackMap["syncModelView"] = [=]
  {
    cout << "callbackMap[\"syncModelView\"]\n";
    tm->printState();
    // tc->updateModelFromView();
    // tc->updateViewFromModel();
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

  tc->inputA_->splicePtrs(&tm->delta_);
  tc->inputB_->splicePtrs(&tm->s_);
  tc->sliderA_->splicePtrs(&tm->s_);
  tc->applyButton_->splicePtrs(buttonText);
  tc->sliderA_->addEventListenerByName("change", "syncModelView");
  tc->applyButton_->addEventListenerByName("click", "iterateModel");

  // tc->updateViewFromModel();
  printf("Setup complete!\n");
  return 0;
}