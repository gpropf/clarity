
#include "CLNodeFactory.hpp"
 

/**
 * @brief Adapted from https://stackoverflow.com/questions/4484982/how-to-convert-typename-t-to-string-in-c
 * This class is basically designed to handle anything pertainting to types for our unit testing efforts.
 * 
 * @tparam T 
 */
template <typename T>
struct TypeWrangler {
    static const string getCanonicalName() { return typeid(T).name(); }
    /**
     * @brief Intended for unit testing, return a typical value which will then
     * change. The only important things about these value is that they differ. The basic template
     * always asserts 'false' because of course there is not such thing a typical value of
     * an unknown type.
     *
     * @return T
     */
    static T typicalFirstValue() { assert(false); }
    /**
     * @brief Can return anything as long as it's not the same as the first value.
     * 
     * @return T 
     */
    static T typicalSecondValue() { assert(false); }
};

// a specialization for each type of those you want to support
// and don't like the string returned by typeid
template <>
struct TypeWrangler<int> {
    static const string getCanonicalName() { return "int"; }
    static int typicalFirstValue() { return -5; }
    static int typicalSecondValue() { return -19; }
};

template <>
struct TypeWrangler<double> {
    static const string getCanonicalName() { return "double"; }
    static double typicalFirstValue() { return -5.3; }
    static double typicalSecondValue() { return -19.6; }
};

template <>
struct TypeWrangler<float> {
    static const string getCanonicalName() { return "float"; }
    static float typicalFirstValue() { return -5.3; }
    static float typicalSecondValue() { return -19.6; }
};

template <>
struct TypeWrangler<unsigned char> {
    static const string getCanonicalName() { return "unsigned char"; }
    static unsigned char typicalFirstValue() { return '1'; }
    static unsigned char typicalSecondValue() { return '2'; }
};

template <>
template <typename V>
struct TypeWrangler<HybridNode<V>>
{
    static const string getCanonicalName()
    {
        return string("HybridNode &lt;") + TypeWrangler<V>::getCanonicalName() + string("&gt;");
    }
};

// usage:
// const string name = TypeName<MyType>::Get();



template <template <typename V> class Nc, typename V, typename N>
class Unittester {
   protected:
    CLNodeFactory<Nc, V, N> *builder_;
    Nc<V> *node_, *label_, *br_;

    V *modelPtr_;
    const string nodeType_ = TypeWrangler<Nc<V>>::getCanonicalName();
    const string valueType_ = TypeWrangler<V>::getCanonicalName();
    const string linkMultiplierType_ = TypeWrangler<N>::getCanonicalName();
    const string completeType_ =
        nodeType_ + ", value type=[" + valueType_ + "], linkMul type=[" + linkMultiplierType_ + "]";

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
        delete br_;
    }
};

template <template <typename V> class Nc, typename V, typename N>
class TextinputUT : public Unittester<Nc, V, N> {
   // const string v1str = "17.4";    
    V v1 = TypeWrangler<V>::typicalFirstValue();
    V v2 = TypeWrangler<V>::typicalSecondValue();
    const string v2str = clto_str<V>(v2);

   public:
    template <template <typename Vi> class Nci, typename Vi, typename Ni>
    TextinputUT(CLNodeFactory<Nci, Vi, Ni> *builder) : Unittester<Nc, V, N>(builder) {}

    virtual void setup() {
        this->modelPtr_ = new V(v1);
        cout << "Starting value is " << *this->modelPtr_ << endl;
        this->node_ =
            this->builder_->withName("textInputTest").withCppVal(this->modelPtr_).textInput();
        this->label_ = this->builder_->withName("textInputTest_lbl").label(this->node_, this->completeType_, true);
        this->br_ = this->builder_->br();
    }

    virtual void runEvent() {
        val makeEvent = val::global("makeEvent");
        val ev = makeEvent(val(string("change")));
        val domEl = this->node_->getDomElement();
        domEl.set(this->node_->getBoundField(), v2str);
        domEl.call<void>("dispatchEvent", ev);
    }

    virtual bool evaluate() {
        cout << "Model Val: " << *this->modelPtr_ << ", text entered into field: " << v2str << ", target numeric value: " << v2 << endl;
        cout << "Model delta: " << *this->modelPtr_ - v2 << endl;
        delete this->builder_;
        if (*this->modelPtr_ == v2) {
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
