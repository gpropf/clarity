#ifndef LabelledInput_hpp
#define LabelledInput_hpp

//#include "clarity.hpp"
#include "CompoundElement.hpp"

namespace clarity
{

  /**
   * @brief Represents an element with multiple subelements and an outer "container" element.
   * Inspired by the idea of a labelled input field. The innerElement_ is used to store the
   * actual values so that getVal and setVal just become pass-thru methods that read and write
   * to the innerElement_.
   *
   */
  class LabelledInput : public CompoundElement

  {
  protected:
    clarity::WebElemNode *label_;

  public:
    LabelledInput(const string &name,
                  const string &tag,
                  const CppType anyvalPtrType,
                  clarity::WebElemNode *innerElement);
    virtual val getVal() const;
    virtual void setVal(const val &inval);
  };

}

#endif