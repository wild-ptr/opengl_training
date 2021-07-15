# opengl_training
Im just checking out how openGL works, along with learning some C because i know way too much C++ and not enough C.
As usual no build system, check Makefile for dependencies necessary for build and apt-get them (yuck!).
Im too lazy for doing CMake for my own personal projects, so please dont vomit.

Current features:
- rendering OBJ files loaded with assimp
- support for phong shading (yuck, but the PBR pipeline will be done in vulkan project.)
- support for normal mapping with tangent space vectors generated from obj.
- dynamic lighting with attenuation factors, point/directional/spotlights supported
- flashlight
- camera and rotating boxes

I will do all other features i want in my vulkan project, such as deferred shading with light volumes,
PBR IBL pipeline, shadow mapping and bunch of other features.

Basically my vulkan project will implement what is there, and much more.
