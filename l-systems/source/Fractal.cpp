// Fractal.cpp: implementation of the CFractal class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/Fractal.h"
#include <GL/glut.h>		// Glut 		
#include <GL/gl.h>			// OpenGl 

#define XPIXELS				768		// Number of pixels in X direction
#define YPIXELS				768
#define	SCALE	200

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFractal::CFractal()
{
	int i, j;

	for (i=0; i<XPIXELS; i++)
	{
		for (j=0; j<YPIXELS; j++)
		{
			fractal[i][j].red = 1;
			fractal[i][j].green = 0;
			fractal[i][j].blue = 0;
		}
	}
}

//////////////////////////////////////////////////////////////////////

CFractal::~CFractal()
{

}

//////////////////////////////////////////////////////////////////////

void CFractal::draw_fractal(void)
{
	int i, j;
	GLubyte ubImage[65536];

	glClear(GL_COLOR_BUFFER_BIT);

	for(i=0; i<XPIXELS; i++)
	{
		for(j=0; j<YPIXELS; j++)
		{
			glRasterPos2f((i*1.0)-199.5, (j*1.0)-199.5);
			glColor3f(fractal[i][j].red, fractal[i][j].green, fractal[i][j].blue);
			glDrawPixels(1, 1, GL_RED, GL_UNSIGNED_BYTE, ubImage);
		}
	}

	glutSwapBuffers();	
}

//////////////////////////////////////////////////////////////////////

void CFractal::set_2d_display(void)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);  
	glutInitWindowSize(XPIXELS, YPIXELS);  
	glutInitWindowPosition(0, 0);  
	glutCreateWindow("Fractal setup"); 

	glLoadIdentity();  
	gluOrtho2D(-SCALE, SCALE, -SCALE, SCALE);
}