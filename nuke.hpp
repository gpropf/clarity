#ifndef nuke_hpp
#define nuke_hpp

#include "clarity.hpp"
#include "ControlNetworkNode.hpp"
#include "ModelNode.hpp"
#include "WebNode.hpp"
#include "WebElemNode.hpp"
#include "WebAttrNode.hpp"
#include "ButtonElement.hpp"


/**
 * @brief A simple model of a nuclear power plant.
 *
 */
class NukeModel //: public clarity::UpdateAble
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

  NukeModel(double s, double delta) : s_(s), delta_(delta)
  {
    printState();
  }

  private:
  friend class NukeControl;
  double s_, delta_;
  
  // int i_;
};

/**
 * @brief Control Panel for the nuclear power plant
 *
 */
class NukeControl : public clarity::WebElemNode
{

public:
  NukeControl(const std::string &name, const std::string &tag, const CppType anyvalPtrType);

  string *buttonText_;
  clarity::WebElemNode *mainDiv_;
  clarity::WebElemNode *applyButton_;
  clarity::ModelNode *buttonModel_;
};

#endif
