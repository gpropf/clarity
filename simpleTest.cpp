
#include "ElementBuilder.hpp"
#include "SignalPrimitives.hpp"
#include "Util.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

using namespace cl2;


int main() {
    

   
    // We're now using the ElementBuilder factory objects to create our web content.
    cl2::ElementBuilder eb = cl2::ElementBuilder();//
    //eb = eb.withParentWebElement(capturedDiv);

    // Creating two text fields.
    const auto srcTextInput = eb.textInput(
        "srcTextInput", "Source field: type something here and it will appear in the next field.");
    const auto dstTextInput = eb.textInput(
        "dstTextInput",
        "Destination field: Type something in the field above and it will be copied here.");

        const auto dstTextInput2 = eb.textInput(
        "dstTextInput2",
        "Second Destination field: another copy of the source field.");

    // Signal wrappers for the text fields.
    auto srcTextInputWSO = make_shared<cl2::WebElementSignalObject<std::string>>(srcTextInput, "value");
    auto dstTextInputWSO = make_shared<cl2::WebElementSignalObject<std::string>>(dstTextInput, "value");
    auto dstTextInputWSO2 = make_shared<cl2::WebElementSignalObject<std::string>>(dstTextInput2, "value");
    srcTextInputWSO->setOutput(dstTextInputWSO);

    auto t1 = make_shared<cl2::Tee<std::string>>();
    t1->setOutput(dstTextInputWSO);
    t1->setSecondOutput(dstTextInputWSO2);
    srcTextInputWSO->setOutput(t1);
    
    return 0;
}

EMSCRIPTEN_BINDINGS(CppObjectSignalObject) {
    

    emscripten::class_<cl2::Merge<std::string, std::string, std::string>>("Merge")
        .function("recompute", &cl2::Merge<std::string, std::string, std::string>::recompute,
                  emscripten::allow_raw_pointers());
}
