# Clarity

Clarity is a web development framework written in C++ using emscripten to facilitate web programming. Its purpose is similar to the React or Vue JavaScript frameworks. It allows you to connect aspects of a data model to web controls in a "reactive" fashion. This means that if the model changes, the web presentation reflects those changes and if the user alters the state of the web controls, the corresponding model elements will update. This is all meant to occur automatically, without the need to explicitly handle web events.

## Design

### An Interface is a Graph

### Events ###





### Important Constants and Definitions ###

ClarityInstallURL: global JS value that tells the systen where to looks for resources.


### Todo ###


1. Event API. We still need a framework to offer people a way to easily use events. Need to be able to use C++ or JS for event handling. 
   1. CLElement.eventListenerGenerators needs to use the HybridNode descendent class names rather than HTML tags as keys since there might be more than one way to use a given HTML tag.
2. Selectables could use a common base class.
   1. Radio buttons
   2. Checkboxes
   3. Select boxes 

### Todo (Cleanups and "doing things right" fixes) ###
1. Go through and replace my improvised val(NULL) constructs with the (presumably more correct/efficient) val::nul()).


### Todo (Somewhat Lower Priority) ###
1. Go through attributes list at w3schools and just look for fun stuff to add to the project.
