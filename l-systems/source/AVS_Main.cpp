/*******************************************************************

	Welcome to my Advanced Visual Simulation L-System!
	--------------------------------------------------

  The formal grammar used is given below:

  F = Move forward a step of length d and draw a line between the 
	  new and the old positions.

  + = Turn right by angle theta.

  - = Turn left by angle theta,

*******************************************************************/

#include <GL/glut.h>		// Glut 		
#include <GL/gl.h>			// OpenGl 
#include <iostream>			// Input, output 
#include <fstream>			// File management 
#include <windows.h>		// Windows. Some windows functions are used
#include <time.h>			// Used for timing
#include <math.h>			// Used for cos, sin etc

////////////////////////////////////////////////////////////////////

#include "../header/ProductionRules.h"
#include "../header/OpenGL.h"
#include "../header/Camera.h"
#include "../header/Vertex.h"
#include "../header/Fractal.h"
#include "../header/Quaternion.h"

////////////////////////////////////////////////////////////////////
// STANDARD NAMESPACE
////////////////////////////////////////////////////////////////////

using namespace std;		// Standard namespace

////////////////////////////////////////////////////////////////////

#define NUM_OF_ITERATIONS	1000
#define XPIXELS				768		// Number of pixels in X direction
#define YPIXELS				768
#define DRAWING_ANGLE		20
#define DRAWING_DISTANCE	1

////////////////////////////////////////////////////////////////////

void read_input_file (void);
void initialise (void);
void clean_up (void);
void print_rules (void);
void display (void);
void idle (void);
void draw_axis (void);
void mouse (int, int, int, int);	// Mouse callback function
void motion (int, int);				// Motion callback function
void keyboard (unsigned char, int, int);	// Keyboard callback function
void reshape(int, int);
void draw_koch_rules (void);
void draw_other_rules (void);

////////////////////////////////////////////////////////////////////

/******* Integers *******/
int window_main;			// Window ID
int old_mouse_x, old_mouse_y;
int	font=(int)GLUT_BITMAP_HELVETICA_18;

/******* Floats *******/
float x_rot, y_rot;

/******* Chars *******/
char text_buffer[512];

CProductionRules *starting_rules;
COpenGL *open_gl;
CCamera *camera;
CFractal *fractal;
CQuaternion *rotation_quaternion;

////////////////////////////////////////////////////////////////////
// MAIN FUNCTION
////////////////////////////////////////////////////////////////////

int main(int argc,char **argv)
{
	// Below is the code to initialise GLUT. As GLUT takes care of all
	// window management, there is no reason to use windows API for window
	// control. This means that a console dialogue box remains when running
	// the game. However, due to the visual progress made in the dialogue 
	// box when loading the game as opposed to seeing nothing on screen
	// until the game starts, this has been left in. 
	
	// To remove dialogue box follow these instructions:
	// -------------------------------------------------

	// Go to project->settings.
	// Select the link tab.
	// In the Project options edit box, add /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup 

	glutInit(&argc,argv);							// Definition of main window 
	glutInitWindowSize(XPIXELS, YPIXELS);			// Size specifications 
	glutInitWindowPosition(0,0);					// Position of window, relative to the top left corner 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);	// Set the mode for the main window 
	
	/********************************************/

	window_main = glutCreateWindow("Stephen Chamberlain :: Advanced Visual Simulation :: L-Systems");	// Create the window and name it 
  	glutDisplayFunc(display);			// Display the window 
	glutKeyboardFunc(keyboard);			// Register the keyboard strokes
//	glutSpecialFunc(special);			// Special keyboard activity e.g. arrow keys etc
	glutMouseFunc(mouse);				// Register mouse activity
	glutMotionFunc(motion);				// Register mouse motion
//	glutPassiveMotionFunc(passive_motion);	// Register passive mouse motion
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	initialise();

	glutMainLoop();				// Main GLUT loop command

	return 0;
}

////////////////////////////////////////////////////////////////////
//  PRINT_RULES FUNCTION
////////////////////////////////////////////////////////////////////

void display (void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);	// Clears the main window to a pleasent blue shade 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear the colour buffer bit

	open_gl->gl_fp_viewing_system(-camera->x, -camera->y, -camera->z, x_rot, y_rot);
	open_gl->set_light_and_properties(0, 100, 0, 0.6, 0, 2.76); 

//	glutSolidCube(1);

	draw_axis();
//	draw_koch_rules();
	draw_other_rules();

	sprintf_s(text_buffer,"Number of Iterations");
	open_gl->text_renderer(10, 25,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf_s(text_buffer,"%d",NUM_OF_ITERATIONS);
	open_gl->text_renderer(10, 11,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	glutSwapBuffers();
}

////////////////////////////////////////////////////////////////////
//  draw_koch_rules FUNCTION
////////////////////////////////////////////////////////////////////

void draw_other_rules (void)
{
	int i;
	static CVertex *turtle_state;
	static CVertex *memorise_turtle_state;
	float matrix[16];

	turtle_state = new CVertex;
	memorise_turtle_state = new CVertex;

	turtle_state->set_x(0);
	turtle_state->set_y(0);
	turtle_state->set_z(0);

	memorise_turtle_state->set_x(0);
	memorise_turtle_state->set_y(0);
	memorise_turtle_state->set_z(0);

	for (i=0; i<8192; i++)
	{
		if (starting_rules->get_drawing_rule(i) == 'F')
		{
			open_gl->draw_line(turtle_state->get_x(),
							   turtle_state->get_y(),
							   turtle_state->get_z(),

							   turtle_state->get_x(),
							   turtle_state->get_y() + DRAWING_DISTANCE,
							   turtle_state->get_z());

			turtle_state->set_x(turtle_state->get_x());
			turtle_state->set_y(turtle_state->get_y() + DRAWING_DISTANCE);
			turtle_state->set_z(turtle_state->get_z());

			glColor3f(0.0, 1.0, 0.0);
		}

		if (starting_rules->get_drawing_rule(i) == '+')
		{
			glTranslatef(turtle_state->get_x(), turtle_state->get_y(), turtle_state->get_z());
			rotation_quaternion->create_from_axis_angle(0.0, 0.0, 1.0, DRAWING_ANGLE);
			rotation_quaternion->create_matrix(matrix);
			glMultMatrixf(matrix);
			glTranslatef(-turtle_state->get_x(), -turtle_state->get_y(), -turtle_state->get_z());

			glColor3f(0.0, 1.0, 0.0);
		}

		if (starting_rules->get_drawing_rule(i) == '-')
		{
			glTranslatef(turtle_state->get_x(), turtle_state->get_y(), turtle_state->get_z());
			rotation_quaternion->create_from_axis_angle(0.0, 0.0, 1.0, -DRAWING_ANGLE);
			rotation_quaternion->create_matrix(matrix);
			glMultMatrixf(matrix);
			glTranslatef(-turtle_state->get_x(), -turtle_state->get_y(), -turtle_state->get_z());

			glColor3f(1.0, 0.0, 0.0);
		}

		if (starting_rules->get_drawing_rule(i) == '&')
		{
			glTranslatef(turtle_state->get_x(), turtle_state->get_y(), turtle_state->get_z());
			rotation_quaternion->create_from_axis_angle(1.0, 0.0, 0.0, 180);
			rotation_quaternion->create_matrix(matrix);
			glMultMatrixf(matrix);
			glTranslatef(-turtle_state->get_x(), -turtle_state->get_y(), -turtle_state->get_z());

			glColor3f(0.0, 0.0, 1.0);
		}

		if (starting_rules->get_drawing_rule(i) == '[')
		{
			memorise_turtle_state->set_x(turtle_state->get_x());
			memorise_turtle_state->set_y(turtle_state->get_y());
			memorise_turtle_state->set_z(turtle_state->get_z());
	
			glColor3f(1.0, 1.0, 1.0);
		}

		if (starting_rules->get_drawing_rule(i) == ']')
		{
			turtle_state->set_x(memorise_turtle_state->get_x());
			turtle_state->set_y(memorise_turtle_state->get_y());
			turtle_state->set_z(memorise_turtle_state->get_z());
	
			glColor3f(0.0, 0.0, 0.0);
		}
	}
}

////////////////////////////////////////////////////////////////////
//  draw_koch_rules FUNCTION
////////////////////////////////////////////////////////////////////

void draw_koch_rules (void)
{
	int i;
	static CVertex *turtle_state;

	turtle_state = new CVertex;

	turtle_state->set_x(0);
	turtle_state->set_y(0);
	turtle_state->set_z(0);

	glColor3f(0.0, 1.0, 0.0);

	for (i=0; i<starting_rules->position_in_drawing_array; i++)
	{
		if (starting_rules->get_drawing_rule(i) == 'F')
		{
			open_gl->draw_line(turtle_state->get_x(),
							   turtle_state->get_y(),
							   turtle_state->get_z(),
							   turtle_state->get_x(),
							   turtle_state->get_y() + DRAWING_DISTANCE,
							   turtle_state->get_z());

			turtle_state->set_x(turtle_state->get_x());
			turtle_state->set_y(turtle_state->get_y() + DRAWING_DISTANCE);
			turtle_state->set_z(turtle_state->get_z());
		}

		if (starting_rules->get_drawing_rule(i) == '+')
		{
			glTranslatef(turtle_state->get_x(), turtle_state->get_y(), turtle_state->get_z());
			glRotatef(DRAWING_ANGLE, 0.0, 0.0, 1.0 );
			glTranslatef(-turtle_state->get_x(), -turtle_state->get_y(), -turtle_state->get_z());

			glColor3f(0.0, 1.0, 0.0);
		}

		if (starting_rules->get_drawing_rule(i) == '-')
		{
			glTranslatef(turtle_state->get_x(), turtle_state->get_y(), turtle_state->get_z());
			glRotatef(-DRAWING_ANGLE, 0.0, 0.0, 1.0 );
			glTranslatef(-turtle_state->get_x(), -turtle_state->get_y(), -turtle_state->get_z());

			glColor3f(1.0, 0.0, 0.0);
		}

		if (starting_rules->get_drawing_rule(i) == '&')
		{
			glTranslatef(turtle_state->get_x(), turtle_state->get_y(), turtle_state->get_z());
			glRotatef(180, 0.0, 0.0, 1.0 );
			glTranslatef(-turtle_state->get_x(), -turtle_state->get_y(), -turtle_state->get_z());

			glColor3f(0.0, 0.0, 1.0);
		}
	}
}

////////////////////////////////////////////////////////////////////
//  PRINT_RULES FUNCTION
////////////////////////////////////////////////////////////////////

void draw_axis (void)
{
	int i;

/*	glColor3f(1.0, 0.0, 0.0);
	open_gl->draw_line(10,0,0,-10,0,0);
	glColor3f(0.0, 1.0, 0.0);
	open_gl->draw_line(0,10,0,0,-10,0);
	glColor3f(0.0, 0.0, 1.0);
	open_gl->draw_line(0,0,10,0,0,-10);
*/
	for (i=0; i<100; i+=10)
	{
		glColor3f(0.0, 0.0, 0.0);
		open_gl->draw_line(i-45,0,45,
						   i-45,0,-45);

		glColor3f(0.0, 0.0, 0.0);
		open_gl->draw_line(45,0,i-45,
						   -45,0,i-45);
	}
}

////////////////////////////////////////////////////////////////////
//  PRINT_RULES FUNCTION
////////////////////////////////////////////////////////////////////

void idle (void)
{
//	y_rot = y_rot + 0.05;

//	if (y_rot > 360)
//	{
//		y_rot = 0;
//	}

	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////
//  PRINT_RULES FUNCTION
////////////////////////////////////////////////////////////////////

void print_rules (void)
{
	int i;

	cout << "Rules" << endl << "_____" << endl << endl;

	for (i=0; i<starting_rules->get_number_of_rules(); i++)
	{
		cout << starting_rules->get_rule(i) << " ";
	}

	cout << endl << endl;

	cout << "Drawing Rules" << endl << "_______________" << endl << endl;

	for (i=0; i<starting_rules->position_in_drawing_array; i++)
	{
		cout << starting_rules->get_drawing_rule(i) << " ";
	}

	cout << endl << endl;
}

////////////////////////////////////////////////////////////////////
// READ_INPUT FILE FUNCTION
////////////////////////////////////////////////////////////////////

void read_input_file (void)
{
	int number_of_rules_to_read, i;
	char temp;

	ifstream input;

	input.open("input/Koch Curve.ini");
//	input.open("input/Plant.ini");
//	input.open("input/Other.ini");

	input >> number_of_rules_to_read;

	for (i=0; i<number_of_rules_to_read; i++)
	{
		input >> temp;
		starting_rules->add_rule(i, temp);
	}

	starting_rules->set_number_of_rules(number_of_rules_to_read);

	input.close();
}

////////////////////////////////////////////////////////////////////
//  INITIALISE FUNCTION
////////////////////////////////////////////////////////////////////

void initialise (void)
{
	starting_rules = new CProductionRules;
	open_gl = new COpenGL;
	camera = new CCamera;
	fractal = new CFractal;
	rotation_quaternion = new CQuaternion;

	read_input_file();

	starting_rules->create_drawing_rules(NUM_OF_ITERATIONS);

	print_rules();

	glutSetWindow(window_main);
	open_gl->set_perspective();
	glColorMaterial(1, 1);	

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glEnable(GL_CULL_FACE);		// Do not draw inside of cube
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out

	camera->x = 0;
	camera->y = 0;
	camera->z = 200;

	x_rot = 45;
	y_rot = 0;

	glClearColor(0, 0, 0, 1.0); 
}

////////////////////////////////////////////////////////////////////
//  CLEAN_UP FUNCTION
////////////////////////////////////////////////////////////////////

void clean_up (void)
{
	if (starting_rules)
		delete starting_rules;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function Name: mouse
// --------------
//
// Gets called when a mouse event occurs. registers the button presses, the state and the coordinates,
// though this program is only interested in the coordinates.
//
// Created On: 4/30/02
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void mouse(int button, int state, int x, int y)
{
	old_mouse_x = x; // Capture the coordinates of the mouse.
	old_mouse_y = y;
	
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function Name: motion
// --------------
//
// Captures mouse movement, should the mouse function be activated by a button click
//
// Created On: 4/30/02
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void motion(int x, int y)
{
	x_rot += (old_mouse_y - y) / 2.0;	// Effect the orbitting angles for the camera. 
	y_rot += (old_mouse_x - x) / 2.0;

	old_mouse_x = x;					// Set the old coordinates ready for use next time.
	old_mouse_y = y;

	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////
// RESHAPE FUNCTION
// ----------------
//
// Reshapes the program window correctly. This is passed to the OpenGL
// class and handled from there.
//
////////////////////////////////////////////////////////////////////

void reshape (int width, int height)
{
	open_gl->gl_reshape(width, height);
}

//////////////////////////////////////////////////////////////////////

void keyboard (unsigned char key, int x, int y)
{
	switch (key)							// Key is the variable to switch between. Given to us by GL
	{
		case 'w':	camera->z = camera->z - 10;

					break;

		case 's':	camera->z = camera->z + 10;

					break;

		case 'f':   glutFullScreen ();

					break;

		case 'q':   clean_up();
					exit (0);

					break;

		default:    break;
	}           
    
	glutPostRedisplay();	
}