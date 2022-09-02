# Clarity

Clarity is a web development framework written in C++ using emscripten to facilitate web programming. Its purpose is similar to the React or Vue JavaScript frameworks. It allows you to connect aspects of a data model to web controls in a "reactive" fashion. This means that if the model changes, the web presentation reflects those changes and if the user alters the state of the web controls, the corresponding model elements will update. This is all meant to occur automatically, without the need to explicitly handle web events.

## Design

### An Interface is a Graph