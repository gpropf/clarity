
#include "CLNodeFactory.hpp"

template <template <typename V> class Nc, typename V, typename N>
class Unittester {
   protected:
    CLNodeFactory<Nc, V, N> *builder_;
    Nc<V> *node_;
    V *modelPtr_;

   public:
    Unittester(CLNodeFactory<Nc, V, N> *builder) { builder_ = CLNodeFactory<Nc, V, N>(builder); }

    // virtual void setup() = 0;
    virtual pair<V *, Nc<V> *> setup() = 0;
    virtual void runEvent() = 0;
    virtual bool evaluate() = 0;
    // virtual void cleanup(pair<V*,Nc<V>*>) = 0;
    virtual void cleanup(pair<V *, Nc<V> *> p) {
        delete p.first;
        delete p.second;
    }
};

template <template <typename V> class Nc, typename V, typename N>
class TextinputDoubleUT : public Unittester<Nc, V, N> {
   public:
    TextinputDoubleUT(CLNodeFactory<HybridNode, double, double> *builder)
        : Unittester<HybridNode, double, double>(builder) {}

    virtual pair<double *, HybridNode<double> *> setup() {
        this->modelPtr_ = new double(1.2);
        this->node_ =
            this->builder_->withName("doubleTextinputTest").withCppVal(this->modelPtr_).textInput();

        return pair(this->modelPtr_, this->node_);
    }

    virtual void runEvent() {
        val makeEvent = val::global("makeEvent");
        val ev = makeEvent(val(string("change")));
        val domEl = this->node_->getDomElement();
        domEl.set(this->node_->getBoundField(), "2.0");
        domEl.call<void>("dispatchEvent", ev);
    }

    virtual bool evaluate() {
        cout << "Model val: " << *this->modelPtr_ - 2.0 << endl;
        if (*this->modelPtr_ == 2.0) return true;
        return false;
    }
};
