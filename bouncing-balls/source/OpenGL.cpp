// OpenGL.cpp: implementation of the COpenGL class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/OpenGL.h"
#include <GL/glut.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenGL::COpenGL()
{

}

//////////////////////////////////////////////////////////////////////

COpenGL::~COpenGL()
{

}

//////////////////////////////////////////////////////////////////////

void COpenGL::GLSetup(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the colour and depth bits

	glColorMaterial(1, 1);				// This command means that the surface colours of the objects will not be affected by the light,
										// otherwise OpenGL tends to render everything in a greyscale
	glEnable(GL_COLOR_MATERIAL);
}

//////////////////////////////////////////////////////////////////////

void COpenGL::InitPerspective(void)
{
	glMatrixMode(GL_PROJECTION);			// Define the basic projection system for OpenGL 
	glLoadIdentity();
	gluPerspective(45, 1.0, 0.1, 10000.0);	// Set a perspective view system 
	glMatrixMode(GL_MODELVIEW);				// Set the modelview matrix mode. This means all succeeding 
	glLoadIdentity();						// transformations affect the modelview matrix stack, 
											// not the projection matrix stack 

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST); // Use really nice perspective calculations 
	glEnable(GL_DEPTH_TEST);				// Enable the depth test so objects are drawn in the right order 
	glDepthFunc(GL_LEQUAL);	

//	glEnable(GL_LIGHTING);					// Enable GL lighting
}

//////////////////////////////////////////////////////////////////////

void COpenGL::PerspectiveViewingSystem(float x, float y, float z, float xRotation, float yRotation)
{
	glLoadIdentity();

	glTranslatef(-x, -y, -z);
	glRotatef(xRotation, 1.0, 0.0, 0.0 );	// Secondary rotation.
	glRotatef(yRotation, 0.0, 1.0, 0.0 );	// Standard primary rotation
}

//////////////////////////////////////////////////////////////////////

void COpenGL::Reshape(int width, int height)
{
	glViewport(0, 0, width, height);				/* Create the viewport with the new values */

	glMatrixMode(GL_PROJECTION);					/* Re-initialise the projection mode */
	glLoadIdentity();
	gluPerspective(45, (float)width / (float)height, 0.1, 10000.0); /* Set the new perspective matrix */
	glMatrixMode(GL_MODELVIEW);						/* Reset the modelview mode */
	glLoadIdentity();			/* Shift origin up to upper-left corner. */
}

//////////////////////////////////////////////////////////////////////

void COpenGL::Point(float x1, float y1, float z1, int size)
{
	glPointSize(size);

	glBegin(GL_POINTS);
	//	glColor3f(1.0, 0.0, 0.0);  /* red */
		glVertex3f(x1, y1, z1);
	glEnd();
}

//////////////////////////////////////////////////////////////////////

void COpenGL::Sphere(float x1, float y1, float z1, int size)
{
	glPushMatrix();
		glTranslatef(x1, y1, z1);
		glutSolidSphere(size, 32, 32);
	glPopMatrix();
}

//////////////////////////////////////////////////////////////////////

void COpenGL::Line(float x1, float y1, float z1,
		 		   float x2, float y2, float z2)
{
	glLineWidth(2);

	glEnable(GL_LINE_SMOOTH);

	glBegin(GL_LINES);
		glColor3f(1.0, 1.0, 1.0);
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
	glEnd();
}

//////////////////////////////////////////////////////////////////////

void COpenGL::Triangle(float x1, float y1, float z1,
		  			   float x2, float y2, float z2,
					   float x3, float y3, float z3)
{
	glBegin(GL_TRIANGLES);
		glColor3f(0.0, 0.0, 1.0);  /* blue */
		glVertex3f(x1, y1, z1);

		glColor3f(0.0, 1.0, 0.0);  /* green */
		glVertex3f(x2, y2, z2);

		glColor3f(1.0, 0.0, 0.0);  /* red */
		glVertex3f(x3, y3, z3);
	glEnd();
}