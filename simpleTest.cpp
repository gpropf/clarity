
//#include "clarity.hpp"
#include "simpleTest.hpp"

#include "embindings.hpp"

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

  int i = 5;
  float f = 9.4;

  vector<TBase *> v;

  Ttest<int> *tti = new Ttest(&i);
  Ttest<float> *ttf = new Ttest(&f);

  v.push_back(tti);
  v.push_back(ttf);

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
  double *d = new double(3.14159);
  double *dd = new double(2.5);
  DynamicValue dvd(d, CppType::Double);

  clarity::ModelNode<double> *nm = new clarity::ModelNode<double>(n);

  clarity::ModelNode<double> *ddynm = new clarity::ModelNode<double>(dd);

  clarity::WebElemNode *maindiv = new clarity::WebElemNode("maindiv", "div",
                                                           clarity::CppType::NoData);
  clarity::WebElemNode *ncntr = new clarity::WebElemNode("n", "input",
                                                         clarity::CppType::Double);

  clarity::WebElemNode *dcntr = new clarity::WebElemNode("d", "input", clarity::CppType::Double);

  ncntr->setAttribute("type", val("text"));
  dcntr->setAttribute("type", val("text"));

  maindiv->appendChild(ncntr);
  maindiv->appendChild(dcntr);

  nm->addALPeer(clarity::ControlNetworkNode::ActiveLink(ncntr, val(10)));
  // ddynm->addPeer(dcntr);
  ddynm->addALPeer(clarity::ControlNetworkNode::ActiveLink(dcntr, val(10)));
  ddynm->pushValToPeersThruAL();

  ncntr->addEventListenerByName("change", "printNetworkState");
  nm->pushValToPeersThruAL();

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
