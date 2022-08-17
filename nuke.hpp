#ifndef nuke_hpp
#define nuke_hpp

#include "clarity.hpp"
#include "ControlNetworkNode.hpp"
#include "ModelNode.hpp"
#include "WebNode.hpp"
#include "WebElemNode.hpp"
#include "WebAttrNode.hpp"
#include "ButtonElement.hpp"
#include "CompoundElement.hpp"
#include "LabelledInput.hpp"
#include "CanvasElement.hpp"

using namespace clarity;
// using CppType = clarity::CppType;

/**
 * @brief A simple model of a nuclear power plant.
 *
 */
class NukeModel //: public clarity::UpdateAble
{
public:
  void printState() const
  {
    // cout<< "MODEL STATE: s = " << s_ << ", delta = " << delta_ << endl;
  }

  void update() {}

  double popFuelChunk();

  void makeFuelSupply(int numFuelChunks = 20);

  void iterate();

  NukeModel();

private:
  friend class NukeControl;
  
  double currentFuelChunk_;
  double controlRodSetting_;
  double activity_;
  double coreTemp_;
  double heatLoss_;
  double heatLoss_coef_;
  double coreToWaterHeatingConstant_;
  double waterTemp_;
  double turbineRPM_;
  double turbineInertia_;
  double powerOutput_;
  double totalPowerOutput_;

  //::ModelNode;

  ModelNode<double> *controlRodSettingNode_;
  ModelNode<double> *coreToWaterHeatingConstantNode_;
  ModelNode<double> *turbineInertiaNode_;

  vector<double> fuelSupply_;

  // int i_;
};

/**
 * @brief Control Panel for the nuclear power plant
 *
 */
class NukeControl : public clarity::WebElemNode
{

public:
  NukeControl(const std::string &name, const std::string &tag, const CppType storedValueType);
  NukeControl(const std::string &name, const std::string &tag, const CppType storedValueType, NukeModel &nm);
  string *buttonText_;
  clarity::WebElemNode *mainDiv_;
  clarity::WebElemNode *applyButton_;
  clarity::ModelNode<string> *buttonModel_;
  clarity::WebElemNode *controlRodSetting_;
  clarity::WebElemNode *coreToWaterHeatingConstant_;
  clarity::WebElemNode *turbineInertia_;
  // clarity::CompoundElement *cpe_;
};

#endif
