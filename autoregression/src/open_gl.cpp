// open_gl.cpp: implementation of the open_gl class.
//
//////////////////////////////////////////////////////////////////////

#include "open_gl.h"
#include <gl\glut.h>

#define XPIXELS	512
#define YPIXELS	512	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

open_gl::open_gl()
{ 

}

//////////////////////////////////////////////////////////////////////

open_gl::~open_gl()
{

}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_display_setup(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the colour and depth bits

	glColorMaterial(1, 1);				// This command means that the surface colours of the objects will not be affected by the light,
										// otherwise OpenGL tends to render everything in a greyscale
	glEnable(GL_COLOR_MATERIAL);
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_display_setup_2d(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);  
	glutInitWindowSize(XPIXELS, YPIXELS);  
	glutInitWindowPosition(0, 0);  
	glutCreateWindow("Procedural Texture Algorithm");  
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_draw_sphere(int x, int y, int z, int size)
{
	glPushMatrix();
		glTranslatef(x, y, z);
		glutSolidSphere(size, 64, 64);
		glutSolidCube(size);
	glPopMatrix();
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_initialise_perspective(void)
{
	glMatrixMode(GL_PROJECTION);			// Define the basic projection system for OpenGL 
	glLoadIdentity();
	gluPerspective(45, 1.0, 1.0, 200.0);	// Set a perspective view system 
	glMatrixMode(GL_MODELVIEW);				// Set the modelview matrix mode. This means all succeeding 
	glLoadIdentity();						// transformations affect the modelview matrix stack, 
													// not the projection matrix stack 

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Use really nice perspective calculations 
	glEnable(GL_DEPTH_TEST);				// Enable the depth test so objects are drawn in the right order 
	glDepthFunc(GL_LEQUAL);	
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_perspective_viewing_system(void)
{
	glLoadIdentity();
	glTranslatef(-9.5, 9.5, 0.0);
	glTranslatef(0.0, 0.0, heading);	// Standard translation
	glRotatef( x_pos, 1.0, 0.0, 0.0 );			// Standard primary rotation
	glRotatef( y_pos, 0.0, 1.0, 0.0 );			// Secondary rotation.
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_keyboard(unsigned char key_press)
{
	switch (key_press)							// Key is the variable to switch between. Given to us by GL
	{
		case 'w':	if ( selected_point_one > 1)
					{
						selected_point_one--;
					}

					break;

		case 's':	if ( selected_point_one < 18)
					{
						selected_point_one++;
					}

					break;

		case 'a':	if ( selected_point_two > 1)
					{
						selected_point_two--;
					}

					break;

		case 'd':	if ( selected_point_two < 18)
					{
						selected_point_two++;
					}

					break;

		default:    break;
	}           
    
	glutPostRedisplay();	
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_increment_xpos(float amount)
{
	x_pos = x_pos + amount;
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_decrement_xpos(float amount)
{
	x_pos = x_pos - amount;
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_increment_ypos(float amount)
{
	y_pos = y_pos + amount;
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_decrement_ypos(float amount)
{
	y_pos = y_pos - amount;
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_light(float x, float y, float z)
{
	GLfloat mat_specular[] = { 1, 1, 1, 1 }; // I shall not go into detail with this function, but it
	GLfloat mat_shininess[] = { 50 };        // basically sets up some basic GL variables to give to the 
	GLfloat light_position[] = { x, y, z,  0 }; // lighting model
	GLfloat white_light[] = { 1, 1, 1, 1 };
	GLfloat lmodel_ambient[] = { 0.5, 0.5, 0.5, 0 };

	glClearColor(0, 0, 0, 0);
	glShadeModel(GL_SMOOTH);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glEnable(GL_LIGHTING);	// Enable GL lighting
	glEnable(GL_LIGHT0);	// Enable the light we just defined.
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_mouse(int x, int y)
{
	old_x = x; // Capture the coordinates of the mouse.
	old_y = y;
	
	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_motion(int x, int y)
{
	if (x_pos <= -90)
	{
		x_pos = -90;
	}

	if (x_pos <= 90)
	{
		x_pos = 90;
	}

	x_pos += (old_y - y) / 2.0;	// Effect the orbitting angles for the camera. 
	y_pos += (old_x - x) / 2.0;

	old_x = x;					// Set the old coordinates ready for use next time.
	old_y = y;

	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_text_renderer(float x, float y, void *font, char *string)
{
	char *c;

	glDisable(GL_DEPTH_TEST);				/* Disable the depth test and lighting. The lighting will not */
	glDisable(GL_LIGHTING);					/* work on the fonts, and the depth test will render the scene */
											/* over the hud if you get the camera closer than the near clip plane. */
	glMatrixMode(GL_PROJECTION);			/* Set the whole view system for 2D */
	glPushMatrix(); 
	glLoadIdentity(); 
	gluOrtho2D(0, XPIXELS, 0, YPIXELS); 
	glMatrixMode(GL_MODELVIEW); 
	glPushMatrix(); 
	glLoadIdentity(); 
	glColor3f(1.0, 0.0, 0.0);

	glRasterPos2f(x, y);					/* set position to start drawing fonts */
	
	for (c=string; *c != '\0'; c++)			/* loop all the characters in the string */
    {
	  glutBitmapCharacter(font, *c);
    }

	glPopMatrix();							/* Reset the projection view system */
	glMatrixMode(GL_PROJECTION); 
	glPopMatrix(); 
	glMatrixMode(GL_MODELVIEW);
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_idle(void)
{	
	int	tag;

	if (update <= 1)
	{
		tag = 1;
	}
	else if (update >= 0)
	{
		tag = 0;
	}

	if (tag == 1)
	{
		update = update + 0.01;
	}
	else if (tag == 0)
	{
		update = update - 0.01;
	}

	glutPostRedisplay();
}

