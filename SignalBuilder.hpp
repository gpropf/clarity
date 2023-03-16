/**
 * @file SignalBuilder.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief This is the factory class that creates and then connects the signals.
 * @version 0.1
 * @date 2023-03-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef SignalBuilder_hpp
#define SignalBuilder_hpp

#include "SignalPrimitives.hpp"
#include "Util.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

namespace cl2 {

/**
 * @brief Factory class that creates WebElements and signals. Also connects signals to one another.
 *
 */
class SignalBuilder {
    TicketMachine tm_;
    bool labelAllInputs_ = true;
    bool labelsSwallowTheirReferents_ = true;
    bool addBRAfterAllCalls_ =
        true;  //!< add a '<BR>' tag after each call so everything goes on its own line.
    val parentDOMElement_ = val::null();
    std::map<const std::string, const WebElement*>
        elementMap_;  //!< a map of elements we've created so we can re-use them.
    std::map<std::string, val> attrs_;
    bool attrsResetAfterSingleUse_ = true;

   public:
    SignalBuilder(int startId = 0, bool labelAllInputs = true,
                  bool labelsSwallowTheirReferents = true, val parentDOMElement = val::null())
        : labelAllInputs_(labelAllInputs),
          labelsSwallowTheirReferents_(labelsSwallowTheirReferents),
          parentDOMElement_(parentDOMElement) {
        tm_ = TicketMachine(startId);
    }

    /**
     * @brief Do this after each method call.
     *
     */
    void postCall(const WebElement& wel) {
        wel.setAttributes(attrs_);
        if (attrsResetAfterSingleUse_) attrs_ = {};

        if (addBRAfterAllCalls_) BR(this->parentDOMElement_);
    }

    /**
     * @brief Add a WebElement to the global map.
     *
     * @param wel
     * @param name
     */
    void addElementToMap(const WebElement& wel, const std::string& name) {
        elementMap_.insert({name, &wel});
    }

    /**
     * @brief Get an element from the global map. Returns a null pointer if element is not found.
     *
     * @param name
     * @return const WebElement*
     */
    const WebElement* getElementByName(const std::string& name) {
        if (elementMap_.find(name) != elementMap_.end()) {
            return elementMap_[name];
        }
        return nullptr;
    }

    /**
     * @brief Creates a `WebElementSignalObject` that represents the specified attribute of a
     * `WebElement`. An example would be creating an attribute signal to represent the radius of an
     * SVG circle object.
     *
     * @tparam S
     * @param wel
     * @param attributeName
     * @param emitInitialValue
     * @return shared_ptr<WebElementSignalObject<S>>
     */
    template <typename S>
    shared_ptr<WebElementSignalObject<S>> attributeSignal(const WebElement& wel,
                                                          const std::string& attributeName,
                                                          bool emitInitialValue = true) {
        shared_ptr<WebElementSignalObject<S>> aso =
            make_shared<cl2::WebElementSignalObject<S>>(wel, attributeName, emitInitialValue);
        return aso;
    }

    /**
     * @brief Creates a `WebElementSignalObject` that represents the specified attribute of a
     * `WebElement` but gets the WE from the global map using the identifier string.
     *
     * @tparam S
     * @param elementName
     * @param attributeName
     * @param emitInitialValue
     * @return shared_ptr<WebElementSignalObject<S>>
     */
    template <typename S>
    shared_ptr<WebElementSignalObject<S>> attributeSignal(const std::string& elementName,
                                                          const std::string& attributeName,
                                                          bool emitInitialValue = true) {
        const WebElement* welptr = getElementByName(elementName);
        if (welptr != nullptr) {
            return attributeSignal<S>(*welptr, attributeName, emitInitialValue);
        }
        return nullptr;
    }

    /**
     * @brief Create future elements with the provided attributes.
     *
     * @param attrs
     * @param attrsResetAfterSingleUse
     * @return SignalBuilder
     */
    SignalBuilder withAttributes(const std::map<std::string, val>& attrs,
                                 bool attrsResetAfterSingleUse = true) const& {
        SignalBuilder cpy(*this);
        cpy.attrs_ = attrs;
        cpy.attrsResetAfterSingleUse_ = attrsResetAfterSingleUse;
        return cpy;
    }

    /**
     * @brief Create future elements with the provided attributes. (r-value version)
     *
     * @param attrs
     * @param attrsResetAfterSingleUse
     * @return SignalBuilder
     */
    SignalBuilder withAttributes(const std::map<std::string, val>& attrs,
                                 bool attrsResetAfterSingleUse = true) && {
        SignalBuilder cpy(std::move(*this));
        cpy.attrs_ = attrs;
        cpy.attrsResetAfterSingleUse_ = attrsResetAfterSingleUse;
        return cpy;
    }

    /**
     * @brief Create future elements as children of the provided DOM element.
     *
     * @param parentDOMElement
     * @return SignalBuilder
     */
    SignalBuilder withParentDOMElement(val parentDOMElement) const& {
        SignalBuilder cpy(*this);
        cpy.parentDOMElement_ = parentDOMElement;
        return cpy;
    }

    /**
     * @brief r-value version of above.
     *
     * @param parentDOMElement
     * @return SignalBuilder
     */
    SignalBuilder withParentDOMElement(val parentDOMElement) && {
        SignalBuilder cpy(std::move(*this));
        cpy.parentDOMElement_ = parentDOMElement;
        return cpy;
    }

    /**
     * @brief Does what `withParentDOMElement()` does except using a `WebElement` reference.
     *
     * @param parentElement
     * @return SignalBuilder
     */
    SignalBuilder withParentWebElement(const WebElement& parentElement) const& {
        SignalBuilder cpy(*this);
        cpy.parentDOMElement_ = parentElement.domElement_;
        return cpy;
    }

    /**
     * @brief r-value version of above.
     *
     * @param parentElement
     * @return SignalBuilder
     */
    SignalBuilder withParentWebElement(const WebElement& parentElement) && {
        SignalBuilder cpy(std::move(*this));
        cpy.parentDOMElement_ = parentElement.domElement_;
        return cpy;
    }

    /**
     * @brief Create a text input field.
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
        postCall(inp);
        addElementToMap(inp, name);
        return inp;
    }

    /**
     * @brief Creates a text input signal emitter using an `EventListenerEmitter`
     *
     * @tparam S
     * @param name
     * @param labelText
     * @param emitInitialValue
     * @return shared_ptr<EventListenerEmitter<S>>
     */
    template <typename S>
    shared_ptr<EventListenerEmitter<S>> textInputELE(const std::string& name,
                                                     const std::string& labelText,
                                                     bool emitInitialValue = true) {
        InputElement inp = textInput(name, labelText);
        val inpDE = inp.getDomElement();
        shared_ptr<EventListenerEmitter<S>> wso =
            make_shared<EventListenerEmitter<std::string>>(inpDE, "change");
        return wso;
    }

    /**
     * @brief Creates a "two-sided" text field signal. It can be used as an emitter or acceptor.
     *
     * @tparam S
     * @param name
     * @param labelText
     * @param emitInitialValue
     * @return shared_ptr<WebElementSignal<S>>
     */
    template <typename S>
    shared_ptr<WebElementSignal<S>> textInputWSS(const std::string& name,
                                                         const std::string& labelText,
                                                         bool emitInitialValue = true) {
        InputElement inp = textInput(name, labelText);
        shared_ptr<WebElementSignal<S>> wso =
            make_shared<WebElementSignal<S>>(inp, "value", emitInitialValue);

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
        postCall(inp);
        return inp;
    }

    /**
     * @brief Creates a "two-sided" range field signal. It can be used as an emitter or acceptor.
     *
     * @tparam S
     * @param name
     * @param labelText
     * @param emitInitialValue
     * @return shared_ptr<WebElementSignal<S>>
     */
    template <typename S>
    shared_ptr<WebElementSignal<S>> rangeInputWSS(const std::string& name,
                                                          const std::string& labelText,
                                                          bool emitInitialValue = true) {
        InputElement inp = rangeInput(name, labelText);
        shared_ptr<WebElementSignal<S>> wso =
            make_shared<WebElementSignal<S>>(inp, "value", emitInitialValue);
        return wso;
    }

    /**
     * @brief Create a button with provided callback.
     *
     * @param displayedText
     * @param onClickFn
     * @return Button
     */
    Button button(const std::string& displayedText, val onClickFn) {
        Button btn =
            Button(displayedText, displayedText, onClickFn, tm_.getNextStrId(), parentDOMElement_);
        postCall(btn);
        return btn;
    }

    /**
     * @brief Plug an emitter signal into an acceptor.
     *
     * @tparam S
     * @param s1
     * @param s2
     */
    template <typename S>
    void connect(shared_ptr<SignalEmitter<S>> s1, shared_ptr<SignalAcceptor<S>> s2) {
        s1->setOutput(s2);
    }

    /**
     * @brief Connect the two inputs to the Merge and then connect the Merge to the optional output.
     *
     * @tparam inT1
     * @tparam inT2
     * @tparam outT
     * @param s1
     * @param s2
     * @param merge
     * @param mergeOut
     */
    template <typename inT1, typename inT2, typename outT>
    void connect(shared_ptr<SignalEmitter<inT1>> s1, shared_ptr<SignalEmitter<inT2>> s2,
                 shared_ptr<MergeSS<inT1, inT2, outT>> merge,
                 shared_ptr<SignalAcceptor<outT>> mergeOut = nullptr) {
        s1->setOutput(merge->getInput1());
        s2->setOutput(merge->getInput2());
        if (mergeOut != nullptr) merge->setOutput(mergeOut);
    }
};
}  // namespace cl2
#endif