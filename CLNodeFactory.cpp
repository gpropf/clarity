#include "CLNodeFactory.hpp"

using namespace clarity;

ControlNetworkNode *CLNodeFactory::build()
{
    return new WebElemNode(name_, tag_, storedValueType_);
}

CLNodeFactory CLNodeFactory::withTag(const string &tag)
{
    CLNodeFactory cpy(*this);
    cpy.tag_ = tag;
    return cpy;
}

CLNodeFactory CLNodeFactory::withName(const string &name)
{
    CLNodeFactory cpy(*this);
    cpy.name_ = name;
    return cpy;
}