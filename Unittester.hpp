
#include "CLNodeFactory.hpp"

template <template <typename V> class Nc, typename V, typename N>
class Unittester {
   protected:
    CLNodeFactory<Nc, V, N> *builder_;

   public:
    Unittester(CLNodeFactory<Nc, V, N> *builder) { builder_ = builder; } 

    //virtual void setup() = 0;
    virtual pair<V*,Nc<V>*> setup() = 0;
    virtual void runEvent() = 0;
    virtual bool evaluate() = 0;
    //virtual void cleanup(pair<V*,Nc<V>*>) = 0;
    virtual void cleanup(pair<V*,Nc<V>*> p) {
        delete p.first;
        delete p.second;

    }
};

template <template <typename V> class Nc, typename V, typename N>
class TextinputDoubleUT : public Unittester<Nc, V, N> {
   public:
    TextinputDoubleUT(CLNodeFactory<HybridNode, double, double> *builder)
        : Unittester<HybridNode, double, double>(builder) {}
    virtual pair<double*, HybridNode<double>*> setup() {
        double *dptr = new double(1.2);
        auto *doubleTextinputTest =
            this->builder_->withName("doubleTextinputTest").withCppVal(dptr).textInput();
        return pair(dptr, doubleTextinputTest);
    }
    virtual void runEvent(){};
    virtual bool evaluate() { return true; }
    
};