# Clarity

Clarity is a web development framework written in C++ using emscripten to facilitate web programming. Its purpose is similar to the React or Vue JavaScript frameworks. It allows you to connect aspects of a data model to web controls in a "reactive" fashion. This means that if the model changes, the web presentation reflects those changes and if the user alters the state of the web controls, the corresponding model elements will update. This is all meant to occur automatically, without the need to explicitly handle web events. The name "Clarity" is meant to imply that you can see your underlying C++ data structures clearly and in an up-to-the-minute fashion without needing to constantly press buttons to reload what you've been working on.

I also had existing C++ code in mind as I wrote this. I wanted something that would allow you to put an existing C++ program on a website with as little modification to the existing data structures as possible.

## Design ##

Setting up responsive web components that allow an end user to interact smoothly with a data model while presenting the api user with an easy to use programming toolkit is a hugely complex task. Taking into account the fact that C++ is a strongly typed language multiplies the potential number of controls still further. The interactions with JavaScript's different and much weaker type system adds another dimension of complexity. 

The current system is the result of several complete refactorings that I undertook as I realized the shortcomings of each successive approach. The current system basically uses several approaches rather than a single programming paradigm. We do not try to extract all the needed functionality from either an object hierarchy or a system of template functions. Thus, there are OO elements such as the fact that I implement some of the more complex controls (like the CanvasGrid) as child classes of more generalized ones. However, I also use templates extensively to allow C++ types to be used directly in web controls. Finally, there is a large templated factory class that can keep track of many of the parameters that go into making web UI elements.

### Terminology ###

Node
: a single GUI element generally created using a node factory. Usually, but not necessarily, contains a C++ pointer to some part of the data model.

Node Factory
: Due to the large number of factors that come into play in creating nodes, we set up factory objects (using the CLNodeFactory class) that store certain persistent parameters relevant to setting up nodes. The most important of these parameters are the template parameters that determine the C++ data types of the underlying data.

Peer
: Nodes can have relationships with other nodes that result in automatic data movement and updating when one node is changed. This can be either due to the end user or the model changing something. Nodes that have such a relationship with another are called its peers.



### An Interface is a Graph ###

Mathematically, due to the fact that nodes can be connected together and that updating one node can trigger others to update, we can regard any Clarity GUI as a graph. I've attempted to take into account the differences between cyclic and acyclic graphs in the various update methods but this is limited right now to marking nodes with a boolean "clean/dirty" flag so that data will not bounce between nodes and create infinite loops. A future direction here would be to have page validation methods that would produce warnings or errors due to the presence of graph cycles or conflicting update rules.


### Events ###

When you create a node the system will install a set of event listeners that are designed to implement the movement of data from the model to and from the GUI. We also need a way for api users to add their own event listeners. Preferably, this mechanism would be agnostic as to whether a listener is written in C++ or JS. It should also be possible to set it up so that such a listener is automatically attached to all nodes of a given type or just a particular one or group.


### Important Constants and Definitions ###

`ClarityInstallURL`: global JS value that tells the system where to look for resources.


### Todo ###


1. Event API. We still need a framework to offer people a way to easily use events. Need to be able to use C++ or JS for event handling. 
   1. ~~CLElement.eventListenerGenerators needs to use the HybridNode descendent class names rather than HTML tags as keys since there might be more than one way to use a given HTML tag.~~ Actually the finalize() method in each descendent class is simpler and doesn't tend to create deep, complex maps of eventListenerGenerators.
2. Selectables could use a common base class.
   1. Radio buttons
   2. Checkboxes: We have a Checkbox class in Selectables now.
   3. Select boxes: Need to make a SimpleSelect class that uses a static option set and uses an underlying type representing the "value" component of `<option value="3" label="Toyota"></option>`. Initially this will be a single value. Multiselections can be vectors of those values. So picking Toyota *and* Ford in my Showcase.cpp test app would mean a vector or list with 1 and 2. A simple API can be used to modify the options if need be.

### Todo (Cleanups and "doing things right" fixes - Somewhat Lower Priority) ###
1. Go through and replace my improvised val(NULL) constructs with the (presumably more correct/efficient) val::nul()).


### Todo (Lowest Priority but probably more fun) ###
1. Go through attributes list at w3schools and just look for fun stuff to add to the project.


### Programming Notes ###

#### Data Flow Class Methods ####
_These are the current methods that move data around in the system. I'm documenting them here mostly for my own benefit so that I can think about a better nomenclature and simpler structure for them._


`HybridNode<V>::refreshDOMValueFromModel()`: Called by refresh() at beginning of GUI construction. Main purpose is to initialize the DOM value with the model val. Calls..

`virtual void HybridNode<V>::setDOMVal(const val &inval)`: Calls `ClarityNode::setDOMVal` but also dispatches value to peers.

`virtual void ClarityNode::setDOMVal(const val &inval)`: Only affects the DOM value.

`void HybridNode<V>::setCppValFromJSVal(const val &jsval)`: Writes the provided val object into the cppVal_ according to the template type using the emscripten as() method. At the end it calls pushValToPeers().

`virtual void HybridNode<V>::updateNodeFromDom()`: The main driver of the data flow. When the user uses a control, the method `static INLINE void ClarityNode::updateNodeFromDomById(int id)` gets called and starts the update process moving data "down" from the DOM into the modelVal and possible also "sideways" to the node's peers (if any).

