// OpenGL.cpp: implementation of the COpenGL class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/OpenGL.h"

#include <gl/glut.h>
#include <time.h>			// Used for timing
#include <windows.h>		// Windows. Some windows functions are used
#include <iostream>			// Input, output 
#include <fstream>			// File management 

////////////////////////////////////////////////////////////////////
// ENUMERATED TYPES 
////////////////////////////////////////////////////////////////////

#define XPIXELS			768		// Number of pixels in X direction
#define YPIXELS			768		// Number of pixels in Y direction

enum render_mode {WIRE, SOLID};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenGL::COpenGL()
{
	font = (int)GLUT_BITMAP_TIMES_ROMAN_24 ;	// GLUT font to use on the HUD
}

//////////////////////////////////////////////////////////////////////

COpenGL::~COpenGL()
{

}

//////////////////////////////////////////////////////////////////////

void COpenGL::gl_keyboard (unsigned char key_press, int x, int y)
{
	switch (key_press)							// Key is the variable to switch between. Given to us by GL
	{
		case 'q':	exit (0);

					break;

		default:    break;
	}           
    
	glutPostRedisplay();	
}

//////////////////////////////////////////////////////////////////////

void COpenGL::gl_reshape (int width, int height)
{
	glViewport(0, 0, width, height);				/* Create the viewport with the new values */

	glMatrixMode(GL_PROJECTION);					/* Re-initialise the projection mode */
	glLoadIdentity();
	gluPerspective(45, (float)width / (float)height, 0.1, 10000.0); /* Set the new perspective matrix */
	glMatrixMode(GL_MODELVIEW);						/* Reset the modelview mode */
	glLoadIdentity();
}

//////////////////////////////////////////////////////////////////////

int COpenGL::get_fullscreen_x (void)
{
	return fullscreen_x;
}

//////////////////////////////////////////////////////////////////////

int COpenGL::get_fullscreen_y (void)
{
	return fullscreen_y;
}

//////////////////////////////////////////////////////////////////////

void COpenGL::set_fullscreen_x (int new_fullscreen_x)
{
	fullscreen_x = new_fullscreen_x;
}

//////////////////////////////////////////////////////////////////////

void COpenGL::set_fullscreen_y (int new_fullscreen_y)
{
	fullscreen_y = new_fullscreen_y;
}

//////////////////////////////////////////////////////////////////////

void COpenGL::set_perspective(void)
{
	glColorMaterial(1, 1);				// This command means that the surface colours of the objects will not be affected by the light,
										// otherwise OpenGL tends to render everything in a greyscale
	glEnable(GL_COLOR_MATERIAL);

	glMatrixMode(GL_PROJECTION);			// Define the basic projection system for OpenGL 
	glLoadIdentity();
	gluPerspective(45, 1.0, 0.1, 10000.0);	// Set a perspective view system 
//	glFrustum(-10, 10, -10, 10, 0.1, 10000.0);
	glMatrixMode(GL_MODELVIEW);				// Set the modelview matrix mode. This means all succeeding 
	glLoadIdentity();						// transformations affect the modelview matrix stack, 
													// not the projection matrix stack 

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST); // Use really nice perspective calculations 
	glEnable(GL_DEPTH_TEST);				// Enable the depth test so objects are drawn in the right order 
	glDepthFunc(GL_LEQUAL);	

//	glEnable(GL_NORMALIZE);
	glCullFace(GL_BACK);
}

//////////////////////////////////////////////////////////////////////

void COpenGL::set_2d_viewing_system(void)
{
	glLoadIdentity();  
	gluOrtho2D(0, 800, 0, 600); 
}

//////////////////////////////////////////////////////////////////////

void COpenGL::gl_fp_viewing_system(float x, float y, float z, float x_rotation, float y_rotation)
{
	glLoadIdentity();

	glTranslatef(x, y, z);
	glRotatef(x_rotation, 1.0, 0.0, 0.0 );			// Secondary rotation.
	glRotatef(y_rotation, 0.0, 1.0, 0.0 );			// Standard primary rotation
}

//////////////////////////////////////////////////////////////////////

void COpenGL::draw_point(float x, float y, float z)
{
	glPointSize(10);

	glBegin(GL_POINTS);
		glVertex3f(x, y, z);
	glEnd();
}

//////////////////////////////////////////////////////////////////////

void COpenGL::draw_line(float x, float y, float z, float x1, float y1, float z1)
{
	glBegin(GL_LINES);
		glVertex3f(x, y, z);
		glVertex3f(x1, y1, z1);
	glEnd();
}

//////////////////////////////////////////////////////////////////////

void COpenGL::draw_sphere(float x, float y, float z, int mode)
{
	glPushMatrix();
		glTranslatef(x, y, z);

		switch (mode)							// Key is the variable to switch between. Given to us by GL
		{
			case WIRE:	glutWireSphere(4.5, 32, 32);

						break;

			case SOLID:	glutSolidSphere(1000, 32, 32);

						break;

			default:    break;
		} 
	
	glPopMatrix();
}

//////////////////////////////////////////////////////////////////////

void COpenGL::set_light_and_properties(float x, float y, float z, float ambience, float specular, float white_light1)
{
	GLfloat mat_specular[] = { specular, specular, specular, 1 }; // I shall not go into detail with this function, but it
	GLfloat mat_shininess[] = { 0.5 };        // basically sets up some basic GL variables to give to the 
	GLfloat light_position[] = { x, y, z,  0 }; // lighting model
	GLfloat white_light[] = { white_light1, white_light1, white_light1, white_light1 };
	GLfloat lmodel_ambient[] = { ambience, ambience, ambience, 0 };

//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glShadeModel(GL_SMOOTH);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glClearColor(0, 0, 0, 0);
	glColorMaterial(1, 1);	

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

//////////////////////////////////////////////////////////////////////

void COpenGL::gl_text_renderer(float x, float y, void *font, char *string, float r, float g, float b)
{
	char *c;

	glDisable(GL_DEPTH_TEST);				/* Disable the depth test and lighting. The lighting will not */
	glDisable(GL_LIGHTING);					/* work on the fonts, and the depth test will render the scene */
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);			/* Set the whole view system for 2D */
	glPushMatrix(); 
	glLoadIdentity(); 
	gluOrtho2D(0, XPIXELS, 0, YPIXELS); 
	glMatrixMode(GL_MODELVIEW); 
	glPushMatrix(); 
	glLoadIdentity(); 
	glColor3f(r, g, b);

	glRasterPos2f(x, y);					/* set position to start drawing fonts */
	
	for (c=string; *c != '\0'; c++)			/* loop all the characters in the string */
    {
	  glutBitmapCharacter(font, *c);
    }

	glPopMatrix();							/* Reset the projection view system */
	glMatrixMode(GL_PROJECTION); 
	glPopMatrix(); 
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);				/* Disable the depth test and lighting. The lighting will not */
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
}

//////////////////////////////////////////////////////////////////////

void COpenGL::draw_textured_polygon(float x1, float y1, float z1,
									float x2, float y2, float z2,
									float x3, float y3, float z3,
									float x4, float y4, float z4)
{
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(x1, y1, z1);
		glVertex3f(x1, y1, z1);

		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(x4, y4, z4);
		glVertex3f(x4, y4, z4);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(x3, y3, z3);
		glVertex3f(x3, y3, z3);

		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(x2, y2, z2);
		glVertex3f(x2, y2, z2);
	glEnd();
}

//////////////////////////////////////////////////////////////////////

void COpenGL::text_renderer(float x, float y, void *font, char *string, float r, float g, float b)
{
	char *c;

	glDisable(GL_DEPTH_TEST);				/* Disable the depth test and lighting. The lighting will not */
	glDisable(GL_LIGHTING);					/* work on the fonts, and the depth test will render the scene */
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);			/* Set the whole view system for 2D */
	glPushMatrix(); 
	glLoadIdentity(); 
	gluOrtho2D(0, XPIXELS, 0, YPIXELS); 
	glMatrixMode(GL_MODELVIEW); 
	glPushMatrix(); 
	glLoadIdentity(); 
	glColor3f(r, g, b);

	glRasterPos2f(x, y);					/* set position to start drawing fonts */
	
	for (c=string; *c != '\0'; c++)			/* loop all the characters in the string */
    {
	  glutBitmapCharacter(font, *c);
    }

	glPopMatrix();							/* Reset the projection view system */
	glMatrixMode(GL_PROJECTION); 
	glPopMatrix(); 
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);				/* Disable the depth test and lighting. The lighting will not */
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
}

////////////////////////////////////////////////////////////////////
// DRAW_HUD FUNCTION
// -----------------
//
// Draws the HUD on the screen
//
////////////////////////////////////////////////////////////////////

void COpenGL::draw_hud (float fps)
{
	glDisable(GL_TEXTURE_2D);

	sprintf_s(text_buffer,"%.0f",fps);
	text_renderer(10,YPIXELS - 20,(void *)font, text_buffer, 1.0, 1.0, 1.0);

	sprintf_s(text_buffer,"Frames Per Second");
	text_renderer(30,YPIXELS - 20,(void *)font, text_buffer, 1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);
}