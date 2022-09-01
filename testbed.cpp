#include "testbed.hpp"
#include "CLNodeFactory.hpp"
#include "embindings.hpp"

map<const int, clarity::ControlNetworkNode *> clarity::ControlNetworkNode::switchboard;
map<string, std::function<void()>> clarity::WebElemNode::callbackMap;
clarity::TicketMachine clarity::ControlNetworkNode::tm;
val clarity::ControlNetworkNode::ActiveLink::CLElement_ = val::global("CLElement");

int main()
{

    using ActiveLink = ControlNetworkNode::ActiveLink;

    double *n = new double(50);
    double *pi = new double(3.14159);

    clarity::WebElemNode *maindiv = new clarity::WebElemNode("maindiv", "div", clarity::CppType::NoData);

    clarity::CLNodeFactory<WebElemNode, double> builder("input", "input_double", CppType::Double, pi);

    clarity::WebElemNode *input1 = builder.buildWithModelNode();
    clarity::WebElemNode *input2 = builder.withStoredValue(n).buildWithModelNode();
    clarity::WebElemNode *button1 = builder.button("button1", "Press me!");
    button1->addEventListenerByName("click", "iterateModel");

    maindiv->appendChild(input1);
    maindiv->appendChild(input2);
    maindiv->appendChild(button1);
    input1->setAttribute("type", val("text"));
    input2->setAttribute("type", val("text"));

    clarity::WebElemNode::callbackMap["iterateModel"] = [=]
    {
        cout << "callbackMap[\"iterateModel\"]\n";
    };

    printf("Setup complete!\n");
    return 0;
}
