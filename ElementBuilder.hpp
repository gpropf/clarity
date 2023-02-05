#ifndef ElementBuilder_hpp
#define ElementBuilder_hpp

#include "SignalPrimitives.hpp"
#include "Util.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

namespace cl2 {

class ElementBuilder {
   public:
    TicketMachine tm_;
    bool labelAllInputs_ = true;
    bool labelsSwallowTheirReferents_ = true;
    bool addBRAfterAllCalls_ = true;
    val parentDOMElement_ = val::null();

    ElementBuilder(int startId = 0, bool labelAllInputs = true,
                   bool labelsSwallowTheirReferents = true, val parentDOMElement = val::null())
        : labelAllInputs_(labelAllInputs),
          labelsSwallowTheirReferents_(labelsSwallowTheirReferents),
          parentDOMElement_(parentDOMElement) {
        tm_ = TicketMachine(startId);
    }

    WebElement textInput(const std::string& name, const std::string& labelText) {
        const InputElement inp =
            InputElement("input", name, "text", tm_.getNextStrId(), parentDOMElement_);
        const Label lbl = Label(labelText, inp, labelsSwallowTheirReferents_, tm_.getNextStrId(),
                          parentDOMElement_);
        if (addBRAfterAllCalls_) BR();
        return inp;
    }
};
}  // namespace cl2
#endif