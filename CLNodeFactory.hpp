#ifndef CLNodeFactory_hpp
#define CLNodeFactory_hpp

#include "allheaders.hpp"

namespace clarity
{
    template <typename T, typename V>
    class CLNodeFactory
    {

    public:
        enum class BasicNodeType : int
        {
            WebElemNode,
            WebAttrNode,
            ModelNode
        };

        string tag_;              //!< Tag to be used with elements this factory builds.
        string name_;             //!< Name to be used with elements this factory builds.
        CppType storedValueType_; //!< storedValueType to be used with elements this factory builds.
        V *storedValue_;          //!< Actually only used when creating a model node along with a web control.
        BasicNodeType basicNodeType_;

        inline CLNodeFactory() {}
        inline CLNodeFactory(const string &tag, const string &name, CppType storedValueType)
            : tag_(tag), name_(name), storedValueType_(storedValueType) {}

        inline CLNodeFactory(const string &tag, const string &name, CppType storedValueType, V *storedValue)
            : tag_(tag), name_(name), storedValueType_(storedValueType), storedValue_(storedValue) {}

        T *build()
        {
            return new T(name_, tag_, storedValueType_);
        }

        T *buildInsideNode(WebNode *outerNode)
        {
            WebNode *innerNode = build();
            outerNode->appendChild(innerNode);
            return outerNode;
        }

        T *buildWithModelNode(const val transformFn = val(1))
        {
            T *cnn = build();
            ModelNode<V> *mn = new ModelNode<V>(storedValue_, storedValueType_);
            mn->addALPeer(clarity::ControlNetworkNode::ActiveLink(cnn, transformFn));
            mn->pushValToPeersThruAL(mn);
            return cnn;
        }

        CLNodeFactory
        withTag(const string &tag)
        {
            CLNodeFactory cpy(*this);
            cpy.tag_ = tag;
            return cpy;
        }

        CLNodeFactory withName(const string &name)
        {
            CLNodeFactory cpy(*this);
            cpy.name_ = name;
            return cpy;
        }

        CLNodeFactory withStoredValueType(clarity::CppType storedValueType)
        {
            CLNodeFactory cpy(*this);
            cpy.storedValueType_ = storedValueType;
            return cpy;
        }

        CLNodeFactory withStoredValue(V *storedValue)
        {
            CLNodeFactory cpy(*this);
            cpy.storedValue_ = storedValue;
            return cpy;
        }
    };
}
#endif
