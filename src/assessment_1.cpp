//****************************************************
// Starter code for assignment #1.  It is provided to 
// help get you started, but you are not obligated to
// use this starter code.
//****************************************************

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

//include header file for glfw library so that we can use OpenGL
#include <windows.h> //this has to be done because my system does not have FREETYPE, delete this line if platform is otherwise
#include "linmath.h" //include GLFW's linear math for vector manipulation
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifdef _WIN32
static DWORD lastTime;
#else
static struct timeval lastTime;
#endif

#define PI 3.14159265 // Should be used from mathlib

using namespace std;

//****************************************************
// Global Variables
// Generally speaking, global variables should be 
// avoided, but for this small assignment we'll make
// and exception.
//****************************************************

GLfloat Translation[3] = {0.0f, 0.0f, 0.0f};
bool Auto_strech = false;
int  Width_global = 400;
int  Height_global = 400;

int  SizeX_saved_global;
int  SizeY_saved_global;

int  PosX_saved_global;
int  PosY_saved_global;

const GLFWvidmode * VideoMode_global = NULL;

inline float sqr(float x) { return x*x; }

/* generate lambertian shade with another light source
 * equation: c = cr(ca + clmax(0, n.l))
 */
void gen_lambert_shade(const vec3 ca, const vec3 cr, const vec3 cl, const vec3 n, vec3 l, vec3 c){
    //normalize light vector
    vec3_norm(l, l);

    //reflectance
    float r = max((float)0, vec3_mul_inner(n,l));
    vec3_scale(c, cl, r);
    vec3_add(c, c, ca);
    //apply reflect fraction
    c[0] = c[0] * cr[0];
    c[1] = c[1] * cr[1];
    c[2] = c[2] * cr[2];
}

//****************************************************
// Simple init function
//****************************************************

void initializeRendering()
{
    glfwInit();
}

//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

void setPixel(float x, float y, GLfloat r, GLfloat g, GLfloat b) {
    glColor3f(r, g, b);
    glVertex2f(x+0.5, y+0.5);  
    // The 0.5 is to target pixel centers
    // Note that some OpenGL implementations have created gaps in the past.
}

//****************************************************
// Keyboard inputs
//****************************************************

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key) {
                    
        // Hint on making up/down left/right work: the variable Translation [0] and [1].
                
        case GLFW_KEY_ESCAPE: 
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break; 
        case GLFW_KEY_Q:      
            glfwSetWindowShouldClose(window, GLFW_TRUE); 
            break;
        case GLFW_KEY_LEFT :
            Translation[0]-=5;
            break;
        case GLFW_KEY_RIGHT:
            Translation[0]+=5;
            break;
        case GLFW_KEY_UP   :
            Translation[1]+=5;
            break;
        case GLFW_KEY_DOWN :
            Translation[1]-=5;
            break;
        case GLFW_KEY_F:
            if (action) {
                Auto_strech = !Auto_strech;                 
                if (Auto_strech){
                    glfwGetWindowPos(window, &PosX_saved_global, &PosY_saved_global);
                    glfwGetWindowSize(window, &SizeX_saved_global, &SizeY_saved_global);
                    glfwSetWindowSize(window, VideoMode_global->width, VideoMode_global->height);
                    glfwSetWindowPos(window, 0, 0);
                }else{
                    glfwSetWindowSize(window, SizeX_saved_global, SizeY_saved_global);
                    glfwSetWindowPos(window, PosX_saved_global, PosY_saved_global);
                }
            }
            break;
        default: 
            break;
    }
    
}



//****************************************************
// Draw a filled circle.
//****************************************************

void drawCircle(float centerX, float centerY, float radius) {

    // Start drawing a list of points
    glBegin(GL_POINTS);

    // We could eliminate wasted work by only looping over the pixels
    // inside the sphere's radius.  But the example is more clear this
    // way.  In general drawing an object by loopig over the whole
    // screen is wasteful.

    int minI = max(0,(int)floor(centerX-radius));
    int maxI = min(Width_global-1,(int)ceil(centerX+radius));

    int minJ = max(0,(int)floor(centerY-radius));
    int maxJ = min(Height_global-1,(int)ceil(centerY+radius));

    for (int i = 0; i < Width_global; i++) {
        for (int j = 0; j < Height_global; j++) {

            // Location of the center of pixel relative to center of sphere
            float x = (i+0.5-centerX);
            float y = (j+0.5-centerY);

            float dist = sqrt(sqr(x) + sqr(y));

            if (dist <= radius) {

                // This is the front-facing Z coordinate
                float z = sqrt(radius*radius-dist*dist);

                //get normalized normal for this point
                vec3 norm = {x, y, z};
                vec3_norm(norm, norm);

                //generate lambertian shade
                vec3 ca = {0.05,0.05,0.05};
                vec3 cl = {1, 1, 1};
                vec3 l = {1, 1, 1};
                vec3 cr = {0, 0.5, 0.8};
                vec3 c;

                //make light static (irrespondent to translation) for more interesting effects
                //(This won't make sense for directional light conceptually, thus disabled for it by default)
                l[0]+=Translation[0];
                l[1]+=Translation[1];

                gen_lambert_shade(ca, cr, cl, norm, l, c);

                setPixel(i, j, c[0], c[1], c[2]);

                // This is amusing, but it assumes negative color values are treated reasonably.
                // setPixel(i,j, x/radius, y/radius, z/radius );
                
            }
        }
    }

    glEnd();
}

//****************************************************
// function that does the actual drawing of stuff
//***************************************************

void display( GLFWwindow* window )
{
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );      //clear background screen to black
    
    glClear(GL_COLOR_BUFFER_BIT);                // clear the color buffer (sets everything to black)
    
    glMatrixMode(GL_MODELVIEW);                  // indicate we are specifying camera transformations
    glLoadIdentity();                            // make sure transformation is "zero'd"
    
    //----------------------- code to draw objects --------------------------
    glPushMatrix();
    glTranslatef (Translation[0], Translation[1], Translation[2]);
    drawCircle(
        Width_global / 2.0 , 
        Height_global / 2.0 , 
        min(Width_global, Height_global) * 0.8 / 2.0);  // present a sphere with max radius possible on screen
    glPopMatrix();
    
    glfwSwapBuffers(window);
    
}

//****************************************************
// function that is called when window is resized
//***************************************************

void size_callback(GLFWwindow* window, int width, int height)
{
    // The width and height arguments are not used
    // because they are not the size of the window 
    // in pixels.

    // Get the pixel coordinate of the window
    // it returns the size, in pixels, of the 
    // framebuffer of the specified window
    glfwGetFramebufferSize(window, &Width_global, &Height_global);
    
    glViewport(0, 0, Width_global, Height_global);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, Width_global, 0, Height_global, 1, -1);
    
    display(window);
}


//****************************************************
// the usual stuff, nothing exciting here
//****************************************************


int main(int argc, char *argv[]) {

    //take user input
    char buffer[20];
    printf("Enter Options: ");
    scanf(" %[^\n]", buffer);
    printf(buffer);

    //This initializes glfw
    initializeRendering();
    
    GLFWwindow* window = glfwCreateWindow( Width_global, Height_global, "Computer Graphics", NULL, NULL );
    if ( !window )
    {
        cerr << "Error on window creating" << endl;
        glfwTerminate();
        return -1;
    }
    
    VideoMode_global = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if ( !VideoMode_global )
    {
        cerr << "Error on getting monitor" << endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent( window );

    size_callback(window, 0, 0);
    
    glfwSetWindowSizeCallback(window, size_callback);
    glfwSetKeyCallback(window, key_callback);
                
    while( !glfwWindowShouldClose( window ) ) // main loop to draw object again and again
    {
        display( window );

        glfwPollEvents();        
    }

    return 0;
}







