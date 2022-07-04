#include "clarity.hpp"

/**
 * @brief A simple model that just changes its internal state over time to
 * act as a testbed for the Clarity library.
 *
 */
class ToyModel //: public clarity::UpdateAble
{
public:
  void printState() const
  {
    cout << "MODEL STATE: s = " << s_ << ", delta = " << delta_ << endl;
  }

  void update() {}

  void iterate()
  {
    s_ += delta_;
    delta_ *= 0.95;
    printState();
  }

  ToyModel(double s, double delta) : s_(s), delta_(delta)
  {
    printState();
  }

  // private:
  double s_, delta_;
  // int i_;
};

/**
 * @brief A simble example of a complex control made up of simpler ones.
 *
 */
class ToyControl : public clarity::WebElemNode
{

public:
  ToyControl(const string &name, const string &tag, const CppType anyvalPtrType) : WebElemNode(name, tag, anyvalPtrType)
  {
    // mainDiv_ = new clarity::WebElement("mainDiv_", "div", CppType::NoData);
    inputA_ = new clarity::WebElemNode("inputA_", "input", CppType::Double);
    inputB_ = new clarity::WebElemNode("inputB_", "input", CppType::Double);
    sliderA_ = new clarity::WebElemNode("sliderA_", "input", CppType::Double);
    applyButton_ = new clarity::ButtonElement("applyButton_", "button", CppType::String);
    inputA_->setAttribute("type", val("text"));
    inputB_->setAttribute("type", val("text"));
    sliderA_->setAttribute("type", val("range"));
    this->appendChild(inputA_);
    this->appendChild(inputB_);
    this->appendChild(applyButton_);
    this->appendChild(sliderA_);
    // this->appendChild(*mainDiv_);
  }

  clarity::WebElemNode *mainDiv_;
  clarity::WebElemNode *inputA_;
  clarity::WebElemNode *sliderA_;
  clarity::WebElemNode *inputB_;
  clarity::WebElemNode *applyButton_;
};

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

  int *i = new int(100);
  double *n = new double(11);
  clarity::ModelNode *nm = new clarity::ModelNode(clarity::ControlNetworkNode::CppType::Double);
  nm->splicePtrs(n);
  clarity::WebElemNode *ratioDiv = new clarity::WebElemNode("maindiv", "div",
                                                            clarity::ControlNetworkNode::CppType::NoData);
  clarity::WebElemNode *ncntr = new clarity::WebElemNode("numerator", "input",
                                                         clarity::ControlNetworkNode::CppType::Double);
  clarity::WebElemNode *nslider = new clarity::WebElemNode("numerator", "input",
                                                           clarity::ControlNetworkNode::CppType::Double);
  clarity::WebElemNode *svgarea = new clarity::WebElemNode("svgarea", "svg", clarity::ControlNetworkNode::CppType::NoData);
  clarity::WebElemNode *cir1 = new clarity::WebElemNode("cir1", "circle", clarity::ControlNetworkNode::CppType::Double);

  clarity::WebAttrNode *cir1Radius = new clarity::WebAttrNode("r", clarity::ControlNetworkNode::CppType::Double, cir1);

  //<circle cx="50" cy="50" r="40" stroke="green" stroke-width="4" fill="yellow" />
  // svgarea->setAttribute("xmlns", val("http://www.w3.org/2000/svg"));
  svgarea->setAttribute("width", val("300"));
  svgarea->setAttribute("height", val("200"));
  svgarea->setAttribute("viewBox", val("0 0 200 200"));
  // svgarea->setAttribute("xmlns:xlink", val("http://www.w3.org/1999/xlink"));
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
  ToyControl *tc = new ToyControl("tc1", "div", clarity::ControlNetworkNode::CppType::NoData);
  ToyModel *tm = new ToyModel(0, 1);
  string *buttonText = new string("CLICK ME!");

  clarity::WebElemNode::callbackMap["iterateModel"] = [=]
  {
    cout << "callbackMap[\"iterateModel\"]\n";
    tm->iterate();
    tm->printState();
    tc->updateViewFromModel();
    tc->updateModelFromView();
  };

  clarity::WebElemNode::callbackMap["syncModelView"] = [=]
  {
    cout << "callbackMap[\"syncModelView\"]\n";
    tm->printState();
    tc->updateModelFromView();
    tc->updateViewFromModel();
  };

  clarity::WebElemNode::callbackMap["printNetworkState"] = [=]
  {
    cout << "callbackMap[\"printNetworkState\"]\n";    
    //ncntr->printState();
    
  };

  clarity::WebElemNode::callbackMap["tick"] = [=]
  {
    cout << "callbackMap[\"tick\"]\n";  
    (*n)++;
    nm->pushValToPeers(nm);  
    //ncntr->printState();
  };

  tc->inputA_->splicePtrs(&tm->delta_);
  tc->inputB_->splicePtrs(&tm->s_);
  tc->sliderA_->splicePtrs(&tm->s_);
  tc->applyButton_->splicePtrs(buttonText);
  tc->sliderA_->addEventListenerByName("change", "syncModelView");
  tc->applyButton_->addEventListenerByName("click", "iterateModel");

  tc->updateViewFromModel();
  printf("Setup complete!\n");
  return 0;
}
