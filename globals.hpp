/**
 * @file globals.hpp
 * @author Gregory Propf (gpropf@gmail.com)
 * @brief This is basically the boilerplate needed to create a Clarity app. It's mostly definitions of the static
 * globals we need to set up the Clarity web app framework.
 * @version 1.0
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef globals_hpp
#define globals_hpp

//#include "CanvasElement.hpp"
#include "ClarityNode.hpp"

map<const int, ClarityNode *> ClarityNode::switchboard;
map<string, std::function<void()>> ClarityNode::callbackMap;
TicketMachine ClarityNode::tm;
val ClarityNode::CLElement_ = val::global("CLElement");


#endif