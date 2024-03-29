# Clarity Web Framework #
<img src="Clarity_Logo-magnifier-1280x843.png" alt="alt text" title="Clarity Web Framework" width="320" align="right"/>

## Preface (MUST READ) ##

As I developed this library and the example apps that use it I realized my original "node centric" approach was both too limited and overly complex. In a few cases I added some features that were, in my opinion, kludges just to maintain momentum in app development. The `cycleButton()` method in the `CLNodeFactory` class exists because sometimes the "one value / one node" pairing didn't quite express evhing we need to. As a result I've decide to create a new version of the library using an entirely new approach more in keeping with the pure functional reactive style. I have decided now (August-September 2023) to refactor the library again with the goal being to further simplify the programming interface and make the library even easier to develop with. I have also added a kind of "overseer" class (now called `AppBuilder`) that acts as both a factory class and runtime GUI manager. This new code is a separate branch called `cl3` and is present in a subdirectory of the same name. You will need to check out the `cl3` branch to see this directory. If you are planning on using the library please check out and build against the `cl3` branch ONLY! The current `master` branch implements the signal pipeline and other support classes in a way I do not intend to pursue further.

## Description ##

Clarity is a web development framework written in C++ using Emscripten to compile the code to WebAssembly (wasm). Its purpose is similar to the React or Vue JavaScript frameworks but with the idea that GUI logic will be written as much as possible in C++. It allows you to connect aspects of a data model to web controls in a "reactive" fashion. This means that if the model changes, the web presentation reflects those changes and if the user alters the state of the web controls, the corresponding model elements will update. This is all meant to occur automatically, without the need to explicitly handle web events. 

The name "Clarity" was chosen for its evocative and connotative qualities. We want to evoke the idea of being able to see your underlying C++ data structures clearly and in an up-to-the-minute fashion without needing to constantly press buttons to reload what you've been working on.  I also had existing C++ code in mind as I wrote this. I wanted something that would allow you to put an existing C++ program (say a large finite element simulation) on a website with as little modification to the existing data structures as possible. Thus we have a *clean break* between application code and presentation (GUI) code and there should not be any kind of translation layer obscuring the view so to speak. Those of you familiar with the Model-View-Controller software pattern should see how those terms can apply here. The Clarity framework aims to offer what the MVC pattern would call **View** and **Controller** functionality while leaving your **Model** design largely up to you.

## Design ##

NOTE: most of the classes described here are used in the current `master` branch, while I have shifted focus to the `cl3` branch. These classes have analogues in the `cl3` branch but they are often named somewhat differently.

There are basically 3 (three) components to the current Clarity system. Each component is exemplified by a class from which others are derived to express more complex behavior. There is a class called `WebElement` that represents an HTML tag. A number of classes are derived from this such as `InputElement`, `Label`, etc... whose names are fairly self-explanatory. The signals that move through the system are all derived from the `Signal<S>` class. The `SignalBuilder` class is a signal and HTML element factory that also acts as a connection helper facilitating the process of plugging the output of one signal into the input of another. One can think of `SignalBuilder` as a worker whose building materials are the `WebElement` and `Signal` classes.

### The Signal Pipeline(s)

NOTE: most of the classes described here are used in the current `master` branch, while I have shifted focus to the `cl3` branch. These classes have analogues in the `cl3` branch but they are often named somewhat differently.

The context for everything that happens here is the webpage. Webpages, of course, are made up of various HTML elements defined by various tags. The standard DOM model defines callbacks in JavaScript that are triggered by various events such as the user choosing an item from a select box, editing an input field, clicking a button, etc... This is all very much in the "imperative" programming style. When this happens, do that... We seek to replace this model with a more "declarative" one. In declarative programming, one does not so much tell the computer the answer to *how* questions as the answer to *what* questions. We essentially want to declare that this is connected to that, where this and that are elements on the web page but could also be objects in an application program. We want the system to know that a given input field is connected to another or to an application object so that when the user changes the field, the connected element is also changed. We also want the connection to work the other way so that if an application object changes state, the connected elements on the webpage are also updated. Users of React and similar frameworks will recognize this as a major motivating factor in their development. There is a bit more to this of course, especially considering that there is often a need to consider the *semantics* of page elements. For example, one input field may need to be interpreted as a floating point number while another is a string or an integer.

We can think of all the things we are connecting with each other as parts of a signal pipeline or pipelines. A given page may contain many such pipelines that may or may not be connected with each other. Just as in physical pipelines we will sometimes make use of specialized components that act to divide, merge, or transform the signals moving around the system. In fact, each of these three actions has a specialized class here devoted to performing exactly that, and only that, task. They are `MultiFork`, `Merge`, and `CppLambda`, respectively.

### Signals ###

NOTE: most of the classes described here are used in the current `master` branch, while I have shifted focus to the `cl3` branch. These classes have analogues in the `cl3` branch but they are often named somewhat differently.

The central primitive in the pipeline is the `Signal<S>` class which doesn't really do anything on its own. The template parameter `S` is the signal type and reflects the type of data we are dealing with at a given point. Immediately descended from `Signal<S>` are a pair of classes called `SignalAcceptor<S>` and `SignalEmitter<S>`. Signals always originate from a class descended from `SignalEmitter<S>` and are received by a class descended from `SignalAcceptor<S>`. Some signal classes are descended from both the emitter and acceptor class. These can be thought of as "passthru" or conduit classes that allow for signals to trace complex paths and undergo various transformations. An important example of this is the above mentioned `CppLambda<S>` class that can be used to convert from one signal type to another or just run a (pure) function on the signal data. 

Signal classes act as interfaces between HTML elements or C++ objects and the signal pipeline. `WebElementSignal` for instance is a wrapper around a `WebElement` such as an SVG area or input element. Creating a `WebElementSignal` automatically sets up JavaScript event listeners that are triggered as the user performs actions with the wrapped HTML element. These listeners inject user input into the signal pipeline or modify the wrapped HTML in response to signals coming through it.

There is also a pair of classes `ObjectAcceptor<S>` and `ObjectEmitter<S>` that are descended from `SignalAcceptor<S>` and `SignalEmitter<S>` respectively. These classes essentially perform the task of interfacing the signal pipeline with arbitrary C++ classes.

## Getting Started ##

There is already a small "showcase" type app called `signalTest` that allows you to see how data can move around a page using the new primitives and how controls can manipulate SVG elements. Just do `make signalTest` and then `emrun st.html` to view the page. If you keep the JavaScript console open while using this app you will see quite a lot of diagnostic information showing what's going on in the app.

## Examples

There is a large example app in the `examples/pixelreactor` subdirectory. There is a link at the top of the `st.html` demo page to it. I have not yet converted the app over to the new `cl3` branch but it should function the same when I do.

## Misc

The directory `examples/pixelreactor/Screenshots-from-original-app` shows the test patterns from the old PixelReactor app that should be tried to verify the behavior of the new app.

## TODO

1. The `Clarity.js` utility file contains stuff the only pertains to the example apps. This needs to be broken out into a separate file.
2. The GridControl, in addition to needing to be updated in the `cl3` branch, needs a mode where it uses a Canvas object rather than SVG.