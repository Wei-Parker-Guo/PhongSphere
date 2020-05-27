# CS 184 Fall 2016
Instructor: James O'Brien

# Implementer of This Project
Wei Guo: wei.parker.guo.sg@gmail.com

# References (Books & Used Libs Mainly)
GLFW Version 3.2.1
GLEW
linmath.h under GLFW Dependencies

# USAGE
1. ‘cd’ into your assignment directory, type into the Terminal the following step by step
2. mkdir build
3. cd build
4. cmake ..
5. make

# Keyboard features
1. 'ESC': Exit
2. 'Q': Exit
3. 'F': Full screen
4. '↓': Translate objects down
5. '↑': Translate objects up
6. '←': Translate objects left
7. '→': Translate objects right

# Features for Assessment 1 (Phong Sphere)

Command Line Options:

• -ka r g b 
This is the ambient color coefficients of the sphere material. The parameters r g b are numbers between 0 and 1 inclusive.

• -kd r g b 
This is the diffuse color coefficients of the sphere material. The parameters r g b are numbers between 0 and 1 inclusive. 

• -ks r g b 
This is the specular color coefficients of the sphere material. The parameters r g b are numbers between 0 and 1 inclusive. 

• -spu pu 
This is the power coefficient on the specular term in the u direction for an anisotropic material. 
It is a number between 0 and max_float.

• -spv pv 
This is the power coefficient on the specular term in the v direction for an anisotropic material. 
It is a number between 0 and max_float.

• -sp p 
This is the power coefficient on the specular term for an isotropic material. It is a number between 0 and max_float. (i.e. the same as setting pu and pv the the same value.)

• -pl x y z r g b 
This adds a point light to the scene. The x y z values are the location of the light. The r g b values are it's color. Note that the x y z values are relative to the sphere. That is, the center of the sphere is at the origin and the radius of the sphere defines one unit of length. The Y direction is UP, the X direction is to the right on the screen, and the Z direction is "in your face." The r g b value are between 0 and max_float, NOT between 0 and 1 (that is, the r g b values encode the brightness of the light).

• -dl x y z r g b 
This adds a directional light to the scene. The x y z values are the direction that the light points in. The r g b values are it's color. See -pl for coordinate system notes.