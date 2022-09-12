# Degugging Analysis

This file is my attempt to document my thought processes during debugging. In every situation where a bug proves difficult to resolve there's usually a basic failure of analysis where the problem is not correctly understood at first. We'll start with

2022-09-12: 
PROBLEM: I finally squashed the bug where the attribute nodes of the circle SVG element were being created with their own DOM elements *in addition to* the DOM elements of their parent nodes. When I disabled the thing that was creating the bogus DOM elements, I got JS errors as it looked for and tried to write to, attributes that didn't exist.

INITIAL ERROR: I had become convinced that the problem was that the system was not setting the domElement_ member of ClarityNode to point to the attribute's parent DOM element. I tried several things to remedy this supposed problem.

MISSED CLUES: I wasn't curious enough about the fact that the attributes of the "cir1" circle element *were* being updated, indicating that the domElement_ member of the attr nodes actually did point where it was supposed to.

EUREKA MOMENT: I realized that the whole problem was manifesting during the initial setup of the nodes, not during actual use. I disabled the line in the build() method reading `modelNode_->pushValToPeers2(modelNode_);` in CLNF.hpp and the node structure built fine but without initializing the field values. That call is now only made if the node in question is not an attribute node. If it is, the call is made in the attributeNode() method that calls the build method.