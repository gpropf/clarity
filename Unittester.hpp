
#include "CLNodeFactory.hpp"

template <template <typename V> class Nc, typename V, typename N>
class Unittester {
   protected:
    CLNodeFactory<Nc, V, N> *builder_;
    Nc<V> *node_;
    V *modelPtr_;

   public:
    template <template <typename Vi> class Nci, typename Vi, typename Ni>
    Unittester(CLNodeFactory<Nci, Vi, Ni> *builder) {
        builder_ = new CLNodeFactory<Nc, V, N>(*builder);
    }

    // virtual void setup() = 0;
    virtual pair<V *, Nc<V> *> setup() = 0;
    virtual void runEvent() = 0;
    virtual bool evaluate() = 0;
    // virtual void cleanup(pair<V*,Nc<V>*>) = 0;
    virtual void cleanup() {
        // delete p.first;
        // delete p.second;
        delete modelPtr_;
        delete node_;
    }
};

template <template <typename V> class Nc, typename V, typename N>
class TextinputUT : public Unittester<Nc, V, N> {
    const char* v1str = "67";
    const char* v2str = "37";
    V *v1 = new V(73);
    V *v2 = new V(84);

   public:
    template <template <typename Vi> class Nci, typename Vi, typename Ni>
    TextinputUT(CLNodeFactory<Nci, Vi, Ni> *builder) : Unittester<Nc, V, N>(builder) {}

    virtual pair<V *, Nc<V> *> setup() {
        this->modelPtr_ = new V(65);
        cout << "Starting value is " << *this->modelPtr_ << endl;
        this->node_ =
            this->builder_->withName("textInputTest").withCppVal(this->modelPtr_).textInput();

        return pair(this->modelPtr_, this->node_);
    }

    virtual void runEvent() {
        val makeEvent = val::global("makeEvent");
        val ev = makeEvent(val(string("change")));
        val domEl = this->node_->getDomElement();
        domEl.set(this->node_->getBoundField(), "87");
        domEl.call<void>("dispatchEvent", ev);
    }

    virtual bool evaluate() {
        cout << "Model delta: " << *this->modelPtr_ - 87 << endl;
        delete this->builder_;
        if (*this->modelPtr_ == 87) return true;
        cout << "TEST FAILED!" << endl;
        return false;
    }
};
