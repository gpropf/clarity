#include "testbed.hpp"
#include "CLNodeFactory.hpp"
#include "embindings.hpp"

map<const int, clarity::ControlNetworkNode *> clarity::ControlNetworkNode::switchboard;
map<string, std::function<void()>> clarity::WebElemNode::callbackMap;
clarity::TicketMachine clarity::ControlNetworkNode::tm;

int main()
{

    using ActiveLink = ControlNetworkNode::ActiveLink;

    double *n = new double(50);
    clarity::ModelNode<double> *nm = new clarity::ModelNode(n, clarity::CppType::Double);
    clarity::WebElemNode *maindiv = new clarity::WebElemNode("maindiv", "div", clarity::CppType::Double);

    clarity::CLNodeFactory<WebAttrNode> builder("input", "foo_input", CppType::Float);
    clarity::ControlNetworkNode *input1 = builder.build();
    clarity::ControlNetworkNode *input2 = builder.withTag("div").build();
    maindiv->appendChild(input1);
    maindiv->appendChild(input2);

    printf("Setup complete!\n");
    return 0;
}
