#ifndef CompoundElement_hpp
#define CompoundElement_hpp

//#include "clarity.hpp"
#include "WebElemNode.hpp"

namespace clarity
{

  /**
   * @brief Represents an element with multiple subelements and an outer "container" element.
   * Inspired by the idea of a labelled input field. The innerElement_ is used to store the
   * actual values so that getVal and setVal just become pass-thru methods that read and write
   * to the innerElement_
   *
   */
  class CompoundElement : public WebElemNode

  {
  public:
    virtual val getVal() const;
    virtual void setVal(const val &inval);

    // CompoundElement(const string &name,
    //                 const string &tag,
    //                 const CppType anyvalPtrType,
    //                 clarity::WebElemNode *innerElement);
    CompoundElement(const string &name,
                    const string &tag,
                    const CppType anyvalPtrType);
    CompoundElement(const string &tag,
                    const CppType anyvalPtrType, const string &name = "");
    CompoundElement(const string &name = "",
                    const string &tag = "div",
                    const CppType anyvalPtrType = CppType::NoData,
                    clarity::WebElemNode *innerElement = nullptr);

  protected:
    // clarity::WebElemNode *outerElement_; // Container div
    clarity::WebElemNode *innerElement_; // Holds the actual value
  };

}

#endif