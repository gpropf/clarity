#include "CLNodeFactory.hpp"

using namespace clarity;

ControlNetworkNode * CLNodeFactory::build() {
    return new WebElemNode(name_, tag_, storedValueType_);
}