#include "testbed.hpp"

#include <map>

#include "CLNodeFactory.hpp"
#include "clarity.hpp"
#include "embindings.hpp"

map<const int, ControlNetworkNode *> ControlNetworkNode::switchboard;
map<string, std::function<void()>> WebElemNode::callbackMap;
TicketMachine ControlNetworkNode::tm;
val ControlNetworkNode::ActiveLink::CLElement_ = val::global("CLElement");

int main() {
    using ActiveLink = ControlNetworkNode::ActiveLink;

    double *n = new double(50);
    double *pi = new double(3.14159);
    double *a = new double(2.78);

    WebElemNode *maindiv = new WebElemNode("maindiv", "div", CppType::NoData);

    CLNodeFactory<WebElemNode, double> builder("input", "input_double",
                                               CppType::Double, pi);

    WebElemNode *input1 = builder.buildWithModelNode();
    WebElemNode *input2 = builder.withStoredValue(n).buildWithModelNode();
    WebElemNode *button1 = builder.button("button1", "Press me!");
    WebElemNode *labelInput1 = builder.label(input1, "PI");
    button1->addEventListenerByName("click", "iterateModel");

    map<string, val> inputFieldAttrs = {{"type", val("text")}};
    WebElemNode *input_a = builder.withStoredValue(a)
                               .withName("input_a_text")
                               .withAttributes(inputFieldAttrs)
                               .buildWithModelNode();
    WebElemNode *labelled_input_a =
        builder.labelGivenNode(input_a, "Label for A");

    maindiv->appendChild(input1);
    maindiv->appendChild(labelInput1);
    maindiv->appendChild(input2);
    maindiv->appendChild(button1);
    // maindiv->appendChild(input_a);
    maindiv->appendChild(labelled_input_a);
    input1->setAttribute("type", val("text"));
    input2->setAttribute("type", val("text"));

    WebElemNode::callbackMap["iterateModel"] = [=] {
        cout << "callbackMap[\"iterateModel\"]\n";
    };

    printf("Setup complete!\n");
    return 0;
}
