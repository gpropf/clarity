#ifndef ActiveVector_hpp
#define ActiveVector_hpp

#include "CLNodeFactory.hpp"

template <template <typename V> class Nc, typename V, typename N>
class ActiveVector {

    virtual ClarityNode * map(V *) = 0;
    virtual void push_back(V* v) {
        ClarityNode * node = map(v);
        storageVector_.push_back(pair(v,node));
    }

    protected:
    CLNodeFactory<Nc,V,N> builder_;
    vector<pair<V*,ClarityNode*>> storageVector_;
    

};























#endif