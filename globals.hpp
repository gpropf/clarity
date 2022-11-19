/**
 * @file globals.hpp
 * @author Gregory Propf (gpropf@gmail.com)
 * @brief This is basically the boilerplate needed to create a Clarity app. It's mostly definitions
 * of the static class members we need to set up the Clarity web app framework.
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

bool ClarityNode::clogSilent = false;

map<const int, ClarityNode *> ClarityNode::switchboard;
vector<string> ClarityNode::jsAuxScripts__;
//vector<string> ClarityNode::preloadJsAuxScripts__;
map<string, std::function<void()>> ClarityNode::callbackMap;
TicketMachine ClarityNode::tm;
val ClarityNode::JSProxyNode_ = val::global("JSProxyNode");
template <>
const array<string, 8> CanvasGrid<unsigned char>::colors = {
    "#F5F5DC", "#00FF00", "#00AA00", "#FF00FF", "#AA00AA", "#00AAFF", "#9090AA", "#888888"};

#endif