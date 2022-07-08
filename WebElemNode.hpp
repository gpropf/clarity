#ifndef WebElemNode_hpp
#define WebElemNode_hpp

#include "clarity.hpp"
#include "WebNode.hpp"

namespace clarity {

class WebElemNode : public WebNode
  {
  public:
    WebElemNode(const CppType anyvalPtrType) : WebNode(anyvalPtrType) {}

  protected:
    vector<ControlNetworkNode *> children_;

    // string tag_, name_;
    string tag_, name_;
    string stringId_;

  public:
    /**
     * @brief Construct a new Web Element object
     *
     * @param name String name of element, may be empty.
     * @param tag HTML tag of this element
     * @param anyvalPtrType C++ type of data contained within
     *
     */
    WebElemNode(const string &name, const string &tag,
                const CppType anyvalPtrType) : tag_(tag),
                                               name_(name),
                                               WebNode(anyvalPtrType)

    {

      jsval_.set("cpptype", val(anyvalPtrType));
      jsval_.set("tag", val(tag));
      jsval_.set("name", val(name_));
      jsval_.set("id", val(id_));

      anyvalPtr_ = nullptr;
      boundField_ = "value";

      ControlNetworkNode::switchboard[id_] = this;
    }

    void setAttribute(const string &attr, const val &value)
    {
      val domElement = jsval_["domElement"];
      domElement.call<void>("setAttribute", attr, value);
    }

    /**
     * @brief Update the view from the model
     *
     */
    /*
    void updateViewFromModel()
    {
      cout << "STARTING: updateViewFromModel for " << this->name_ << "\n";
      val domElement = this->jsval_["domElement"];
      for (auto child : children_)
      {
        child->updateViewFromModel();
      }

      if (anyvalPtr_ == nullptr)
      {
        cout << "ENDING: updateViewFromModel for " << this->name_ << " because anyvalPtr_ == nullptr\n";
        return;
      }

      switch (this->anyvalPtrType_)
      {
      case CppType::Int:
        domElement.set(boundField_, val(cpp2js<int>(anyvalPtr_)));
        break;
      case CppType::Float:
        domElement.set(boundField_, val(cpp2js<float>(anyvalPtr_)));
        break;
      case CppType::Double:
        // domElement.set(boundField, val(*reinterpret_cast<double *>(anyvalPtr_)));
        domElement.set(boundField_, val(cpp2js<double>(anyvalPtr_)));
        // jsval_.set("anyval", val(*reinterpret_cast<double *>(anyvalPtr_)));
        break;
      case CppType::String:
        domElement.set(boundField_, val(cpp2js<string>(anyvalPtr_)));
        break;
      case CppType::NoData:
      default:
        break;
      }
      cout << "ENDING: updateViewFromModel for " << this->name_ << "\n";
    } */
    /*
    virtual void updateModelFromView()
    {
      cout << "STARTING: updateModelFromView for " << this->name_ << "\n";
      for (auto child : children_)
      {
        child->updateModelFromView();
      }

      if (anyvalPtr_ == nullptr)
      {
        cout << "ENDING: updateModelFromView for " << this->name_ << " because anyvalPtr_ == nullptr\n";
        return; // this->jsval_["anyval"];
      }

      switch (this->anyvalPtrType_)
      {
      case CppType::Int:
        *reinterpret_cast<int *>(anyvalPtr_) = this->jsval_.call<int>("jsToCPPVal", getVal());
        cout << "C++ side: New Int Value: " << *reinterpret_cast<int *>(anyvalPtr_) << endl;
        break;
      case CppType::Float:
        *reinterpret_cast<float *>(anyvalPtr_) = this->jsval_.call<int>("jsToCPPVal", getVal());
        cout << "C++ side: New Float Value: " << *reinterpret_cast<float *>(anyvalPtr_) << endl;
        break;
      case CppType::Double:
        *reinterpret_cast<double *>(anyvalPtr_) = this->jsval_["anyval"].as<double>();
        *reinterpret_cast<double *>(anyvalPtr_) = this->jsval_.call<double>("jsToCPPVal", getVal());
        cout << "C++ side: New Double Value: " << *reinterpret_cast<double *>(anyvalPtr_) << endl;
        break;
      case CppType::String:
        *reinterpret_cast<string *>(anyvalPtr_) = this->jsval_.call<string>("jsToCPPVal", getVal());
        cout << "C++ side: New String Value: " << *reinterpret_cast<string *>(anyvalPtr_) << endl;
        break;
      case CppType::NoData:
        cout << "C++ side: This element contains no data." << endl;
        break;
      default:
        break;
      }
      cout << "ENDING: updateModelFromView for " << this->name_ << "\n";
      return; // this->jsval_["anyval"];
    } */

    bool appendChild(WebNode *child)
    {
      children_.push_back(child);
      jsval_.call<void>("appendChild", child->getJSval());
      return true; // FIXME: need to check for duplicate ids.
    }

    void addEventListenerByName(const string &eventName, const string &callbackName)
    {
      jsval_.call<void>("addEventListenerById", eventName, callbackName);
    }
    //=====================
    // static map<const int, WebElemNode *> switchboard;
    static map<string, std::function<void()>> callbackMap;

    string getTag() const { return tag_; }

    string getId() const { return name_; }
    // void setId(string id) { id_ = id; }

    void setJsval(val jsval) { jsval_ = jsval; }
    void *getAnyvalPtr() const { return anyvalPtr_; }
    void setAnyvalPtr(void *valptr) { anyvalPtr_ = valptr; }
    CppType getAnyvalPtrType() const { return anyvalPtrType_; }
    void setAnyvalPtrType(CppType cppType)
    {
      anyvalPtrType_ = cppType;
      jsval_.set("cpptype", cppType);
    }

    // static void updateModelFromViewById(const int id) { switchboard[id]->updateModelFromView(); }
    // static WebElemNode &getCLElementById(const int id) { return *(switchboard[id]); }
    static void runCallbackById(const string &id) { callbackMap[id](); }
  };




}

#endif
