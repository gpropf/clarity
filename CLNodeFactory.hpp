#ifndef CLNodeFactory_hpp
#define CLNodeFactory_hpp

#include "allheaders.hpp"

namespace clarity
{

    class CLNodeFactory
    {

    public:
        enum class BasicNodeType : int
        {
            WebElemNode,
            WebAttrNode,
            ModelNode
        };

        string tag_;
        string name_;
        CppType storedValueType_;
        BasicNodeType basicNodeType_;

        inline CLNodeFactory() {}
        inline CLNodeFactory(const string &tag, const string &name, CppType storedValueType)
            : tag_(tag), name_(name), storedValueType_(storedValueType) {}

        ControlNetworkNode *build();
        CLNodeFactory withTag(const string &tag);
        CLNodeFactory withName(const string &name);
    };
}
#endif
