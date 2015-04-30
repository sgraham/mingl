MinGL is a minimal graphics/demo library.

MinGL's goals definitely do not include being the fastest graphics library, taking the best advantange of fancy hardware features, or showing off the latest neat rendering effects.

Instead, its goals are to be servicable to draw simple diagnostic scenes on many different platforms, while being distributed as merely one C++ header file, not requiring any configuration, and not having any external dependencies other than the standard platform libraries.

The API of MinGL's graphics is modelled closely after OpenGL ES 1.0. MinGL expects to have a floating point unit though, and drops all the fixed-related API functions and types from the ES spec. While MinGL is definitely _not_ a certified implementation of the OpenGL ES specification, it strives to conform to that specification nonetheless (again, minus the fixed point functions and types).

In addition to the graphics API there is some additional functionality to handle setting up rendering context, handling basic user input, basic "flycam", etc. This extra functionality is completely optional though, if you're only interested in the 'GL' part.