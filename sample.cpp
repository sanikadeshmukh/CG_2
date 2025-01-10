#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// title of these windows:

const char* WINDOWTITLE = "OpenGL / GLUT Sample -- Joe Graphics";
const char* GLUITITLE = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT = 4;
const int MIDDLE = 2;
const int RIGHT = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char* ColorNames[] =
{
	(char*)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE = GL_LINEAR;
const GLfloat FOGDENSITY = 0.30f;
const GLfloat FOGSTART = 1.5f;
const GLfloat FOGEND = 4.f;
const int NX = 20;    // Number of grid lines along the X-axis
const int NZ = 20;    // Number of grid lines along the Z-axis
const float GRID_SIZE = 10.0f;  // Size of the grid in each direction
const float GRID_SPACING = 1.0f;  // Spacing between grid lines
// for lighting:

const float	WHITE[] = { 1.,1.,0.,1. };

// for animation:

const int MS_PER_CYCLE = 10000;		// 10000 milliseconds = 10 seconds


// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
GLuint	Dog;				// object display list
GLuint	Cat;
GLuint	Rat;
GLuint CircleDL;
GLuint Ducky;
GLuint Tree1;
GLuint Fence;
GLuint Grass;
GLuint Fountain;
GLuint House;
GLuint Chairs;
bool	Freeze;


#define BOX_WIDTH 5.0f
#define BOX_HEIGHT 1.0f
#define BOX_LENGTH 10.0f

GLuint BoxDL;

GLuint GridDL;

int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowProjection;		// ORTHO or PERSP
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees




// function prototypes:

void	Animate();
void	Display();
void	DoAxesMenu(int);
void	DoColorMenu(int);
void	DoDepthBufferMenu(int);
void	DoDepthFightingMenu(int);
void	DoDepthMenu(int);
void	DoDebugMenu(int);
void	DoMainMenu(int);
void	DoProjectMenu(int);
void	DoRasterString(float, float, float, char*);
void	DoStrokeString(float, float, float, float, char*);
float	ElapsedSeconds();
void	InitGraphics();
void	InitLists();
void	InitMenus();
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset();
void	Resize(int, int);
void	Visibility(int);

void			Axes(float);
void			HsvRgb(float[3], float[3]);
void			Cross(float[3], float[3], float[3]);
float			Dot(float[3], float[3]);
float			Unit(float[3], float[3]);
float			Unit(float[3]);


//Class for Keytime Animation




// utility to create an array from 3 separate values:

float*
Array3(float a, float b, float c)
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float*
MulArray3(float factor, float array0[])
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


float*
MulArray3(float factor, float a, float b, float c)
{
	static float array[4];
	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}

// these are here for when you need them -- just uncomment the ones you need:

#include "setmaterial.cpp"
#include "setlight.cpp"
#include "osusphere.cpp"
//#include "osucone.cpp"
//#include "osutorus.cpp"
//#include "bmptotexture.cpp"
#include "loadobjfile.cpp"
#include "keytime.cpp"
//#include "glslprogram.cpp"


// main program:
const int MSEC = 10000;		// 10000 milliseconds = 10 seconds
Keytimes Xpos, Ypos, Zpos;
Keytimes dogXpos, dogYpos, dogZpos;
Keytimes catXpos, catYpos, catZpos;
Keytimes ratXpos, ratYpos, ratZpos;
Keytimes duckXpos, duckYpos, duckZpos;
Keytimes R, G, B;
Keytimes XRot, YRot, ZRot;
Keytimes dogXRot, dogYRot, dogZRot;
Keytimes catXRot, catYRot, catZRot;
Keytimes ratXRot, ratYRot, ratZRot;
Keytimes duckXRot, duckYRot, duckZRot;

Keytimes Xeye, Yeye, Zeye;
Keytimes LightR, LightG, LightB, LightPosx, LightPosy, LightPosz;




int
main(int argc, char* argv[])
{
	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	glutInit(&argc, argv);

	// setup all the graphics stuff:

	InitGraphics();

	// create the display lists that **will not change**:

	InitLists();

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset();

	// setup all the user interface stuff:

	InitMenus();

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow(MainWindow);
	glutMainLoop();

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate()
{
	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// draw the complete scene:

void
Display()
{


	// turn # msec into the cycle ( 0 - MSEC-1 ):
	int msec = glutGet(GLUT_ELAPSED_TIME) % MSEC;

	// turn that into a time in seconds:
	float nowTime = (float)msec / 250.;
	// set which window we want to do the graphics into:
	glutSetWindow(MainWindow);

	// erase the background:
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
#ifdef DEMO_DEPTH_BUFFER
	if (DepthBufferOn == 0)
		glDisable(GL_DEPTH_TEST);
#endif


	// specify shading to be flat:

	glShadeModel(GL_FLAT);

	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (NowProjection == ORTHO)
		glOrtho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
	else
	{

		gluPerspective(70, 1.f, 0.1f, 1000.f);   //45-12
	}


	// place the objects into the scene:

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	//gluLookAt(15, 5, 15, 0.f, 0.f, 0.f, 0, 1.f, 0.f);
	gluLookAt(Xeye.GetValue(nowTime), Yeye.GetValue(nowTime), Zeye.GetValue(nowTime), 0.f, 0.f, 0.f, 0, 1.f, 0.f);

	//Comment the first Glulookat and uncomment the second one for keytime animation of eye.
	//Make sure you have the values for keytime in initgraphics() function

// rotate the scene

	glRotatef((GLfloat)Yrot, 0.f, 1.f, 0.f);
	glRotatef((GLfloat)Xrot, 1.f, 0.f, 0.f);

	// uniformly scale the scene:

	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

	// set the fog parameters:

	if (DepthCueOn != 0)
	{
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}

	// possibly draw the axes:

	if (AxesOn != 0)
	{
		glColor3fv(&Colors[NowColor][0]);
		glCallList(AxesList);
	}

	// since we are using glScalef( ), be sure the normals get unitized:

	glEnable(GL_NORMALIZE);


	// draw the box object by calling up its display list:


	glEnable(GL_LIGHTING);
	//SetPointLight(GL_LIGHT1, 0, 10, 0, 1, 1, 1);
	SetPointLight(GL_LIGHT1, LightPosx.GetValue(nowTime), LightPosy.GetValue(nowTime), LightPosz.GetValue(nowTime), LightR.GetValue(nowTime), LightG.GetValue(nowTime), LightB.GetValue(nowTime));

	//Comment the first setpointlight and uncomment the second one for keytime animation of light.
	//Make sure you have the values for keytime in initgraphics() function
	//It will be considered as 0 is you dont define it in initgraphics()

	glDisable(GL_LIGHTING);
	glPushMatrix();
	glColor3f(1, 1, 0);
	glTranslatef(LightPosx.GetValue(nowTime), LightPosy.GetValue(nowTime), LightPosz.GetValue(nowTime));
	OsuSphere(1, 50, 50);
	glPopMatrix();
	glEnable(GL_LIGHTING);


	glPushMatrix();

	//Keytime of object translation
	glTranslatef(dogXpos.GetValue(nowTime), 0, 0.);
	glTranslatef(0, dogYpos.GetValue(nowTime), 0.);
	glTranslatef(0, 0, dogZpos.GetValue(nowTime));

	//Keytime of Object Rotation
	glRotatef(dogXRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(dogYRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(dogZRot.GetValue(nowTime), 1, 0, 0);

	//Color keytime animation
	SetMaterial(R.GetValue(nowTime), G.GetValue(nowTime), B.GetValue(nowTime), 20);
	//Change the object 
	glCallList(Dog);
	glPopMatrix();


	glPushMatrix();
	//Object GlCalllis
	

	//Keytime of object translation
	glTranslatef(catXpos.GetValue(nowTime), 0, 0.);
	glTranslatef(0, catYpos.GetValue(nowTime), 0.);
	glTranslatef(0, 0, catZpos.GetValue(nowTime));

	//Keytime of Object Rotation
	glRotatef(catXRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(catYRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(catZRot.GetValue(nowTime), 1, 0, 0);

	//Color keytime animation
	SetMaterial(R.GetValue(nowTime), G.GetValue(nowTime), B.GetValue(nowTime), 20);


	//Change the object 

	glCallList(Cat);
	glPopMatrix();

	glPushMatrix();
	//Object GlCalllis


	//Keytime of object translation
	glTranslatef(ratXpos.GetValue(nowTime), 0, 0.);
	glTranslatef(0, ratYpos.GetValue(nowTime), 0.);
	glTranslatef(0, 0, ratZpos.GetValue(nowTime));

	//Keytime of Object Rotation
	glRotatef(ratXRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(ratYRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(ratZRot.GetValue(nowTime), 1, 0, 0);
	glScalef(0.6, 0.6, 0.6); // Adjust the scale if necessary

	//Color keytime animation
	SetMaterial(R.GetValue(nowTime), G.GetValue(nowTime), B.GetValue(nowTime), 20);


	//Change the object 

	glCallList(Rat);
	glPopMatrix();


	glPushMatrix();
	//Object GlCalllis
	//Keytime of object translation
	glTranslatef(Xpos.GetValue(nowTime), 0, 0.);
	glTranslatef(0, Ypos.GetValue(nowTime), 0.);
	glTranslatef(0, 0, Zpos.GetValue(nowTime));
	//Keytime of Object Rotation
	glRotatef(XRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(YRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(ZRot.GetValue(nowTime), 1, 0, 0);
	//Color keytime animation
	SetMaterial(R.GetValue(nowTime), G.GetValue(nowTime), B.GetValue(nowTime), 20);
	//Change the object 

	glCallList(CircleDL);
	glPopMatrix();


	glPushMatrix();
	//Object GlCalllis
	//Keytime of object translation
	glTranslatef(duckXpos.GetValue(nowTime), 0, 0.);
	glTranslatef(0, duckYpos.GetValue(nowTime), 0.);
	glTranslatef(0, 0, duckZpos.GetValue(nowTime));
	//Keytime of Object Rotation
	glRotatef(duckXRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(duckYRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(duckZRot.GetValue(nowTime), 1, 0, 0);
	//Color keytime animation
	SetMaterial(R.GetValue(nowTime), G.GetValue(nowTime), B.GetValue(nowTime), 20);
	//Change the object 

	glCallList(Ducky);
	glPopMatrix();

	glPushMatrix();
	//Object GlCalllis
	//Keytime of object translation
	glTranslatef(Xpos.GetValue(nowTime), 0, 0.);
	glTranslatef(0, Ypos.GetValue(nowTime), 0.);
	glTranslatef(0, 0, Zpos.GetValue(nowTime));
	//Keytime of Object Rotation
	glRotatef(XRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(YRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(ZRot.GetValue(nowTime), 1, 0, 0);
	//Color keytime animation
	SetMaterial(R.GetValue(nowTime), G.GetValue(nowTime), B.GetValue(nowTime), 20);
	//Change the object 

	glCallList(BoxDL);
	glPopMatrix();


	glPushMatrix();
	//Object GlCalllis
	//Keytime of object translation
	glTranslatef(Xpos.GetValue(nowTime), 0, 0.);
	glTranslatef(0, Ypos.GetValue(nowTime), 0.);
	glTranslatef(0, 0, Zpos.GetValue(nowTime));
	//Keytime of Object Rotation
	glRotatef(XRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(YRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(ZRot.GetValue(nowTime), 1, 0, 0);
	//Color keytime animation
	SetMaterial(R.GetValue(nowTime), G.GetValue(nowTime), B.GetValue(nowTime), 20);
	//Change the object 

	glCallList(Tree1);
	glPopMatrix();

	glPushMatrix();  // Save the current matrix state
	//glRotatef(90, 1, 0, 0);
	glTranslatef(0.0f, 0.0f, 30.0f);  // Position the tree
	//glScalef(10.0f, 0.0f, 0.25f);  // Scale the tree
	glCallList(Tree1);  // Render the tree
	glPopMatrix();  // Restore the matrix state

	glPopMatrix();
	

	glPushMatrix();
	glTranslatef(30, 0, 0.0f);  // Position left of the square
	glRotatef(90, 0, 1, 0);
	//glScalef(0.25f, 0.25f, 0.25f);  // Scale the tree
	glCallList(Tree1);  // Render the tree
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-5, 0, 0.0f);  // Position left of the square
	glRotatef(90, 0, 1, 0);
	//glScalef(0.25f, 0.25f, 0.25f);  // Scale the tree
	glCallList(Tree1);  // Render the tree
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.0f, 0.0f, 33.0f);
	glScalef(1.09f, 1.0f, 1.0f);
	glCallList(Fence);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(35, 0, 0.0f);
	glRotatef(90, 0, 1, 0);
	glCallList(Fence);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glTranslatef(0, 0, -10.0f);
	glCallList(Fence);
	glPopMatrix();

	/*glPushMatrix();
	glCallList(Fence);
	glPopMatrix();*/


	glPushMatrix();
	glCallList(Grass);
	glPopMatrix();


	glPushMatrix();
	//Object GlCalllis
	//Keytime of object translation
	glTranslatef(Xpos.GetValue(nowTime), 0, 0.);
	glTranslatef(0, Ypos.GetValue(nowTime), 0.);
	glTranslatef(0, 0, Zpos.GetValue(nowTime));
	//Keytime of Object Rotation
	glRotatef(XRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(YRot.GetValue(nowTime), 1, 0, 0);
	glRotatef(ZRot.GetValue(nowTime), 1, 0, 0);
	//Color keytime animation
	SetMaterial(R.GetValue(nowTime), G.GetValue(nowTime), B.GetValue(nowTime), 20);
	//Change the object 

	glCallList(Fountain);
	glPopMatrix();

	glPushMatrix();
	SetMaterial(0.85f, 0.75f, 0.60f, 20);
	//SetMaterial(0.87f, 0.72f, 0.53f, 15);
	glTranslatef(-40.0f, 2.0f, -25.0f); // Shift further left by changing from -30 to -50
	glScalef(8.0, 5.0, 2.7); // Scale for house dimensions
	glCallList(House);
	glPopMatrix();

	glCallList(Chairs);
	glPopMatrix();



	//glCallList(CircleDL);
	//glCallList(Ducky);
	//glCallList(BoxDL);
	//glCallList(Tree1);
	//glCallList(Fountain);
	//glPopMatrix();

#ifdef DEMO_Z_FIGHTING
	if (DepthFightingOn != 0)
	{
		glPushMatrix();
		glRotatef(90.f, 0.f, 1.f, 0.f);
		glCallList(BoxList);
		glPopMatrix();
	}
#endif


	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glColor3f(1.f, 1.f, 1.f);
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Eye is Moving" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.f, 100, 0.f, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.f, 1.f, 1.f);


	// swap the double-buffered framebuffers:

	glutSwapBuffers();

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush();
}


void
DoAxesMenu(int id)
{
	AxesOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoColorMenu(int id)
{
	NowColor = id - RED;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDebugMenu(int id)
{
	DebugOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthBufferMenu(int id)
{
	DepthBufferOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthFightingMenu(int id)
{
	DepthFightingOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthMenu(int id)
{
	DepthCueOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// main menu callback:

void
DoMainMenu(int id)
{
	switch (id)
	{
	case RESET:
		Reset();
		break;

	case QUIT:
		// gracefully close out the graphics:
		// gracefully close the graphics window:
		// gracefully exit the program:
		glutSetWindow(MainWindow);
		glFinish();
		glutDestroyWindow(MainWindow);
		exit(0);
		break;

	default:
		fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoProjectMenu(int id)
{
	NowProjection = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// use glut to display a string of characters using a raster font:

void
DoRasterString(float x, float y, float z, char* s)
{
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString(float x, float y, float z, float ht, char* s)
{
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	float sf = ht / (119.05f + 33.33f);
	glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds()
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet(GLUT_ELAPSED_TIME);

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow(MainWindow);

	int numColors = sizeof(Colors) / (3 * sizeof(float));
	int colormenu = glutCreateMenu(DoColorMenu);
	for (int i = 0; i < numColors; i++)
	{
		glutAddMenuEntry(ColorNames[i], i);
	}

	int axesmenu = glutCreateMenu(DoAxesMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthcuemenu = glutCreateMenu(DoDepthMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int debugmenu = glutCreateMenu(DoDebugMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int projmenu = glutCreateMenu(DoProjectMenu);
	glutAddMenuEntry("Orthographic", ORTHO);
	glutAddMenuEntry("Perspective", PERSP);

	int mainmenu = glutCreateMenu(DoMainMenu);
	glutAddSubMenu("Axes", axesmenu);
	glutAddSubMenu("Axis Colors", colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu("Depth Buffer", depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu("Depth Fighting", depthfightingmenu);
#endif

	glutAddSubMenu("Depth Cue", depthcuemenu);
	glutAddSubMenu("Projection", projmenu);
	glutAddMenuEntry("Reset", RESET);
	glutAddSubMenu("Debug", debugmenu);
	glutAddMenuEntry("Quit", QUIT);

	// attach the pop-up menu to the right mouse button:

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}





// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

	// open the window and set its title:

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);

	// set the framebuffer clear values:

	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);

// Initialize motion variables for all objects
//latest code for finlas


dogXpos.Init();
dogZpos.Init();
dogYpos.Init();

ratXpos.Init();
ratYpos.Init();
ratZpos.Init();

catXpos.Init();
catYpos.Init();
catZpos.Init();

float fountainX = -5.0f; // Fountain's X position
float fountainZ = 0.0f;  // Fountain's Z position
float radius = 2.0f;     // Radius of the circular motion
float speed = 1.0f;      // Speed of rotation

for (float t = 0.0f; t <= 20.0f; t += 2.0f) {
	float angle = speed * t; // Angle for circular motion

	// Dog's motion (leading the circle)
	dogXpos.AddTimeValue(t, fountainX + radius * cos(angle));
	dogZpos.AddTimeValue(t, fountainZ + radius * sin(angle));
	dogYpos.AddTimeValue(t, 0.0f); // Keep Y constant

	// Rat's motion (following the dog, delayed slightly)
	ratXpos.AddTimeValue(t, fountainX + radius * cos(angle - 0.5f)); // Offset by -0.5 radians
	ratZpos.AddTimeValue(t, fountainZ + radius * sin(angle - 0.5f));
	ratYpos.AddTimeValue(t, 0.0f); // Keep Y constant

	// Cat's motion (following the rat, delayed further)
	catXpos.AddTimeValue(t, fountainX + radius * cos(angle - 4.0f)); // Offset by -1.0 radians
	catZpos.AddTimeValue(t, fountainZ + radius * sin(angle - 4.0f));
	catYpos.AddTimeValue(t, 0.0f); // Keep Y constant
}

	duckXpos.Init();
	for (float t = 0.0; t <= 20.0; t += 0.1) {
		float angle = t / 20.0 * 2 * M_PI; // Adjust time parameter for a full circle in 20 seconds
		float xPos = 3.0f * cos(angle);
		duckXpos.AddTimeValue(t, xPos);
	}

	duckYpos.Init();
	for (float t = 0.0; t <= 20.0; t += 0.1) {
		duckYpos.AddTimeValue(t, 0.0);  // Duck stays at Y=0 throughout the motion
	}

	duckZpos.Init();
	for (float t = 0.0; t <= 20.0; t += 0.1) {
		float angle = t / 20.0 * 2 * M_PI; // Adjust time parameter for a full circle in 20 seconds
		float zPos = 3.0f * sin(angle);
		duckZpos.AddTimeValue(t, zPos);
	}


	YRot.Init();
	ZRot.Init();
	LightR.Init();
	LightR.AddTimeValue(0.0, 1.0);
	LightR.AddTimeValue(10.0, 1.0);


	LightB.Init();
	LightB.AddTimeValue(10.0, 1);
	LightB.AddTimeValue(0.0, 1);
	


	LightG.Init();
	LightG.AddTimeValue(0.0, 1.0);
	LightG.AddTimeValue(10.0, 1.0);

	LightPosx.Init();
	LightPosx.AddTimeValue(0.0, -25.0);

	for (float t = 0.1; t <= 00.0; t += 0.1) {
		float angle = t / 40.0 * 2 * M_PI; // Adjust time parameter for a full rotation in 40 seconds
		float xPos = 25.0f * cos(angle);
		LightPosx.AddTimeValue(t, xPos);
	}

	LightPosy.Init();
	LightPosy.AddTimeValue(0.0, 0.0);

	for (float t = 0.1; t <= 40.0; t += 0.1) {
		float angle = t / 40.0 * 2 * M_PI;
		float yPos = 50.0f * sin(angle);
		LightPosy.AddTimeValue(t, yPos);
	}


	LightPosz.Init();
	LightPosz.AddTimeValue(0, 0);
	LightPosz.AddTimeValue(10, 0);

	Xeye.Init();
	Yeye.Init();
	Zeye.Init();

	float startX = 15.0f;   // Starting X position from gluLookAt
	float endX = -15.0f;    // Ending X position for steady motion
	float backX = -18.0f;   // Slightly back position
	float centerY = 5.0f;   // Y position from gluLookAt
	float centerZ = 15.0f;  // Z position from gluLookAt
	float steadyTime = 50.0f; // Time for steady motion
	float backTime = 60.0f;   // Total time including backup

	for (float t = 0.0f; t <= backTime; t += 2.0f) {
		float x;
		if (t <= steadyTime) {
			// Linear interpolation for steady motion along X-axis
			x = startX + (endX - startX) * (t / steadyTime);
		}
		else {
			// Backup phase
			x = endX + (backX - endX) * ((t - steadyTime) / (backTime - steadyTime));
		}

		// Add values to the interpolators
		Xeye.AddTimeValue(t, x);
		Yeye.AddTimeValue(t, centerY); // Keep Y constant from gluLookAt
		Zeye.AddTimeValue(t, centerZ); // Keep Z constant from gluLookAt
	}


	
	

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc(Animate);

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "glewInit Error\n");
	}
	else
		fprintf(stderr, "GLEW initialized OK\n");
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// all other setups go here, such as GLSLProgram and KeyTime setups:

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists()
{

#define XSIDE	30			// length of the x side of the grid
#define X0      (-XSIDE/2.)		// where one side starts
#define NX	250			// how many points in x
#define DX	( XSIDE/(float)NX )	// change in x between the points
#define CIRCLE_RADIUS	14
#define YGRID	0.f
#define DEG2RAD (3.14159265358979323846 / 180.0)
#define ZSIDE	30			// length of the z side of the grid
#define Z0      (-ZSIDE/2.)		// where one side starts
#define NZ	250			// how many points in z
#define DZ	( ZSIDE/(float)NZ )	// change in z between the points
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");

	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow(MainWindow);

	float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // Time in seconds for animation

	Dog = glGenLists(1);
	glNewList(Dog, GL_COMPILE);
	SetMaterial(0.6, 0.3, 0.0, 50);
	//glTranslatef(-5.0f, 0.0f, 1.5f); // Position near the fountain
	glTranslatef(dogXpos.GetValue(t), dogYpos.GetValue(t), dogZpos.GetValue(t)); // Use interpolated values
	glScalef(1, 1, 1);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f); // Rotate to face the center
	LoadObjFile((char*)"dog.obj");
	glEndList();

	// Cat Setup (Facing towards the center of the group)
	Cat = glGenLists(1);
	glNewList(Cat, GL_COMPILE);
	SetMaterial(1.0, 0.647, 0.0, 50);
	glTranslatef(-5.5f, 0.0f, 0.0f); // Offset closer to the dog and fountain
	//glTranslatef(catXpos.GetValue(t), catYpos.GetValue(t), catZpos.GetValue(t)); // Use interpolated values
	glScalef(0.6, 0.6, 0.6);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // Rotate to face the center
	LoadObjFile((char*)"catH.obj");
	glEndList();

	// Rat Setup (Facing towards the center of the group)
	Rat = glGenLists(1);
	glNewList(Rat, GL_COMPILE);
	SetMaterial(0.5, 0.5, 0.5, 50);
	glTranslatef(-4.5f, 0.0f, 2.0f); // Close to the dog and cat, near fountain
	//glTranslatef(ratXpos.GetValue(t), ratYpos.GetValue(t), ratZpos.GetValue(t)); // Use interpolated values
	glScalef(0.5, 0.5, 0.5);
	glRotatef(270.0f, 1.0f, 0.0f, 0.0f); // Adjust orientation
	LoadObjFile((char*)"19349_Mouse_v1.obj");
	glEndList();

	Ducky = glGenLists(1);
	glNewList(Ducky, GL_COMPILE);
	//glColor3f(1.0f, 1.0f, 0.6f);
	SetMaterial(1.0, 1.0, 0.0, 50);
	glTranslatef(10.1f, 0.0f, 10.1f);
	glScalef(0.5, 0.5, 0.5);
	glRotatef(0, 0, 0, 0);
	LoadObjFile((char*)"ducky.obj");
	glEndList();

	//Tree

	Tree1 = glGenLists(1);
	glNewList(Tree1, GL_COMPILE);
	SetMaterial(0.0, 0.8, 0.0, 50);  // RGB values for green
	glTranslatef(1.0f, 0.2f, -13.0f);
	glScalef(0.25, 0.25, 0.25);
	glRotatef(0.0f, 0, 1, 0);  // Rotate 90 degrees to the right
	LoadObjFile((char*)"trees9.obj");
	//for (int i = 0; i < numTrees; i++) {
	//	for (int j = 0; j < numTrees; j++) {
	//		glPushMatrix();

	//		// Translate the tree position
	//		// Trees are placed at positions (i, j) in a square grid
	//		glTranslatef(i * spacing, 0.2f, j * spacing);  // Adjust the position

	//		// Scale the trees and render them
	//		glScalef(0.25f, 0.25f, 0.25f);  // Scale down the tree model

	//		// Render the tree
	//		glCallList(Tree1);

	//		glPopMatrix();
	//	}
	//}
	glEndList();

	Fence = glGenLists(1);
	glNewList(Fence, GL_COMPILE);

	// Set material for the wood fence (brown color with some shine)
	SetMaterial(0.545, 0.271, 0.075, 50); // RGB values for brown, shininess of 50

	// Move the fence to the desired position in the scene
	glTranslatef(1.0f, 0.2f, -13.0f); // Position at the center of the scene

	// Scale the fence object to adjust its size
	glScalef(0.2, 0.05, 0.03);  // Increase the X-axis scaling factor to make the fence wider

	// Rotate the fence to align it correctly in the scene
	glRotatef(270.0f, 1, 0, 0);  // Rotate it 90 degrees around the X-axis to make it stand upright

	// Load the fence object
	LoadObjFile((char*)"13076_Gothic_Wood_Fence_Panel_v2_l3.obj");

	glEndList();


	Grass = glGenLists(1);
	glNewList(Grass, GL_COMPILE);

	////// Set material color for the grass (green)
	SetMaterial(0.0f, 0.95f, 0.0f, 10.0f);

	////// Translate to the desired position
	glTranslatef(1.0f, -1.8f, -13.0f);  // Moves the object slightly downwards along the Y-axis


	////// Scale down the grass object (reduce the size)
	glScalef(0.25, 0.25, 0.25);  // Adjust these scale values as needed

	////// Rotate to make it flat on the X-axis
	glRotatef(275.0f, 1, 0, 0);  // Rotate 90 degrees around the X-axis to make it lay flat

	////// Load the grass model (e.g., "Grass2.obj")
	LoadObjFile((char*)"Grass2.obj");

	glEndList();



	//Fountain





	// Circle

	CircleDL = glGenLists(1);
	glNewList(CircleDL, GL_COMPILE);
	SetMaterial(0.0, 1.0, 1.0, 50);
	// Draw a solid quarter circle centered at (0, 0, 0) and 90 degrees to the right
	glTranslatef(0.0f, 0.0, 0.0f);
	glBegin(GL_TRIANGLE_FAN);
	float centerX = 15.0f;  // New X coordinate
	float centerY = 0.05f;    // New Y coordinate
	float centerZ = 15.0f;   // New Z coordinate
	float initialAngle = 180.0f;  // New initial angle

	glVertex3f(centerX, centerY, centerZ); // Center of the circle
	for (int i = 0; i <= 90; i += 10) {
		float angle = (initialAngle + static_cast<float>(i)) * DEG2RAD;
		float x = centerX + cos(angle) * CIRCLE_RADIUS;
		float z = centerZ + CIRCLE_RADIUS * sin(angle);
		glVertex3f(x, centerY, z);
	}
	glEnd();
	glEndList();


	Fountain = glGenLists(1);
	glNewList(Fountain, GL_COMPILE);

	// Set material for the fountain (light blue color)
	SetMaterial(0.929, 0.908, 0.922, 100);

	// Move the fountain to the center of the scene (on the Y-axis)
	glTranslatef(0.0f, 0.0f, 0.0f);  // Position at the center of the scene

	// Scale the fountain object (adjust size as needed)
	glScalef(0.03, 0.05, 0.03);  // Adjust size based on your needs

	// Rotate the fountain to stand upright on the Y-axis
	// This rotates the fountain 90 degrees around the X-axis
	glRotatef(270.0f, 1, 0, 0);  // Rotate it 90 degrees around the X-axis to make it stand upright

	// Load the fountain object (replace with your actual object file)
	LoadObjFile((char*)"fountain.obj");

	glEndList();

	House = glGenLists(1);
	glNewList(House, GL_COMPILE);
	SetMaterial(0.96f, 0.96f, 0.86f, 10.0f);
	glTranslatef(2, 0, 0);
	glScalef(0.5f, .5f, .5f);
	LoadObjFile((char*)"Bambo_House.obj");
	glEndList();



	Chairs = glGenLists(1);
	glNewList(Chairs, GL_COMPILE);
	SetMaterial(0.8f, 0.5f, 0.3f, 20.0f);

	glTranslatef(10, 0, -5);
	glScalef(0.005f, 0.005f, 0.005f);
	LoadObjFile((char*)"3d-model.obj");
	glEndList();

	// create the axes:

	AxesList = glGenLists(1);
	glNewList(AxesList, GL_COMPILE);
	glLineWidth(AXES_WIDTH);
	Axes(1.5);
	glLineWidth(1.);
	glEndList();
}


// the keyboard callback:

void
Keyboard(unsigned char c, int x, int y)
{
	if (DebugOn != 0)
		fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

	switch (c)
	{
	case 'o':
	case 'O':
		NowProjection = ORTHO;
		break;

	case 'p':
	case 'P':
		NowProjection = PERSP;
		break;

	case 'f':
	case 'F':
		Freeze = !Freeze;
		if (Freeze)
			glutIdleFunc(NULL);
		else
			glutIdleFunc(Animate);
		break;


	case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu(QUIT);	// will not return here
		break;				// happy compiler
	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// called when the mouse button transitions down or up:

void
MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if (DebugOn != 0)
		fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);


	// get the proper button bit mask:

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;

	case SCROLL_WHEEL_UP:
		Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	case SCROLL_WHEEL_DOWN:
		Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}

	// button down sets the bit, up clears the bit:

	if (state == GLUT_DOWN)
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion(int x, int y)
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT * dy);
		Yrot += (ANGFACT * dx);
	}

	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset()
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Freeze = false;
	Scale = 1.0;
	ShadowsOn = 0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize(int width, int height)
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// handle a change to the window's visibility:

void
Visibility(int state)
{
	if (DebugOn != 0)
		fprintf(stderr, "Visibility: %d\n", state);

	if (state == GLUT_VISIBLE)
	{
		glutSetWindow(MainWindow);
		glutPostRedisplay();
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = { 0.f, 1.f, 0.f, 1.f };

static float xy[] = { -.5f, .5f, .5f, -.5f };

static int xorder[] = { 1, 2, -3, 4 };

static float yx[] = { 0.f, 0.f, -.5f, .5f };

static float yy[] = { 0.f, .6f, 1.f, 1.f };

static int yorder[] = { 1, 2, 3, -2, 4 };

static float zx[] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes(float length)
{
	glBegin(GL_LINE_STRIP);
	glVertex3f(length, 0., 0.);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., length, 0.);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., length);
	glEnd();

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 4; i++)
	{
		int j = xorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 5; i++)
	{
		int j = yorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 6; i++)
	{
		int j = zorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
	}
	glEnd();

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb(float hsv[3], float rgb[3])
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while (h >= 6.)	h -= 6.;
	while (h < 0.) 	h += 6.;

	float s = hsv[1];
	if (s < 0.)
		s = 0.;
	if (s > 1.)
		s = 1.;

	float v = hsv[2];
	if (v < 0.)
		v = 0.;
	if (v > 1.)
		v = 1.;

	// if sat==0, then is a gray:

	if (s == 0.0)
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:

	float i = (float)floor(h);
	float f = h - i;
	float p = v * (1.f - s);
	float q = v * (1.f - s * f);
	float t = v * (1.f - (s * (1.f - f)));

	float r = 0., g = 0., b = 0.;			// red, green, blue
	switch ((int)i)
	{
	case 0:
		r = v;	g = t;	b = p;
		break;

	case 1:
		r = q;	g = v;	b = p;
		break;

	case 2:
		r = p;	g = v;	b = t;
		break;

	case 3:
		r = p;	g = q;	b = v;
		break;

	case 4:
		r = t;	g = p;	b = v;
		break;

	case 5:
		r = v;	g = p;	b = q;
		break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit(float v[3])
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}

