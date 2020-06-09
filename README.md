# CS 184 Fall 2016
Instructor: James O'Brien

# Implementer of This Project
<p align="center">
  <img width="256" height="256" src="render_snapshots/me.jpg">
</p>
<p align="center">
Wei Guo: wei.parker.guo.sg@gmail.com
</p>

# References (Books & Used Libs Mainly)
1. GLFW Version 3.2.1
2. GLEW
3. linmath.h under GLFW Dependencies
4. Code references are enclosed as inline comments.

# USAGE
1. ‘cd’ into your assignment directory, type into the Terminal the following step by step
2. mkdir build
3. cd build
4. cmake ..
5. make
6. run the as1.exe in build
7. if you wish to use custom setup for options, type the options line by line in a txt file in build and specify it when program opens.
8. there is a template options.txt inside src, you can copy, rename and modify it to start quicker.

# Keyboard features
1. 'ESC': Exit
2. 'Q': Exit
3. 'F': Full screen
4. '↓': Translate objects down
5. '↑': Translate objects up
6. '←': Translate objects left
7. '→': Translate objects right

# Render Results for Assessment 1 (Phong Sphere)

The picture below shows results of rendering with different shaders, given the parameters below each one.

<p align="center">
  <img width="960" height="1080" src="snaps_assembly.jpg">
</p>

# Features for Assessment 1

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
[NOTE] For efficiency pu now is an int, you can switch back to float if you edit all the fast_pow to regular power in assessment_1.cpp.

• -pl x y z r g b 
This adds a point light to the scene. The x y z values are the location of the light. The r g b values are it's color. Note that the x y z values are relative to the sphere. That is, the center of the sphere is at the origin and the radius of the sphere defines one unit of length. The Y direction is UP, the X direction is to the right on the screen, and the Z direction is "in your face." The r g b value are between 0 and max_float, NOT between 0 and 1 (that is, the r g b values encode the brightness of the light).

• -dl x y z r g b 
This adds a directional light to the scene. The x y z values are the direction that the light points in. The r g b values are it's color. See -pl for coordinate system notes.

# Additonal Features for Assessment 1

Command Line Options:

• -toon cc1-r cc1-g cc2-b cc2-r cc2-g cc2-b
This renders the sphere with a cartoon like artistic shader. cc1's rgb and cc2's rgb values need to be provided for cool and warm color blending.

• -toonl layer
This sets the layers of specular a toon can have, should be a positive nonzero interger.

• -toonl layer
This sets the layers of specular a toon can have, should be a positive nonzero interger.

• -transludepth n
This sets the depth of the subsurface beneath, should normally set to be a very small float between 0-1.

• -transludd n
This sets the depth decay of the subsurface reflection (how much light is going to be sent back), should be a float between 0-1.

• -transluds n
This sets the diameter of the simulated microfacet dipole on subsurface for light transport, should be a very small float.

• -sketch
This sets the render to use an experimental sketch shader that manifests the object as lines and silhouttes.
