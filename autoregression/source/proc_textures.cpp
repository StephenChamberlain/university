#include "../header/open_gl.h"
#include "../header/texture_algorithm.h"

#include <gl\glut.h>
#include <time.h>

////////////////////////////

#define	SCALE	256

////////////////////////////

void display (void);
void idle (void);

////////////////////////////

open_gl open_gl;
texture_algorithm texture_algorithm;

int scanline;

////////////////////////////

int main(int argc, char** argv)
{	
	open_gl.gl_display_setup_2d(argc, argv);

	glutDisplayFunc(display);  
	glutIdleFunc(idle);  
//	glutKeyboardFunc(keyboard);	

	glLoadIdentity();  
	gluOrtho2D(-SCALE, SCALE, -SCALE, SCALE);  

	glClearColor(0, 0, 0, 1.0); 
  
	glutMainLoop();  

	return EXIT_SUCCESS;
}

////////////////////////////

void display (void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	texture_algorithm.tex_draw_pixels();

	glutSwapBuffers();
	glutPostRedisplay();
}

////////////////////////////

void idle (void)
{
	if (scanline != 511)
	{
		scanline ++;
	}

	texture_algorithm.tex_perform_texture_algorithm(scanline);

	glutPostRedisplay();
}