
//#include "clarity.hpp"
#include "nuke.hpp"

#include "embindings.hpp"

double NukeModel::popFuelChunk() {
    double fuelChunk = fuelSupply_.back();
    fuelSupply_.pop_back();
    return fuelChunk;
}

NukeModel::NukeModel() {
    printState();
    thermalEnergyLossRateConstant_ = 1.01;
    controlRodSetting_ = 0.5;
    coreToWaterHeatingConstant_ = 2.0;
    turbineInertia_ = 1.4;
    turbineInertiaNode_ =
        new ModelNode<double>(&turbineInertia_, CppType::Double);
    controlRodSettingNode_ =
        new ModelNode<double>(&controlRodSetting_, CppType::Double);
    coreToWaterHeatingConstantNode_ =
        new ModelNode<double>(&coreToWaterHeatingConstant_, CppType::Double);
    makeFuelSupply();
    currentFuelChunk_ = popFuelChunk();
}

void NukeModel::makeFuelSupply(int numFuelChunks) {
    for (int i = 0; i < numFuelChunks; i++) {
        cout << i << " ";
        fuelSupply_.push_back(1.0);
    }
}

NukeControl::NukeControl(const string &name, const string &tag,
                         clarity::CppType anyvalPtrType)
    : clarity::WebElemNode(name, tag, anyvalPtrType) {
    mainDiv_ =
        new clarity::WebElemNode("nukediv", "div", clarity::CppType::NoData);

    coreToWaterHeatingConstant_ = new clarity::WebElemNode(
        "coreToWaterHeatingConstant_", "input", clarity::CppType::Double);
    coreToWaterHeatingConstant_->setAttribute("type", val("text"));
    clarity::LabelledInput *coreToWaterHeatingConstant =
        new clarity::LabelledInput("coreToWaterHeatingConstant_", "div",
                                   clarity::CppType::Double,
                                   coreToWaterHeatingConstant_);

    controlRodSetting_ = new clarity::WebElemNode("controlRodSetting_", "input",
                                                  clarity::CppType::Double);
    controlRodSetting_->setAttribute("type", val("text"));
    clarity::LabelledInput *controlRodSetting = new clarity::LabelledInput(
        "controlRodSetting_", "div", clarity::CppType::Double,
        controlRodSetting_);

    turbineInertia_ = new clarity::WebElemNode("turbineInertia_", "input",
                                               clarity::CppType::Double);
    turbineInertia_->setAttribute("type", val("text"));
    clarity::LabelledInput *turbineInertia = new clarity::LabelledInput(
        "turbineInertia_", "div", clarity::CppType::Double, turbineInertia_);

    thermalEnergyLossRateConstant_ = new clarity::WebElemNode(
        "thermalEnergyLossRateConstant_", "input", clarity::CppType::Double);
    thermalEnergyLossRateConstant_->setAttribute("type", val("text"));
    clarity::LabelledInput *thermalEnergyLossRateConstant =
        new clarity::LabelledInput("thermalEnergyLossRateConstant_", "div",
                                   clarity::CppType::Double,
                                   thermalEnergyLossRateConstant_);

    applyButton_ = new clarity::ButtonElement("applyButton_", "button",
                                              clarity::CppType::String);

    clarity::CanvasElement *cvs =
        new clarity::CanvasElement(400, 300, string("cvs1"));

    mainDiv_->appendChild(cvs);
    mainDiv_->appendChild(controlRodSetting);
    mainDiv_->appendChild(coreToWaterHeatingConstant);
    mainDiv_->appendChild(turbineInertia);

    labelledInputTestVal_ = new double(3.14);
}

NukeControl::NukeControl(const string &name, const string &tag,
                         clarity::CppType anyvalPtrType, NukeModel &nm)
    : NukeControl(name, tag, anyvalPtrType) {
    nm.controlRodSettingNode_->addALPeer(
        ActiveLink(controlRodSetting_, val(1)));
    nm.controlRodSettingNode_->pushValToPeersThruAL(nm.controlRodSettingNode_);
    nm.coreToWaterHeatingConstantNode_->addALPeer(
        ActiveLink(coreToWaterHeatingConstant_, val(1)));
    nm.coreToWaterHeatingConstantNode_->pushValToPeersThruAL(
        nm.coreToWaterHeatingConstantNode_);
    nm.turbineInertiaNode_->addALPeer(ActiveLink(turbineInertia_, val(1)));
    nm.turbineInertiaNode_->pushValToPeersThruAL(nm.turbineInertiaNode_);
    thermalEnergyLossRateConstant_->installModelNode(
        &(nm.thermalEnergyLossRateConstant_));
}

map<const int, clarity::ControlNetworkNode *>
    clarity::ControlNetworkNode::switchboard;

/**
 * @brief callbackMap holds C++ functions that can be triggered from JS
 * when events like a button press or timer tick require modification of
 * the C++ model state.
 *
 */
map<string, std::function<void()>> clarity::WebElemNode::callbackMap;
clarity::TicketMachine clarity::ControlNetworkNode::tm;
val clarity::ControlNetworkNode::ActiveLink::CLElement_ =
    val::global("CLElement");

void NukeModel::iterate() {
    activity_ = currentFuelChunk_ * controlRodSetting_;
    printState();
}

int main() {
    using ActiveLink = ControlNetworkNode::ActiveLink;

    double *n = new double(50);
    clarity::ModelNode<double> *nm =
        new clarity::ModelNode<double>(n, clarity::CppType::Double);
    clarity::WebElemNode *maindiv =
        new clarity::WebElemNode("maindiv", "div", clarity::CppType::NoData);
    clarity::WebElemNode *ncntr = new clarity::WebElemNode(
        "numerator", "input", clarity::CppType::Double);
    clarity::WebElemNode *nslider = new clarity::WebElemNode(
        "numerator", "input", clarity::CppType::Double);
    clarity::WebElemNode *svgarea =
        new clarity::WebElemNode("svgarea", "svg", clarity::CppType::NoData);
    clarity::WebElemNode *cir1 =
        new clarity::WebElemNode("cir1", "circle", clarity::CppType::Double);
    clarity::WebAttrNode *cir1Radius =
        new clarity::WebAttrNode("r", clarity::CppType::Double, cir1);

    NukeModel *nmod = new NukeModel();
    NukeControl *nc =
        new NukeControl("nuke_control", "div", clarity::CppType::NoData, *nmod);
    svgarea->setAttribute("width", val("300"));
    svgarea->setAttribute("height", val("200"));
    svgarea->setAttribute("viewBox", val("0 0 200 200"));
    svgarea->setAttribute("style", val("border: 1px solid black"));
    cir1->setAttribute("cx", val(100));
    cir1->setAttribute("cy", val(100));
    cir1->setAttribute("stroke", val("green"));
    cir1->setAttribute("fill", val("rgb(50,99,77)"));
    cir1->setAttribute("stroke-width", val(4));
    cir1->setAttribute("r", val(80));

    nm->addALPeer(ActiveLink(cir1Radius, val(1)));

    clarity::WebAttrNode *cir1TempColor =
        new clarity::WebAttrNode("fill", clarity::CppType::String, cir1);
    double *temp = new double(85);
    clarity::WebElemNode *tempInput =
        new clarity::WebElemNode("temp", "input", clarity::CppType::Double);
    tempInput->setAttribute("type", val("text"));
    clarity::ModelNode<double> *tempModel =
        new clarity::ModelNode(temp, CppType::Double);

    val CLE = val::global("CLElement");
    val blackbody = CLE["blackbody"];

    tempModel->addALPeer(ActiveLink(tempInput, 1));
    tempModel->addALPeer(ActiveLink(cir1TempColor, blackbody));
    tempModel->pushValToPeersThruAL(tempModel);

    nslider->setAttribute("type", val("range"));
    maindiv->appendChild(ncntr);
    maindiv->appendChild(nslider);
    maindiv->appendChild(tempInput);
    maindiv->appendChild(nc);
    svgarea->appendChild(cir1);
    ncntr->setAttribute("type", val("text"));

    nm->addALPeer(ActiveLink(ncntr, 10));
    nm->addALPeer(ActiveLink(nslider, val(1)));

    ncntr->addEventListenerByName("change", "printNetworkState");
    nslider->addEventListenerByName("change", "printNetworkState");
    maindiv->appendChild(svgarea);

    std::string *buttonText = new std::string("CLICK ME!");

    nc->buttonText_ = new string("CLICK ME TOO!");
    nc->buttonModel_ = new clarity::ModelNode<string>(nc->buttonText_,
                                                      clarity::CppType::String);

    nc->buttonModel_->addALPeer(ActiveLink(nc->applyButton_));
    nc->buttonModel_->pushValToPeersThruAL(nc->buttonModel_);

    nm->pushValToPeersThruAL(nm);

    clarity::WebElemNode::callbackMap["iterateModel"] = [=] {
        cout << "callbackMap[\"iterateModel\"]\n";
    };

    clarity::WebElemNode::callbackMap["syncModelView"] = [=] {
        cout << "callbackMap[\"syncModelView\"]\n";
    };

    clarity::WebElemNode::callbackMap["printNetworkState"] = [=] {
        cout << "callbackMap[\"printNetworkState\"]\n";
    };

    clarity::WebElemNode::callbackMap["tick"] = [=] {
        (*temp) += 5;
        tempModel->pushValToPeersThruAL(tempModel);
    };

    printf("Setup complete!\n");
    return 0;
}
