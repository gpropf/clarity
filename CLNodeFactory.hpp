#ifndef CLNodeFactory_hpp
#define CLNodeFactory_hpp

#include "allheaders.hpp"

namespace clarity
{
    template <typename T>
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

        T *build()
        {
            return new T(name_, tag_, storedValueType_);
        }

        CLNodeFactory<T> withTag(const string &tag)
        {
            CLNodeFactory cpy(*this);
            cpy.tag_ = tag;
            return cpy;
        }

        CLNodeFactory<T> withName(const string &name)
        {
            CLNodeFactory cpy(*this);
            cpy.name_ = name;
            return cpy;
        }
    };
}
#endif
