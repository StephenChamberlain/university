#include "../header/open_gl.h"
#include "../header/c_dlaa.h"
#include <gl\glut.h>
#include <time.h>

////////////////////////////

#define	SCALE	200

////////////////////////////

void display (void);
void idle (void);
void keyboard (unsigned char, int, int);

////////////////////////////

open_gl open_gl1;
c_dlaa dlaa;

////////////////////////////

int main(int argc, char **argv)
{
	open_gl1.gl_display_setup_2d(argc, argv);

	glutDisplayFunc(display);  
	glutIdleFunc(idle);  
	glutKeyboardFunc(keyboard);	

	glLoadIdentity();  
	gluOrtho2D(-SCALE, SCALE, -SCALE, SCALE);  

	dlaa.initialise_class();

	glClearColor(0, 0, 0, 1.0); 
  
	glutMainLoop();  

	return EXIT_SUCCESS;
}

////////////////////////////

void display (void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	dlaa.draw_pixels();

	glutSwapBuffers();
	glutPostRedisplay();
}

////////////////////////////

void idle (void)
{
	int seed, rand_no, i;

	seed = time(NULL);
	srand(seed);

	rand_no = rand()%99999;
	srand(rand_no);

	rand_no = rand()%99999;

	for (i=0; i<1000000; i++)
	{
		dlaa.peform_diffusion_limited_aggregation(rand_no);
	}
}

////////////////////////////

void keyboard(unsigned char key, int x, int y)
{
	switch (key)							// Key is the variable to switch between. Given to us by GL
	{
		case '1':	dlaa.colour[0] = 1;
					dlaa.colour[1] = 0;
					dlaa.colour[2] = 0;

					break;

		case '2':	dlaa.colour[0] = 0;
					dlaa.colour[1] = 1;
					dlaa.colour[2] = 0;

					break;

		case '3':	dlaa.colour[0] = 0;
					dlaa.colour[1] = 0;
					dlaa.colour[2] = 1;

					break;

		default:	break;
	}           
    
	glutPostRedisplay();
}