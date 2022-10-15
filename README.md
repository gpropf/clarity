# Clarity

Clarity is a web development framework written in C++ using emscripten to facilitate web programming. Its purpose is similar to the React or Vue JavaScript frameworks. It allows you to connect aspects of a data model to web controls in a "reactive" fashion. This means that if the model changes, the web presentation reflects those changes and if the user alters the state of the web controls, the corresponding model elements will update. This is all meant to occur automatically, without the need to explicitly handle web events.

## Design

### An Interface is a Graph

### Events ###



### Todo ###

1. ~~Find out why the 'name' field doesn't set in CLE.~~
3. ~~Clean up update functions and standardize nomenclature. Mostly done, but need docstrings.~~
4. ~~Collect global static data into a header or place at end of each header file so each new program doesn't need to create them.~~
5. ~~Delete or disable unimplemented features.~~
6. Set up real test template with standardized debug elements like a node audit button. - Started...
7. Event API. We still need a framework to offer people a way to easily use events.
8. ~~Figure out what actually needs to be inline~~
9. ~~Rework ModelNode as DOM-less CN and think through model-driven updates and relationship to threads.~~

### Todo (Somewhat Lower Priority) ###
1. Go through attributes list at w3schools and just look for fun stuff to add to the project.
