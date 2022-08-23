#ifndef CLNodeFactory_hpp
#define CLNodeFactory_hpp

#include "allheaders.hpp"

namespace clarity
{

    class CLNodeFactory
    {

        string tag_;
        string name_;
        clarity::CppType storedValueType_;

        

        inline CLNodeFactory() {}

        inline CLNodeFactory(const string &tag, const string &name, CppType storedValueType)
        : tag_(tag), name_(name), storedValueType_(storedValueType) {}

        ControlNetworkNode * build();
    };
}
#endif
