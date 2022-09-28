# Clarity

Clarity is a web development framework written in C++ using emscripten to facilitate web programming. Its purpose is similar to the React or Vue JavaScript frameworks. It allows you to connect aspects of a data model to web controls in a "reactive" fashion. This means that if the model changes, the web presentation reflects those changes and if the user alters the state of the web controls, the corresponding model elements will update. This is all meant to occur automatically, without the need to explicitly handle web events.

## Design

### An Interface is a Graph


### Three Component Model (Holy Trinity) ###

#### ControlNodes ####

#### DOM Elements ####

#### Translators ####











### TODO ###

1. Streamline and standardize 3-layer translation process
2. 




#### Some common cryptic compiler errors and quickfixes ####

`error: array 'new' cannot have initialization arguments return new T(v);`
You forgot to enclose a string literal argument in a 'val' value. The compiler thinks the string is some kind of char array.

