//****************************************************
// Starter code for assignment #1.  It is provided to 
// help get you started, but you are not obligated to
// use this starter code.
//****************************************************

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

//include header file for glfw library so that we can use OpenGL
#ifdef _WIN32
/* this has to be done because my system does not have FREETYPE, delete this line if platform is otherwise
STRONGLY NOT RECOMMENDED for GLFW setup */
#include <windows.h>
#endif
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

#define PI 3.14159265f // Should be used from mathlib

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

//parameters
bool using_phong = true;
bool using_WARD = false;
vec3 ca = {0.05f,0.05f,0.05f}; //default ambient
vec3 cr = {0.9f, 0.5f, 0.5f}; //default diffuse
vec3 cp = {0.9f,0.5f,0.5f}; //default specular
float pu = 0.1f; //default pu for anisotropic shade
float pv = 0.2f; //default pv for anisotropic shade
int p = 64; //default p for isotropic shade
vec3 dls[5]; //default directional light list
vec3 dl_cs[5]; //dedault directional light color list
vec3 pls[5]; //default point light list
vec3 pl_cs[5]; //default point light color list

const GLFWvidmode * VideoMode_global = NULL;

inline float sqr(float x) { return x*x; }

/* This function gets the direction and color stored in a particular light in a size 5 array. */
void get_light(const vec3 dir_list[], const vec3 c_list[], const int light_n, vec3 dir, vec3 c){
    dir[0] = dir_list[light_n][0];
    dir[1] = dir_list[light_n][1];
    dir[2] = dir_list[light_n][2];
    c[0] = c_list[light_n][0];
    c[1] = c_list[light_n][1];
    c[2] = c_list[light_n][2];
}

/* A faster function for square root. ONLY WORKS for 32 bit float, should be ok for most platforms.
This is directly taken from the url below, and used only for low-precision scenarios in this scope:
https://www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Function-Precisi 
Not used currently for better looks of a smoother sphere */
float fast_sqrt(float x)
 {
   unsigned int i = *(unsigned int*) &x; 
   // adjust bias
   i  += 127 << 23;
   // approximation of square root
   i >>= 1; 
   return *(float*) &i;
 } 

/* A faster function to calculate float powered an int with
binary storage recursion. Time Complexity: O(logn)
Inspired by geeksforgeeks.com's examples, reedited to work with floats. */
float fast_pow(float x, int y){
    float temp;
    if(y==0) return 1; //base case

    //recursion
    temp = fast_pow(x, y/2);
    if(y%2==0) return temp*temp;
    else return x*temp*temp;
}

/* A simple function to make sure every channel for a color stays in [0-1]
could be disabled for better efficiency */
void vec3_cull(vec3 r){
    r[0] = min(1.0f, r[0]);
    r[1] = min(1.0f, r[1]);
    r[2] = min(1.0f, r[2]);
}

/* A simple function that uses a vector to fraction another.
Basically times each row with each other. */
void vec3_fraction(vec3 r, const vec3 a, const vec4 b){
    r[0] = a[0] * b[0];
    r[1] = a[1] * b[1];
    r[2] = a[2] * b[2];
}

/* generate lambertian shade with another light source
 * equation: c = cr(ca + clmax(0, n.l))
 */
void gen_lambert_shade(const vec3 ca, const vec3 cr, const vec3 cl, const vec3 n, vec3 l, vec3 c){
    //normalize light vector
    vec3_norm(l, l);

    //reflectance
    float r = max(0.0f, vec3_mul_inner(n,l));
    vec3_scale(c, cl, r);
    vec3_add(c, c, ca);
    //apply reflect fraction
    vec3_fraction(c, cr, c);
}

/* generate phong shade based on a precalculated lambertian shade
equation: c = clambert + clcp(h.n)^p */
void gen_phong_shade(const vec3 cl, const vec3 cp, const vec3 l, const vec3 e, const vec3 n, const int p, vec3 clambert){
    //calculate h
    vec3 h;
    vec3_add(h, e, l);
    vec3_norm(h,h);

    //calculate shade
    vec3 cphong;
    vec3_scale(cphong, cp, fast_pow(vec3_mul_inner(h,n),p));
    vec3_fraction(cphong, cl, cphong);
    vec3_add(clambert, clambert, cphong);
    vec3_cull(clambert);
}

/* generate anisotropic phong shade by dividing the normal into uv directions
and calculating specular based on the Ward Anisotropic Distribution Model (a BRDF), finally adding the lambertian base shade.
Notice this can't use our fast_pow since the exponential is float.
The formula is referenced from this url:
https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Brushed_Metal */
void gen_WARD_anisotropic_phong_shade(const vec3 cl, const vec3 cp, const vec3 l, const vec3 e, const vec3 n, const float pu, const float pv, const float y, vec3 clambert){
    //calculate h
    vec3 h;
    vec3_add(h, e, l);
    vec3_norm(h,h);

    //calculate u and v unit vector
    vec3 v;
    vec3 y_v = {0.0f, y, 0.0f};
    vec3_scale(v, n, vec3_mul_inner(n,y_v));
    vec3_sub(v, y_v, v);
    vec3_norm(v,v);
    vec3 u;
    vec3_mul_cross(u, v, n);
    vec3_norm(u,u);
    
    //calculate kspecular
    vec3 kspec;
    vec3_scale(kspec, cp, sqrt( max(0.0f, vec3_mul_inner(n,l) / vec3_mul_inner(n,l)*vec3_mul_inner(n,e)) )); //first term of the multiplication
    vec3_scale(kspec, kspec, 1.0f / (4.0f*PI*pu*pv) ); //second term
    vec3_scale(kspec, kspec, exp( -2.0f * (sqr(vec3_mul_inner(h,u)/pu) + sqr(vec3_mul_inner(h,v)/pv)) / (1.0f+vec3_mul_inner(h,n)) ) ); //thrid term
    vec3_fraction(kspec, kspec, cl); //control one more time by light color

    //add specular to lambertian shade
    vec3_add(clambert, clambert, kspec);
    vec3_cull(clambert);
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
            Translation[0]-=1.0f;
            break;
        case GLFW_KEY_RIGHT:
            Translation[0]+=1.0f;
            break;
        case GLFW_KEY_UP   :
            Translation[1]+=1.0f;
            break;
        case GLFW_KEY_DOWN :
            Translation[1]-=1.0f;
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

    float mv_ratio = min(Width_global, Height_global) * 0.8 / 2.0;

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

                vec3 c_total = {0.0f, 0.0f, 0.0f};

                //also make light static (irrespondent to translation) for more interesting effects
                //(This won't make sense for directional light conceptually, thus disabled for it by default)
                //shade for each light
                for(int a=0; a<10; a++){
                    //retrieve directional light for first 5 iteration, spotlight for next 5
                    vec3 l;
                    vec3 cl;
                    if(a<5) get_light(dls, dl_cs, a, l, cl); //raw light info
                    //if point light recal light direction
                    else{
                        get_light(pls, pl_cs, a-5, l, cl);

                        //move light with translation (keep static)
                        vec3 trans = {Translation[0]/mv_ratio, Translation[1]/mv_ratio, Translation[2]/mv_ratio};
                        vec3_add(l, l, trans);

                        //if the light is inside the unit sphere don't draw, doesn't make sense
                        if(vec3_len(l)<1) continue;

                        //cal direction
                        vec3_sub(l, l, norm);
                    }

                    //only render a nonzero light
                    if(vec3_len(l)>0){
                        vec3 c;

                        //generate base lambert shade (note this step will also make the light normalized)
                        gen_lambert_shade(ca, cr, cl, norm, l, c);

                        //generate phong from the previous lambertian shade, with assumed view right down z axis
                        vec3 e = {0.0f, 0.0f, 1.0f};
                        if(using_phong) gen_phong_shade(cl, cp, l, e, norm, p, c);

                        //generate anisotropic shade with pu and pv given (Note: pu and pv has to be culled to [0-1] for this to work)
                        //this can be used to simulate brushed metal or wooden surface, once an appropriate diffuse is given
                        if(using_WARD) gen_WARD_anisotropic_phong_shade(cl, cp, l, e, norm, pu, pv, y, c);

                        //composite
                        vec3_add(c_total, c_total, c);
                    }
                }

                setPixel(i, j, c_total[0], c_total[1], c_total[2]);

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

enum token_code{
    not_specified,
    ambient,
    diffuse,
    specular,
    set_pu,
    set_pv,
    set_p,
    add_dl,
    add_pl
};

token_code get_token_code(string const& token){
    if (token == "-ka") return ambient;
    if (token == "-kd") return diffuse;
    if (token == "-ks") return specular;
    if (token == "-spu") return set_pu;
    if (token == "-spv") return set_pv;
    if (token == "-sp") return set_p;
    if (token == "-dl") return add_dl;
    if (token == "-pl") return add_pl;
    return not_specified;
}

//function to record rgb of a given cmd token
void record_token_rgb(vec3 r, const vector<string> tokens){
    r[0] = stof(tokens[1]);
    r[1] = stof(tokens[2]);
    r[2] = stof(tokens[3]);
}

/* This function changes the direction and color stored in a particular light in a size 5 array. */
void change_light(vec3 dir_list[], vec3 c_list[], int light_n, const vec3 dir, const vec3 c){
    dir_list[light_n][0] = dir[0];
    dir_list[light_n][1] = dir[1];
    dir_list[light_n][2] = dir[2];
    c_list[light_n][0] = c[0];
    c_list[light_n][1] = c[1];
    c_list[light_n][2] = c[2];
}

void record_token_light(vec3 light_l[], vec3 light_color_l[], const vector<string> tokens){
    //find the first available light spot and add our light, if all occupied then do nothing
    for(int i=0; i<5; i++){
        vec3 l;
        vec3 cl;
        get_light(dls, dl_cs, i, l, cl);
        if(vec3_len(l)==0){
            vec3 nl = {stof(tokens[1]), stof(tokens[2]), stof(tokens[3])};
            vec3 ncl = {stof(tokens[4]), stof(tokens[5]), stof(tokens[6])};
            change_light(light_l, light_color_l, i, nl, ncl);
            return;
        }
    }
}

void read_cmd_tokens(const vector<string> tokens){

    switch (get_token_code(tokens[0])) {
        case ambient:
            record_token_rgb(ca, tokens);
            break;

        case diffuse:
            record_token_rgb(cr, tokens);
            break;
        
        case specular:
            record_token_rgb(cp, tokens);
            break;

        case set_pu:
            using_phong = false;
            using_WARD = true;
            pu = stof(tokens[1]);
            break;
        
        case set_pv:
            using_phong = false;
            using_WARD = true;
            pv = stof(tokens[1]);
            break;
        
        case set_p:
            using_phong = true;
            using_WARD = false;
            p = stoi(tokens[1]);
            break;

        case add_dl:
            record_token_light(dls, dl_cs, tokens);
            break;

        case add_pl:
            record_token_light(pls, pl_cs, tokens);
            break;

        default:
            break;
    }

}

int main(int argc, char *argv[]) {

    //take user input
    char buffer[32];
    printf("Enter Options File: ");
    scanf(" %32s", buffer);
    printf("Initialzed using option file [%s]\n", buffer);

    //initialize lights
    vec3 zero = {0.0f, 0.0f, 0.0f};
    for(int i=0; i<5; i++){
        change_light(dls, dl_cs, 0, zero, zero);
        change_light(pls, pl_cs, 0, zero, zero);
    }

    //open option file and record variables
    ifstream input_stream(buffer);
    if(!input_stream){
        printf("Can't open the option file, using default.");
        //add a default directional light
        vec3 l = {1.0f, 1.0f, 0.8f};
        vec3 cl = {0.8f, 0.8f, 0.8f};
        change_light(dls, dl_cs, 0, l, cl);
    }
    
    //read lines for values
    vector<string> text;
    string line;
    while(getline(input_stream, line)) text.push_back(line);
    input_stream.close();
    for(int i=0; i<text.size();i++){
        string cmd = text[i];
        vector<string> split_cmds;
        stringstream ss(cmd);
        string token;
        while(getline(ss, token, ' ')) split_cmds.push_back(token);
        read_cmd_tokens(split_cmds);
    }

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
