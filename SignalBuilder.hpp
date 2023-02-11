#ifndef SignalBuilder_hpp
#define SignalBuilder_hpp

#include "SignalPrimitives.hpp"
#include "Util.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

namespace cl2 {

class SignalBuilder {
    TicketMachine tm_;
    bool labelAllInputs_ = true;
    bool labelsSwallowTheirReferents_ = true;
    bool addBRAfterAllCalls_ = true;
    val parentDOMElement_ = val::null();

   public:
    SignalBuilder(int startId = 0, bool labelAllInputs = true,
                  bool labelsSwallowTheirReferents = true, val parentDOMElement = val::null())
        : labelAllInputs_(labelAllInputs),
          labelsSwallowTheirReferents_(labelsSwallowTheirReferents),
          parentDOMElement_(parentDOMElement) {
        tm_ = TicketMachine(startId);
    }

    void postCall() {
        if (addBRAfterAllCalls_) BR(this->parentDOMElement_);
    }

    SignalBuilder withParentDOMElement(val parentDOMElement) const& {
        SignalBuilder cpy(*this);
        cpy.parentDOMElement_ = parentDOMElement;
        return cpy;
    }

    SignalBuilder withParentDOMElement(val parentDOMElement) && {
        SignalBuilder cpy(std::move(*this));
        cpy.parentDOMElement_ = parentDOMElement;
        return cpy;
    }

    SignalBuilder withParentWebElement(const WebElement& parentElement) const& {
        SignalBuilder cpy(*this);
        cpy.parentDOMElement_ = parentElement.domElement_;
        return cpy;
    }

    SignalBuilder withParentWebElement(const WebElement& parentElement) && {
        SignalBuilder cpy(std::move(*this));
        cpy.parentDOMElement_ = parentElement.domElement_;
        return cpy;
    }

    /**
     * @brief Create a text imput field.
     *
     * @param name
     * @param labelText
     * @return WebElement
     */
    InputElement textInput(const std::string& name, const std::string& labelText) {
        const InputElement inp =
            InputElement("input", name, "text", tm_.getNextStrId(), parentDOMElement_);
        const Label lbl = Label(labelText, inp, labelsSwallowTheirReferents_, tm_.getNextStrId(),
                                parentDOMElement_);
        postCall();
        return inp;
    }

    template <typename S>
    shared_ptr<WebElementSignalObject<S>> textInput(const std::string& name,
                                                    const std::string& labelText,
                                                    bool emitInitialValue = true) {
        InputElement inp = textInput(name, labelText);
        shared_ptr<WebElementSignalObject<S>> wso =
            make_shared<cl2::WebElementSignalObject<S>>(inp, "value", emitInitialValue);
        return wso;
    }

    /**
     * @brief Make a range input
     *
     * @param name
     * @param labelText
     * @return InputElement
     */
    InputElement rangeInput(const std::string& name, const std::string& labelText) {
        const InputElement inp =
            InputElement("input", name, "range", tm_.getNextStrId(), parentDOMElement_);
        const Label lbl = Label(labelText, inp, labelsSwallowTheirReferents_, tm_.getNextStrId(),
                                parentDOMElement_);
        postCall();
        return inp;
    }

    template <typename S>
    shared_ptr<WebElementSignalObject<S>> rangeInput(const std::string& name,
                                                     const std::string& labelText,
                                                     bool emitInitialValue = true) {
        InputElement inp = rangeInput(name, labelText);
        shared_ptr<WebElementSignalObject<S>> wso =
            make_shared<cl2::WebElementSignalObject<S>>(inp, "value", emitInitialValue);
        return wso;
    }

    Button button(const std::string& displayedText, val onClickFn) {
        Button btn =
            Button(displayedText, displayedText, onClickFn, tm_.getNextStrId(), parentDOMElement_);
        postCall();
        return btn;
    }

    template <typename S>
    void connect(shared_ptr<SignalObject<S>> s1, shared_ptr<SignalObject<S>> s2) {
        s1->setOutput(s2);
    }

    template <typename S>
    void connect(shared_ptr<Tee<S>> tee, shared_ptr<SignalObject<S>> s1,
                 shared_ptr<SignalObject<S>> s2) {
        // s1->setOutput(s2);
        tee->setOutput(s1);
        tee->setSecondOutput(s2);
    }

    template <typename inT1, typename inT2, typename outT>
    void connect(shared_ptr<SignalObject<inT1>> s1, shared_ptr<SignalObject<inT2>> s2,
                 shared_ptr<Merge<inT1, inT2, outT>> merge,
                 shared_ptr<SignalObject<outT>> mergeOut = nullptr) {
        s1->setOutput(merge->getInput1());
        s2->setOutput(merge->getInput2());        
        if (mergeOut != nullptr) merge->setOutput(mergeOut);
    }
};
}  // namespace cl2
#endif