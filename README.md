# Clarity Web Framework #
<img src="Clarity_Logo-magnifier-1280x843.png" alt="alt text" title="Clarity Web Framework" width="320" align="right"/>

## Preface ##

As I developed this library and the example apps that use it I realized my original "node centric" approach was both too limited and overly complex. In a few cases I added some features that were, in my opinion, kludges just to maintain momentum in app development. The `cycleButton()` method in the `CLNodeFactory` class exists because sometimes the "one value / one node" pairing didn't quite express everything we need to. As a result I've decide to create a new version of the library using an entirely new approach more in keeping with the functional reactive style.

## Description ##

Clarity is a web development framework written in C++ using Emscripten to compile the code to WebAssembly (wasm). Its purpose is similar to the React or Vue JavaScript frameworks but with the idea that GUI logic will be written as much as possible in C++. It allows you to connect aspects of a data model to web controls in a "reactive" fashion. This means that if the model changes, the web presentation reflects those changes and if the user alters the state of the web controls, the corresponding model elements will update. This is all meant to occur automatically, without the need to explicitly handle web events. 

The name "Clarity" was chosen for its evocative and connotative qualities. We want to evoke the idea of being able to see your underlying C++ data structures clearly and in an up-to-the-minute fashion without needing to constantly press buttons to reload what you've been working on.  I also had existing C++ code in mind as I wrote this. I wanted something that would allow you to put an existing C++ program (say a large finite element simulation) on a website with as little modification to the existing data structures as possible. Thus we have a *clean break* between application code and presentation (GUI) code and there should not be any kind of translation layer obscuring the view so to speak. Those of you familiar with the Model-View-Controller software pattern should see how those terms can apply here. The Clarity framework aims to offer what the MVC pattern would call **View** and **Controller** functionality while leaving your **Model** design largely up to you.

## Design ##

There are basically 3 (three) components to the current Clarity system. Each component is exemplified by a class from which others are derived to express more complex behavior. There is a class called `WebElement` that represents an HTML tag. A number of classes are derived from this such as `InputElement`, `Label`, etc... whose names are fairly self-explanatory. The signals that move through the system are all derived from the `Signal<S>` class. The `SignalBuilder` class is a signal and HTML element factory that also acts as a connection helper facilitating the process of plugging the output of one signal into the input of another. One can think of `SignalBuilder` as a worker whose building materials are the `WebElement` and `Signal` classes.

### Signals ###

The new central primitive is the `Signal<S>` class. Immediately descended from that are a pair of classes called `SignalAcceptor<S>` and `SignalEmitter<S>`. Signals originate from a class descended from `SignalEmitter<S>` and terminate at a class descended from `SignalAcceptor<S>`. The template parameter `S` is the signal type and reflects the type of data we are dealing with at a given point. There is a `CppLambda<S>` class that can be used to convert from one signal type to another or just run a (pure) function on the signal data. 

Signal classes act as interfaces between HTML elements or C++ objects and the signal pipeline. `WebElementSignal` for instance is a wrapper around a `WebElement` such as an SVG area or input element. Creating a `WebElementSignal` automatically sets up event listeners that are triggered as the user performs actions with the wrapped HTML element. These listeners inject user input into the signal pipeline or modify the wrapped HTML in response to signals coming through it.

There is also a pair of classes `ObjectAcceptor<S>` and `ObjectEmitter<S>` that are descended from `SignalAcceptor<S>` and `SignalEmitter<S>` respectively. These classes essentially perform the task of interfacing the signal pipeline with arbitrary C++ classes.

## Getting Started ##

There is already a small "showcase" type app called `signalTest` that allows you to see how data can move around a page using the new primitives and how controls can manipulate SVG elements. Just do `make signalTest` and then `emrun st.html` to view the page. If you keep the JavaScript console open while using this app you will see quite a lot of diagnostic information showing what's going on in the app.