
#include "CLNodeFactory.hpp"

template <template <typename V> class Nc, typename V, typename N>
class Unittester {
   protected:
    CLNodeFactory<Nc, V, N> *builder_;
    Nc<V> *node_, *label_;

    V *modelPtr_;
    const string nodeType_ = TypeName<Nc<V>>::Get();
    const string valueType_ = TypeName<V>::Get();
    const string linkMultiplierType_ = TypeName<N>::Get();
    const string completeType_ =
        nodeType_ + " [[" + valueType_ + "]]" + ", linkMul type=" + linkMultiplierType_;

   public:
    template <template <typename Vi> class Nci, typename Vi, typename Ni>
    Unittester(CLNodeFactory<Nci, Vi, Ni> *builder) {
        builder_ = new CLNodeFactory<Nc, V, N>(*builder);
    }

    // virtual void setup() = 0;
    virtual void setup() = 0;
    virtual void runEvent() = 0;
    virtual bool evaluate() = 0;

    virtual void notify(bool pass) {
        if (pass) {
            this->node_->getDomElement().template call<void>("setAttribute", val("style"),
                                                             val("background-color: lightgreen;"));
            return;
        }
        this->node_->getDomElement().template call<void>("setAttribute", val("style"),
                                                         val("background-color: red;"));
    }
    // virtual void cleanup(pair<V*,Nc<V>*>) = 0;
    virtual void cleanup() {
        // delete p.first;
        // delete p.second;
        delete modelPtr_;
        delete node_;
        delete label_;
    }
};

template <template <typename V> class Nc, typename V, typename N>
class TextinputUT : public Unittester<Nc, V, N> {
    const char *v1str = "67";
    const char *v2str = "37";
    V *v1 = new V(73);
    V *v2 = new V(84);

   public:
    template <template <typename Vi> class Nci, typename Vi, typename Ni>
    TextinputUT(CLNodeFactory<Nci, Vi, Ni> *builder) : Unittester<Nc, V, N>(builder) {}

    virtual void setup() {
        this->modelPtr_ = new V(65);
        cout << "Starting value is " << *this->modelPtr_ << endl;
        this->node_ =
            this->builder_->withName("textInputTest").withCppVal(this->modelPtr_).textInput();
        this->label_ = this->builder_->withName("textInputTest_lbl").label(this->node_, this->completeType_, true);
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
        if (*this->modelPtr_ == 87) {
            //  this->node_->getDomElement().template call<void>("setAttribute", val("style"),
            // val("background-color: lightgreen;"));
            return true;
        }
        cout << "TEST FAILED!" << endl;
        // this->node_->getDomElement().template call<void>("setAttribute", val("style"),
        //                                                  val("background-color: red;"));
        return false;
    }
};
