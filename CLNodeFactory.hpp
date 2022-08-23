#ifndef CLNodeFactory_hpp
#define CLNodeFactory_hpp

#include "allheaders.hpp"

namespace clarity
{

    class CLNodeFactory
    {
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
        

    public:
        inline CLNodeFactory() {}
        inline CLNodeFactory(const string &tag, const string &name, CppType storedValueType)
            : tag_(tag), name_(name), storedValueType_(storedValueType) {}

        ControlNetworkNode *build();
        CLNodeFactory withTag(const string &tag)
        {
            CLNodeFactory cpy(*this);
            cpy.tag_ = tag;
            return cpy;
        }
    };
}
#endif
