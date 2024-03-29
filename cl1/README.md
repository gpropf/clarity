# Clarity Web Framework #
<img src="Clarity_Logo-magnifier-1280x843.png" alt="alt text" title="Clarity Web Framework" width="320" align="right"/>

## Preface ##

As I've developed this library and the example apps that use it I've realized that there are some limitations to my "node centric" approach that can't easily be worked into the existing framework. In a few cases I added some features that are, in my opinion, kludges just to maintain momentum in app development. The `cycleButton()` method in the `CLNodeFactory` class exists because sometimes the "one value / one node" pairing doesn't quite express everything we need to. In the next phase of development I plan to add in a set of classes and tools that are much more "value centric" rather than "node centric" and that express much more of a functional reactive programming style than what I've been doing so far. The existing library and code will remain though and I will continue developing it as needed but the eventual plan is to adopt this new more FRP style in future work.

Please see the `CL2` subdirectory for the new version of this library. It's pretty much a complete rethinking using the FRP style and doesn't rely on the previous codebase in fact. I feel this is a more productive line of development for the future. There is already a small "showcase" type app called `signalTest` that allow your to see how data can move around a page using the new primitives and how controls can manipulate SVG elements. Just change into the `CL2` directory and do `make signalTest` and then `emrun st.html` to view the page.

## Description ##

Clarity is a web development framework written in C++ using Emscripten to compile the code to WebAssembly (wasm). Its purpose is similar to the React or Vue JavaScript frameworks but with the idea that GUI logic will be written as much as possible in C++. It allows you to connect aspects of a data model to web controls in a "reactive" fashion. This means that if the model changes, the web presentation reflects those changes and if the user alters the state of the web controls, the corresponding model elements will update. This is all meant to occur automatically, without the need to explicitly handle web events. 

The name "Clarity" was chosen for its evocative and connotative qualities. We want to evoke the idea of being able to see your underlying C++ data structures clearly and in an up-to-the-minute fashion without needing to constantly press buttons to reload what you've been working on.  I also had existing C++ code in mind as I wrote this. I wanted something that would allow you to put an existing C++ program (say a large finite element simulation) on a website with as little modification to the existing data structures as possible. There is thus a *clean break* between application code and presentation (GUI) code and there should not be any kind of translation layer obscuring the view so to speak. Those of you familiar with the Model-View-Controller software pattern should see how those terms can apply here. The Clarity framework aims to offer what the MVC pattern would call **View** and **Controller** functionality while leaving your **Model** design largely up to you.

## Getting Started ##

The best place to start is the existing example programs. Doing `make showcase` should build the main example program which contains working examples of the types of nodes implemented so far. We have a simple convenience and testing framework defined in `PageContent.hpp` based on an abstract class called `PageContent`. There is also a testing framework class called `TestFramework` which also descends from `PageContent`. The purpose of this class is to function as a sort of debugging dashboard. Currently it only has a "List Nodes" button which can be quite useful as it prints the details of any nodes you've created. The actual "meat" of the showcase app, for instance, is in the `Showcase` class defined in `Showcase.hpp` which is then instantiated by the program in `showcase.cpp`. You aren't required to use this framework however and your code will work fine if you don't.

There is also `speedtest.cpp` which basically just creates and destroys a large number of nodes. This is both to monitor performance and also to test the system's ability to avoid memory leaks and segfaults (well, memory overruns since Emscripten doesn't really have segfaults). The basic rule is that any CPP file that starts with a lowercase letter is a runnable program. Files that start with an uppercase letter contain class definitions or implementations. The only exceptions to this are the files `clarity.cpp` and `clarity.hpp`, which contain utility code and namespace code that doesn't really belong to any particular class.

I've also been pretty good about documenting as I go and there are a lot of Doxygen docstrings. If you do `make docs` you'll get a detailed reference on the API and some auto-generated class diagrams.

### But how do I make a model of a nuclear power plant that you can interact with? ###

Or some such question. Well, I don't know what exactly you want to model here but the basic plan of attack and rules of engagement are these...

1. Clarity can only "see" things that are on the heap. If you create a model that is created within a function and is destroyed when it ends there's no way to attach web controls to it. Basically, your model needs to be persistent and visible within the `main()` function of your program.
2. Figure out what you plan to expose to the user and create Clarity nodes of whatever type is appropriate to manipulating that part of the data model. When creating these controls you will invoke the `withCppVal()` method of the `CLNodeFactory` class. The argument to this method will be a pointer to whatever data you want to make accessible to the user. When the user changes a value in the web control the actual value in your model should change as well. 
   
   One potential problem here is that you may want to allow the user to manipulate the value of a data member that does not have `public` access. Eventually we may have a way to install designated getter and setter methods for these kinds of member. Currently though, there are three solutions:
   1. When creating your special purpose subclass of `HybridNode`, make it a `friend` of the data type it will contain.  You can see this approach in the `Pixelreactor` demo app where the `BeakerNode` class is a `friend` of the `Beaker` class.
   2. Create a method that returns a pointer to the member. You can see this approach in the `CanvasGrid` class in this project with the `getPtr2CurrentCellVal()` method. This method is what we use in the `showcase` demo and elsewhere to make it possible to change the color.
   3. Finally, there's always the option to just declare `public` access to the member you need to expose.

3. To "close the loop" and have the model update the interface you will call the `refresh()` method of the relevant nodes when your model updates itself. The usual method for this is still to use JavaScript's setInterval and setTimeout methods to periodically iterate your model and refresh the relevant Clarity nodes. I'd like to make use of web workers or Emscripten's thread capability for this eventually.

### Complex Controls for Complex Objects ###

From the beginning I intended for it to be possible to build complex UI components for complex model sub-components. Let's say your model nuclear plant has a C++ class called `Reactor`. This represents a single complete reactor with associated dials, buttons, and internal states. Perhaps you want to be able to have multiple reactors though with each one having its own GUI module on the screen to control it. In this case you would figure out how to set up the control elements for a single reactor as a web app. Once you have that working, you create a subclass of `HybridNode` with an underlying model element of type `Reactor`, so --> `ReactorNode<Reactor>`. You can see this exact pattern in use in the Clarity Demo Apps repo discussed below.

### Clarity Demo Apps ###

I've also created a completely separate repo called [clarity-apps](https://github.com/gpropf/clarity-apps) that contains some Clarity demo apps. Actually, there's not much there yet so right now this is just intended as a sanity check to make sure that people can actually create apps **outside** this repository. We want it to be straightforward to make a Clarity app without getting a console window full of angry red 404 messages, compilation errors, and runtime exceptions caused by hidden assumptions about file paths and other Clarity internals. 

This repo should be a good place to start making your own Clarity app by just copying and then gutting one of the demo apps and putting your own content in. It also already contains an example of a complex control in the form of `BeakerNode` in the _Pixel Reactor_ demo. The initial problems I had with connecting `Beaker` and `BeakerNode` have been resolved though currently the `Beaker` class doesn't do much. I'm actively working on this demo though as it will feature prominently on my new homepage.

### A Word on Timers ###

It looks like, for the foreseeable future, we will still be relying on JavaScript's `setTimeout()` and `setInterval()` method to simulate continuous processes. The problem with using these methods from C++ is that the function you specify as an argument to the call must be a JavaScript function. If the function you want to call repeatedly is actually a method of a C++ object you have the additional problem of passing a C++ object and then calling one of its methods from JavaScript. Obviously, you'll need to define bindings for your C++ class and the method you want to use. I've created a few wrapper methods for the basic timer functions that can help make this process less painful assuming you have the necessary bindings. The methods are found in the Util.js file as static methods of the `Util` JavaScript class. The methods are called `setIntervalForObjectWithNamedMethod()` and  `setTimeoutForObjectWithNamedMethod()`. They allow you to specify the name of the iteration method as a string and then look it up and use the JS `apply()` function to avoid the inefficiencies and security problems of `eval()`.

## Design ##

Setting up responsive web components that allow an end user to interact smoothly with a data model while presenting the api user with an easy to use programming toolkit is a hugely complex task. Taking into account the fact that C++ is a strongly typed language multiplies the potential number of controls still further. The interactions with JavaScript's different and much weaker type system adds another dimension of complexity. 

The current system is the result of several complete refactorings over the Summer of 2022 that I undertook as I realized the shortcomings of each successive approach. The current system basically uses several approaches rather than a single programming paradigm. We do not try to extract all the needed functionality from either an object hierarchy or a system of template functions. Thus, there are OO elements such as the fact that I implement some of the more complex controls (like the CanvasGrid) as child classes of more generalized ones. However, I also use templates extensively to allow C++ types to be used directly in web controls. Finally, some of the variations in functionality and implementations that I originally thought would go into classes are instead implemented as methods in the factory class. The `<label>` and `<button>` tags, for instance, are implemented as factory methods rather than as descendant classes of HybridNode. The use of the various "with" methods makes it possible for the factory to implement many tag-specialized behaviors and parameters that would otherwise require properties in a descendant class. Thus there is no "Button" or "Label" class.

__JavaScript__

I've tried to keep most of the functionality in the C++ code but when dealing with things like event listeners, some JavaScript is unavoidable. There is one main JavaScript file called `JSProxyNode.js`. It contains a class called `JSProxyNode` that acts as a kind of "mirror image" of `ClarityNode` in the JS world. It exists to carry out the system's tasks for a given node on the JS side. There's also `Selectables.js` which contains some event listener generators that support the C++ classes in `Selectables.cpp`. There's also `Util.js` which does what the name suggests it might.

### Event Listener Generators

One idiom that I make use of throughout the project is that of the JavaScript event listener generator. This is a function that generally takes a bound C++ or plain JavaScript object as an argument and returns an event listener function. These functions are useful for encapsulating object behavior in web controls. The best example being button event listeners which fire when the user clicks the button. Every text or other input control, however, has its own event listener that is manufactured for it by one of these generator functions. There are several static objects called `listenerGenerators` in `JSProxyNode.js` and other JavaScript files that organize these functions according to C++ class, HTML tag, and input type, in that order. When an input control is created a method called `installEventListenersByTagAndType` in `JSProxyNode` is run to equip it with the appropriate listener.

### The C++ Object Hierarchy ###

**`ClarityNode`**: Base class for all nodes. Implements most of the system's basic behavior. Does *not* contain a model pointer, and cannot be instantiated due to its pure virtual methods.

**`HybridNode<V>`**: The first child class of ClarityNode. The template parameter determines the type of the model pointer. Most nodes are implemented as a HybridNode with the `tag_` member telling us what kind of HTML element to create for it. The "hybrid" part of the name refers to the fact that the earliest versions of Clarity had a special type of node that just held the C++ model data but had no associated DOM element while all others dealt exclusively with the JavaScript DOM side of things. Today, all nodes can both store C++ data *and* interact with the DOM, hence they are hybrids of those earlier types.

**`CanvasElement<V>`**: Descends from `HybridNode<V>`. Not currently used in the examples and meant more as a generic base class for `<canvas>` nodes that might implement games or other types graphical elements.

**`CanvasGrid<V>`**: Descends from `CanvasElement<V>`. Uses an HTML5 `<canvas>` element to display a user-modifiable grid of pixels. I actually created this child class specifically for an app I'm planning to port from my old website and also to flesh out the particulars of implementing complex child classes that handle extended model data, in this case a large array representing the pixel values.

**`Select<V>`**: Descends from `HybridNode<V>`. Selectable dropdown option lists. I think I'm going to make several variants of this since a simple select box doesn't need that same complexity as a multi-select with optgroup tags. The current `Select` class is actually rather useless since it doesn't store the current _selection_.

**`Checkbox<V>`**: Descends from `HybridNode<V>`. Your basic checkable box. My example implementation uses V = bool but other types could work too.

---

### Terminology ###

**Node**
: a single GUI element generally created using a node factory. Nodes contains a C++ pointer whose type is determined by the template parameter. Note that the pointer can be null but its type will be used to determine how data is handled in that node.

**Node Factory**
: Due to the large number of factors that come into play in creating nodes, we set up factory objects (using the `CLNodeFactory` class) that store certain persistent parameters relevant to setting up nodes. The most important of these parameters are the template parameters that determine the C++ data types of the underlying data. The `CLNodeFactory` class has a number of methods whose names start with "with" such as `withName()`. Use these methods to construct your interface. There is also a constructor that allows you to create a new `CLNodeFactory` from an existing one but with totally different template parameters. You can see this in use in several places in the example code.

**Peer**
: Nodes can have relationships with other nodes that result in automatic data movement and updating when one node is changed. This can be either due to the end user or the model changing something. A node that has such a relationship with another is called a peer.

**Data Link**
: These are the 'edges' mathematically in our graph. They can not only move but transform data in the process.

**Model**
: The underlying C++ data. Nodes contain pointers to model values. Even when the model pointer is null its type is used to determine what kind of data a given node is expected to store.

### An Interface is a Graph ###

Nodes can be connected together using data links and updating one node can trigger others to update through these links. We can thus regard any Clarity app mathematically as a graph. I've attempted to take into account the differences between cyclic and acyclic graphs in the various update methods but this is limited right now to marking nodes with a boolean "clean/dirty" flag so that data will not bounce between nodes and create infinite loops. A future direction here would be to have page validation methods that would produce warnings or errors due to the presence of graph cycles or conflicting update rules.

### Events ###

When you create a node the system will install a set of event listeners that are designed to implement the movement of data from the model to and from the GUI. We also need a way for api users to add their own event listeners. Preferably, this mechanism would be agnostic as to whether a listener is written in C++ or JS. It should also be possible to set it up so that such a listener is automatically attached to all nodes of a given type or just a particular one or group.

### Todo ###

1. Reorganizing and cleaning up this README!
   
2. We need some way to register getter and setter methods for model data that is inside classes and has non-public access. Not everyone can modify their model classes to add `friend` declarations and the use of `friend` is frowned upon in any case.

3. `CanvasGrid` needs to have a more generic type for its color selection. Right now I have the 8 colors for the `PixelReactor` app in clarity-apps hardcoded in as the color choices for all CanvasGrids. This violates the principle of separating framework and app logic.

3. ~~Event API~~. I now have the ClarityNode::installListenerGenerators() static method to allow the user to replace or augment the standard event listeners in their programs.

4. ~~Speedtest needs to be put in the standard format used in Showcase.~~

5. ~~Wasm workers, pthreads, or both.~~ As of right now (first week in December 2022) it seems that Wasm Workers will not easily run code that uses the DOM and the DOM is also confined to the main thread so that pthreads will not be able to touch it either. These considerations would seem to preclude using a pthread or Wasm Worker to directly take the place of `setInterval()/setTimeout()` for things like iterating simulations. However, using a thread to run the calculations should work. We would need to use `setInterval()` to refresh the view while the pthread or WW does the calculations in the background.

6. ~~The template parameterized multiplier value in DualLink should be stored so it can be the target of user interaction. Basically, you could edit the conversion factor for some peer value.~~ This is partially implemented but should not currently be relied upon.
   
7. Need support for radio buttons in CLNF.
   
8. ~~Checkboxes~~: We have a Checkbox class in Selectables now.

9.  Selectables could use a common base class?
   
10. Select boxes: SimpleSelect class now uses a single value to represent the current selection and has eventListeners and other constructs to update that value as the user changes their selection.

### Todo (cleanups and "doing things right" fixes - somewhat lower priority) ###

1. Go through and replace my improvised val(NULL) constructs with the (presumably more correct/efficient) val::nul()).
2. Need to make sure that runStateFunction() is called in all places where it needs to be. This could also shine some more light on the data flow.

### Todo (Lowest priority but probably more fun) ###

1. Go through attributes list at w3schools and just look for fun stuff to add to the project.

### Future Directions ###

#### Cloud Filesystem Hooks ####

I'd like to provide a way for people to save things they create using Clarity apps to the cloud. My plan is to start with [NextCloud](https://nextcloud.com/).


#### Interface Builder ####

The `TestFramework` class could become a full blown interface builder. The idea is to use SVG to create a graphical representations of the nodes and have click and drag capability allowing you to create and destroy data links. An "emit source" button would allow you to produce compile-able C++ code once you get things working. While thinking about representing each node as a little circle or something in SVG I realized that we would need to solve a more general problem which is how to maintain "node mappings" of collections of things to collections of nodes. Things like the select boxes with each option being an actual Option node are an obvious application. Basically there needs to be a sort of Clarity event system with events like "nodeCreated" and "nodeDestroyed" that would fire when things changed in the data. Keeping the factories around and allowing them to respond to and generate these events might be necessary.

##### Merge Switchboard and Factory? #####

The "switchboard" static map in `ClarityNode` was an early hack that I came up with when I couldn't work out how to store a pointer to the `ClarityNode` that owned each `JSProxyNode` in that `JSProxyNode` even using allow_raw_pointers(). My initial plan had always been to store such a pointer so that things like eventListeners could directly alter the C++ data. I eventually figured it out but the switchboard remains useful for things like the node list method and is now a pretty low level part of the system.  It might be a good idea to keep the switchboard concept alive but have each `CLNF` store a pointer to all the nodes it builds. When we use a factory to create a new node this can potentially send messages to other nodes it has built. The example of adding an option to a select box again comes to mind but this could also be used to update things like the proposed interface builder above.

##### ActiveVector #####

The `ActiveVector` class is an experiment in something along the lines discussed just above. The basic idea is to maintain a set of nodes in sync with a collection of some other objects. The goal is to have something that could, for instance, maintain a set of `<option>` tags in sync with a list of people or cars or whatever. There's a small program called `avtest.cpp` that demonstrates this class. Just do `make avtest` to see what it does.

#### Dynamic CSS

Basically something like a CSS node type so we can have styling information built into the page.

#### Automated Testing ####

There is now a set of classes that do basic testing on a few of the most common types of controls. To run the tests just build the `unittest` make target and look at the page it produces. Fields that pass their tests are in green, the ones that fail turn red. The relevant files here are `Unittest.hpp`, `Unittester.hpp`, `Unittest.js`, and `unittest.cpp`.


#### Web Templates? ####

There is of course the possibility of developing something akin to React's JSX pages for Clarity (CSX pages?) but the problem with creating a novel template language is that editors will not initially support it. Losing autocompletion or automatic syntax checking is a pretty high price to pay just to have your C++ code and HTML/CSS all together in one place. Maybe if the library gets enough mindshare that people are willing to write VSCode or Vim/Emacs extensions for it...

---
### Programming Notes ###

#### Coding Conventions ####

* Variable names follow the usual camel case style used in C++
* All of the C++ library code is contained within the `clarity` namespace.
* Brackets: the first bracket is on the same line as the method name.
* Member variables: member vars end with an underscore in both C++ and JS code.
* In some places I've used underscore to indicate that what follows is some sort of type information
  so you have things like "_tinp" for "text input".
  
#### Source Code Files ####

My initial plan was to try to avoid the "header-only library" style that has become popular today and is exemplified by Boost and other popular libraries. I started to realize that using a lot of template metaprogramming made this almost impossible and led to constant struggles with the linker. So basically I now have almost all the code in hpp files. The cpp files are basically where template specializations live. A great deal of the behavior of the library is due to these specializations though and in particular, this is how nodes "know" what type they are even if they have a null in `cppVal_`.


#### Data Flow Class Methods ####
_These are the current methods that move data around in the system. They went through several reworkings as I refactored the system. I'm documenting them here mostly for my own benefit so that I can think about a better nomenclature and simpler structure for them._


`HybridNode<V>::refresh()`: Should be called whenever a node is updated, either by the user or through automated state changes in the data it represents. Propagates changes to both peers and children.

`HybridNode<V>::refreshDOMValueFromModel()`: Called by refresh() at beginning of GUI construction. Main purpose is to initialize the DOM value with the model val. Calls..

`virtual void HybridNode<V>::setDOMVal(const val &inval)`: Calls `ClarityNode::setDOMVal` but also dispatches value to peers.

`virtual void ClarityNode::setDOMVal(const val &inval)`: Only affects the DOM value.

`void HybridNode<V>::setCppValFromJSVal(const val &jsval)`: Writes the provided val object into the cppVal_ according to the template type using the emscripten as() method. At the end it calls pushValToPeers().

`virtual void HybridNode<V>::updateNodeFromDom()`: The main driver of the data flow. When the user uses a control, the method `static INLINE void ClarityNode::updateNodeFromDomById(int id)` gets called and starts the update process moving data "down" from the DOM into the modelVal and possibly also "sideways" to the node's peers (if any). Actually, now that I've solved the problem with storing a pointer to a subclass of ClarityNode in JSProxyNode, we just call the updateNodeFromDom() method of each node directly from the JavaScript event listener. Eventually the static method that uses the node id will be phased out.


#### Troubleshooting Common Problems

If you see a BindingError like this on loading a page you've created the issue is that you don't have an embinding for the _specific_ node type you've instantiated.


```
{
    "name": "BindingError",
    "message": "_emval_take_value has unknown type PN7clarity8CheckboxIbEE",
    "stack": "BindingError: _emval_take_value has unknown type PN7clarity8CheckboxIbEE\n
    ...
}
```

The 4 letters at the end of the type actually tell you exactly what's missing. In this case the most relevant part is the 'b' which means type `bool`. So essentially we didn't have a Checkbox<bool> type defined anywhere.

#### Important Constants and Definitions ####

~~`ClarityInstallUrl`: global JS value that tells the system where to look for content including its own boot scripts. In the examples I set this inside a `<script>` tag in the HTML template file.~~
~~`JSAuxUrl`: global JS value that tells the system where to look for auxilliary scripts such as user-defined JS modules containing custom listener generators, etc...~~

I've decided to phase out this whole "bootstrap" process in favor of letting people add whatever auxilliary JS they need as `--pre-js` or `--post-js` options. There's no sense in duplicating a feature that Emscripten already includes and I feel that locating and loading the extra JavaScript files was error prone. The goal now is for the core Clarity library to consist of just a JavaScript file and the corresponding WASM file.

#### Known Bugs

* Setting `engulfForNode` to true when using a label causes problems when the thing being engulfed is an SVG area or attributeNode. Probably because the label "yanks away" the SVG components from their rightful parent nodes. Seems to work fine for other controls.
* CanvasGrid uses an int for the cell width and height but this is derived by dividing the CG width in pixels by its width in cells. So if you have a canvas that is 210 pixels wide but 20 cells wide, the cells get rounded to 10px and then don't fill the whole width of the allocated canvas.
* Not exactly a bug but if you delete a node you need to explicitly do a `removeChild()` call on the node's parent (if any). If the parent is later deleted itself, the dangling pointer will get double-deleted and cause a runtime error.