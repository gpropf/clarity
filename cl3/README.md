# Clarity Web Framework #
<img src="Clarity_Logo-magnifier-1280x843.png" alt="alt text" title="Clarity Web Framework" width="320" align="right"/>

## CL3: Refactoring of the Third Kind! ##

I'm refactoring the code again. This time the goal is two-fold. First, we are going to address some things that were overlooked in the first versions such as the need to have values in the GUI update as they changed in the C++ objects and values they represent. Second, we are aiming to greatly simplify the process of using the library for the developer. I realized as I developed the Pixel Reactor app that the process of connecting GUI elements to C++ values is still too complex. To address both of these concerns we are going to develop a new class called `AppBuilder`. This class is sort of an enhanced version of the `SignalBuilder` class from the CL2 code. It will serve as a factory class and perform utility functions like its predecessor but will also remain an active part of the applications, acting as a watchdog and iteratively maintaining signal connections and consistency.


## Getting Started ##


## Misc

