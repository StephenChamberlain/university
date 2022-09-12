/*******************************************************************************************

 Title: Graphical Programming ICA
 --------------------------------
                               
 Author: Stephen Chamberlain
 ---------------------------
							   
 Date: 2002										       
 ----------

 List of features:
 -----------------

   - File reader, reads in data from data.txt
   - Dynamic memory allocation
   - Matrix and vector multiplication
   - Allows user to use mouse to move round the object
   - Control system for ease of manipulation

 List of items to query:
 -----------------------   

   - Matrix calculations 
   - Reshape function
   - Viewing system 

********************************************************************************************/

/*******************************************************************************************

 Includes, essential header files that are required

********************************************************************************************/

#include <stdlib.h>							/* Standard librarys */						
#include <stdio.h>
#include <math.h>							/* Used for COS and SIN functions */
#include <GL/glut.h>						/* Standard OpenGL librarys */

/********************************************************************************************

 Definition of enumerated types to make program more readable 

********************************************************************************************/

enum Object {COORDS, EDGES, POLY, CURVE};			/* Specifies what part of an object to draw */			
enum Transformations {VIEW, PERSP};			/* Used for switching between transformation calculations */

/********************************************************************************************

 User Defined Data Types 

********************************************************************************************/

typedef struct{								/* Custom data-type for matrices */
	float c[4][4];
} Matrix3D;

typedef struct{								/* Custom data-type for matrices */
	float x[3];
	float y[3];
} PolyPoints;

/********************************************************************************************

 Constant Values, makes the code easier to understand

********************************************************************************************/

#define SCALE		15						/* Viewport scale */
#define NPIXELS		800						/* No. of pixels in the window */
#define DEG2RAD		0.0174532925199f		/* constant value used to transform DEGREE angles into RADIAN  */
#define HOMO		4						/* Value for homogeneous vectors */
#define VER			3						/* Constant value used for dynamic memory allocation */
#define SIZE		3
#define NUMBEZCP	5

/********************************************************************************************

 Function Prototypes for the various function used in the program

********************************************************************************************/

void readInObject(void);					/* Read the scene in from a text file */
void readInitialValues(void);				/* Reads in the initial viewing system values */			
void printMenu(void);						/* Prints the controls on screen */
void display(void);							/* Main function for glut callback glutDisplayFunc */
void viewingSystem(void);					/* Does all the matrix calculations on the vertices in the scene */
void keyboard(unsigned char, int, int);		/* Main function for glut callback glutKeyboardFunc */
void drawAxes(void);						/* Drawes 3 axes on the screen, x, y and z */
void drawObject(enum Object);				/* Draw the objects read from the text file */
void special(int, int, int);				/* Handles the keyboard special keys */
void mainMenuFunc(int item);				/* Code for delivering a floating menu in OpenGL */
void initialisation(void);					/* Set the initial values of the system */
void mouse(int, int, int, int);				/* Records the position on the mouse when a button is pressed */
void motion(int, int);						/* Function for handling mouse movement when a button is pressed */
float **alloc_mem_2Darray_float(int, int);	/* Memory allocation function for the VERTICES in the scene */
int **alloc_mem_2Darray_int(int, int);		/* Memory allocation function for the POLYGONS in the scene */
void drawline(float, float, float, float );
void drawpoly(float, float, float, float, float, float, int);
//void reshape(int, int);						/* Function for reshaping the viewport */
void calcNormals (int, int, int);
float flatShade(int);
int back_face_culling (int);
PolyPoints bubbleSort (PolyPoints);
void bezier (void);
void draw_bezier_sphere(void);

/* Function prototypes for matrix calculations */

void matrix_t1(void);						/* Functions for the viewing system */
void matrix_t2(void);
void matrix_t3(void);
void matrix_t4(void);
void matrix_tpers(void);
void matrix_translate(void);

void matrix_calc(enum Transformations, Matrix3D *);	/* Function to perform the actual calculations on the vertices */

/********************************************************************************************

 Global Variables used by the program. Though they present a memory overhead, they are without
 doubt the easiest variables to work with.

********************************************************************************************/

/* Object Variables. Used for the actual scene's vertices and polygons */

int nVertices = 0;							/* The number of vertices in the scene */
int nFaces = 0;								/* The number of polygons or faces in the scene */
float **vertex;								/* Dynamic array */
int **face;									/* Dynamic array */
float **normals;

/* Program Variables. Used and modified by the program */

float cam_pos[3];							/* The camera angle, in terms of u, theta and phi */
float translate[3];
float light[3];
int old_mouse[2];							/* The old mouse position. This is changed when the mouse function gets called */
float d;									/* Variable used for the perspective transformation. Distance to viewplane */ 

/* Bezier control points */

float bez_points[4][4];

/********************************************************************************************/
/*********************** THE MAIN PROGRAM ***************************************************/
/********************************************************************************************/


/********************************************************************************************

 main
 ----

 This is the standard main function for the program.

********************************************************************************************/

int main(int argc, char **argv)
{
	int mainMenu;									/* Variable used for the menu function */

	/********** Glut Start up Specs ***********/
	glutInit(&argc, argv);						
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);	/* GLUT_DOUBLE allows double-buffering */
	glutInitWindowSize(NPIXELS, NPIXELS);			/* Set the window size */
	glutInitWindowPosition(0, 0);					/* Specifies where the window is to be placed */
	glutCreateWindow("Stephen Chamberlain :: Bezier spline using linear interpolation");		/* The title of the application */
	/******************************************/

	/**** Assign GL callback functions ********/
	glutDisplayFunc(display);						/* Callback function jumps to display */	
	glutKeyboardFunc(keyboard);						/* Callback function jumps to keyboard */	
	glutSpecialFunc(special);						/* Callback function jumps to special */
	glutMotionFunc(motion);							/* Callback function jumps to motion */
//	glutReshapeFunc(reshape);						/* Callback function jumps to reshape */
	/******************************************/

	/**** GLUT Menu specifications ************/
	mainMenu = glutCreateMenu(&mainMenuFunc);		/* When the menu is called, jump to mainMenuFunc */
	glutSetMenu(mainMenu);							
	glutAddMenuEntry("Reset", 0);					/* Menu option for reseting the initial values */
	glutAddMenuEntry("Exit", 1);					/* Menu option for exiting the application */
	glutAttachMenu(GLUT_LEFT_BUTTON);				/* Attach the menu callback to the left mouse button */
	/******************************************/

	initialisation();								/* Initialise the system */
	printMenu();									/* Display the controls for the user */

	glClearColor(0.5f, 0.5f, 0.5f, 1.f);			/* Set the colour of the background */
  
	glutMainLoop();									/* Call the main GL looping function */

	return EXIT_SUCCESS;							/* Return success */
}

/********************************************************************************************

 readInObject
 ------------

 This function reads in the objects for the scene from a text file, data.txt and stores them
 in global variables. There is no return type.

********************************************************************************************/

void readInObject (void)
{
   int count, pta, ptb;						/* Variable for looping */
   FILE *fileptr;							/* Pointer for the file that is to be read in */

   if (vertex)								/* If either vertex or face have been previously */
   {										/* declared, wipe them clean */
		free(vertex);
		vertex=NULL;
   }

   if (face)
   {
		free(face);
		face=NULL;
   }

   fileptr = fopen( "data.txt", "r" );		/* Tell the program to open data.txt */
   
   if( fileptr == NULL )					/* Deal with errors */
   {
      printf( "Error - The data file was not opened.\n" );
   }
   else
   {
      fseek( fileptr, 0L, SEEK_SET );		/* Set pointer to beginning of file */
	
	  fscanf(fileptr, "%d", &nVertices);	/* Scan in the number of vertices */

	  vertex = alloc_mem_2Darray_float(nVertices, HOMO);	/* Allocate some memory for that number of vertices */

	  for(count=0; count<nVertices; count++)	/* Read in the vertices */
		{
			fscanf(fileptr, "%f", &(vertex[count][0]));
			fscanf(fileptr, "%f", &(vertex[count][1]));
			fscanf(fileptr, "%f", &(vertex[count][2]));	
			fscanf(fileptr, "%f", &(vertex[count][3]));	
		//	printf("%d\n", count);
        }

	  fscanf(fileptr, "%d", &nFaces);		/* Now read in the number of polygons */

	  face = alloc_mem_2Darray_int(nFaces, VER);	/* Allocate memory again */
	  normals = alloc_mem_2Darray_float(nFaces, VER);

	  for(count=0; count<nFaces; count++)	/* Read in the polygons */
		{
			fscanf(fileptr, "%d", &(face[count][0]));
			fscanf(fileptr, "%d", &(face[count][1]));
			fscanf(fileptr, "%d", &(face[count][2]));

			pta = (face[count][0]);
			ptb = (face[count][1]);

			calcNormals(pta, ptb, count);
		}

      fclose(fileptr);						/* Close the file before exiting the function */
   }
}

/********************************************************************************************

 readInitialValues
 -----------------

 This function reads in the initial values for the viewing system.

********************************************************************************************/

void readInitialValues (void)
{
   FILE *fileptr;							/* Pointer for the file that is to be read in */

   fileptr = fopen( "initial.txt", "r" );	/* Tell the program to open initial.txt */
   
   if( fileptr == NULL )					/* Deal with errors */
   {
      printf( "Error - The initialisation file was not opened.\n" );
   }
   else
   {
      fseek( fileptr, 0L, SEEK_SET );		/* Set pointer to beginning of file */
	
	  fscanf(fileptr, "%f", &cam_pos[0]);	/* Scan in the initial camera position */
	  fscanf(fileptr, "%f", &cam_pos[1]);
	  fscanf(fileptr, "%f", &cam_pos[2]);		

	  fscanf(fileptr, "%f", &d);

	  fscanf(fileptr, "%f", &old_mouse[0]);
	  fscanf(fileptr, "%f", &old_mouse[1]);

	  fscanf(fileptr, "%f", &translate[0]);	
	  fscanf(fileptr, "%f", &translate[1]);
	  fscanf(fileptr, "%f", &translate[2]);

      fclose(fileptr);						/* Close the file before exiting the function */
   }
}

/********************************************************************************************

 readInitialValues
 -----------------

 This function reads in the initial values for the viewing system.

********************************************************************************************/

void readBezierPoints (void)
{
   FILE *fileptr;							/* Pointer for the file that is to be read in */
   int i;

   fileptr = fopen( "bezier.txt", "r" );	/* Tell the program to open initial.txt */
   
   if( fileptr == NULL )					/* Deal with errors */
   {
      printf( "Error - The bezier file was not opened.\n" );
   }
   else
   {
      fseek( fileptr, 0L, SEEK_SET );		/* Set pointer to beginning of file */
	
	  for (i=0; i<5; i++)
	  {
		fscanf(fileptr, "%f", &bez_points[i][0]);	/* Scan in the initial camera position */
		fscanf(fileptr, "%f", &bez_points[i][1]);
		fscanf(fileptr, "%f", &bez_points[i][2]);
		fscanf(fileptr, "%f", &bez_points[i][3]);
	  }

      fclose(fileptr);						/* Close the file before exiting the function */
   }

}

/********************************************************************************************

 printMenu
 ---------

 This function prints the programs controls in the command window, be it the Linux console or
 the MS-DOS command window.

********************************************************************************************/

void printMenu(void)
{
	printf("\n\n-------------------------------\n");
	printf("\nControls:\n\n");
	printf("\n\n-------------------------------\n");
}

/********************************************************************************************

 initialisation
 --------------

 This function sets the OpenGL system ready for display. I have included where possible, OpenGL
 commands for the operations that I perform, therefore allowing the viewer to see what I have 
 done more clearly.

********************************************************************************************/

void initialisation(void)
{
	/* Shown here is an equivalent projection version for use by OpenGL */

	/*
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-2.0, 2.0, -2.0, 2.0, 3.0, 50.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	*/

	gluOrtho2D(-SCALE, SCALE, -SCALE, SCALE);	/* A basic 2D setup. All frustum/ perspective calculations
												   are performed manually from now on */	

	readInitialValues();					/* Read in the intial viewing system values */

	light[0] = 5;
	light[1] = 5;
	light[2] = 5;
		
	return;
}

/********************************************************************************************

 display
 -------

 This function is the epi-centre of the whole process, the matrices and vertices are called
 and calculated here, drawn to the screen and the whole process starts again.

********************************************************************************************/

void display(void) 
{ 
	glClear(GL_COLOR_BUFFER_BIT);			/* Clear the viewable screen first */

	readInObject();							/* Call the read in object function to read in the scene */
	readBezierPoints();

	viewingSystem();						/* Perform the matrix calculations */

//	drawAxes();								/* Help development by drawing the x, y and z axes in */

//	drawObject(EDGES);						/* Render the lines of the polygons */
//	drawObject(COORDS);						/* Render the points of the vertices */
//	drawObject(POLY);
	drawObject(CURVE);

	glutSwapBuffers();						/* This is the command to use double buffering 
											   swap the frame that was just drawn */
}

/********************************************************************************************

 viewingSystem
 -------------

 This function does all the hard work. It initialises and populates the transfromation matrices
 used to transform the vertices from world to screen co-ordinates. Each of these functions then
 send the resulting matrix to matrix_calc which performs the vector/matrix multiplications
 necessary for the complete scene to be rendered.

********************************************************************************************/

void viewingSystem(void)
{
	/* OpenGL Transformations - these are used in place of my own transformations for OpenGL. 

		glLoadIdentity();
		glTranslatef(0.0, 0.0, -cam_pos[0]);
		glRotatef(-camera[1], 1.0, 0.0, 0.0);
		glRotatef(-camera[2], 0.0, 1.0, 0.0);
	*/

	matrix_t1();							/* Zoom matrix for the specified zoom factor */
	matrix_t2();
	matrix_t3();
	matrix_t4(); 
	matrix_translate();
    matrix_tpers();

	return;
}

/********************************************************************************************

 drawAxes
 --------

 Simple function to draw axis. Does exactly what it says it does.

********************************************************************************************/

void drawAxes(void)
{
	glLineWidth(1);

	glBegin(GL_LINES);

		glColor3f(1.0, 0.0, 0.0);			/* RED X axis */
		glVertex3f(-1.0, 0.0, 0.0);
		glVertex3f(3.5, 0.0, 0.0);

		glColor3f(1.0, 1.0, 0.0);			/* YELLOW Y axis */			
		glVertex3f(0.0, -1.0, 0.0);
		glVertex3f(0.0, 3.5, 0.0);

		glColor3f(0.0, 0.0, 1.0);			/* BLUE Z axis */
		glVertex3f(0.0, 0.0, -1.0);
		glVertex3f(0.0, 0.0, 3.5);

	glEnd();
}

/********************************************************************************************

 drawObject
 ----------

 Function to draw the actual scene. The function takes the enumerated type OBJECT as a parameter
 so that the function knows whether to draw lines or simply plot the vertices. These are then 
 drawn using OpenGL commands GL_POINTS and GL_LINE_LOOP.

********************************************************************************************/

void drawObject(enum Object OBJECT) 
{ 
	int count;								/* Used for looping */
	int i;
	float ax, ay, bx, by, cx, cy, x, y;	
	int vertatemp, vertbtemp, vertctemp, cull;

	cull = 1;
							
	switch(OBJECT)							/* Enumerated type, OBJECT */
	{
	case COORDS:							/* If COORDS then draw the points */

		glColor3f(1.f, 0.f, 0.f);

		glPointSize(10);

		for(count=0; count<nVertices; count++)
		{
			x = vertex[count][0];
			y = vertex[count][1];

	//		printf("X %d: %f\n", count, x);
	//		printf("Y %d: %f\n", count, y);

			glBegin(GL_POINTS);
				glVertex2f(x, y);		/* By now the z component no longer exists */ 
			glEnd();						/* so just show the x and y co-ordinates */
		}

	//	for(count=0; count<nFaces; count++)
	//	{
	//		x = normals[count][0];
	//		y = normals[count][1];

	//		printf("X %d: %f\n", count, x);
	//		printf("Y %d: %f\n", count, y);

	//		glBegin(GL_POINTS);
	//			glVertex2f(x, y);		/* By now the z component no longer exists */ 
	//		glEnd();						/* so just show the x and y co-ordinates */
	//	}

		break;

	case EDGES:								/* If EDGES, then draw the lines between the points */

		for(i=0; i<nFaces; i++)
		{
			vertatemp = face[i][0];
			vertbtemp = face[i][1];
			vertctemp = face[i][2];
			
			ax = vertex[vertatemp][0];
			ay = vertex[vertatemp][1];

			bx = vertex[vertbtemp][0];
			by = vertex[vertbtemp][1];

			cx = vertex[vertctemp][0];
			cy = vertex[vertctemp][1];

			drawline(ax, ay, bx, by);
			drawline(bx, by, cx, cy);
			drawline(cx, cy, ax, ay);
		}
		
		break;

	case POLY:

		for(i=0; i<nFaces; i++)
		{
			vertatemp = face[i][0];
			vertbtemp = face[i][1];
			vertctemp = face[i][2];
			
			ax = vertex[vertatemp][0];
			ay = vertex[vertatemp][1];

			bx = vertex[vertbtemp][0];
			by = vertex[vertbtemp][1];

			cx = vertex[vertctemp][0];
			cy = vertex[vertctemp][1];

		//	cull = back_face_culling(i);

		//	if ( cull == 0 )
		//	{
				drawpoly(ax, ay, bx, by, cx, cy, i);
		//	}
		}

		break;

	case CURVE:
		
		bezier();

		break;

	default:		break;					/* Default exit condition, in this case anything other than above */
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function Name:bezier uses de casteljau's algorithm
// --------------
//
// Created On: 3/25/02
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void bezier (void)
{
	float B0[4][3];
	float B1[3][3];
	float B2[2][3];
	float B3[20][3]; // 1/0.05 = 20 times the curve gets called
	float u;
	int count, i;

	count = 0;

	for (u=0; u<1; u=u + 0.05)
	{
		B0[0][0] = ((1-u) * bez_points[0][0]) + (u * bez_points[1][0]);  // =a
		B0[0][1] = ((1-u) * bez_points[0][1]) + (u * bez_points[1][1]);  
		B0[0][2] = ((1-u) * bez_points[0][2]) + (u * bez_points[1][2]);

		B0[1][0] = ((1-u) * bez_points[1][0]) + (u * bez_points[2][0]);  // =b
		B0[1][1] = ((1-u) * bez_points[1][1]) + (u * bez_points[2][1]);  
		B0[1][2] = ((1-u) * bez_points[1][2]) + (u * bez_points[2][2]);

		B0[2][0] = ((1-u) * bez_points[2][0]) + (u * bez_points[3][0]);  // =c
		B0[2][1] = ((1-u) * bez_points[2][1]) + (u * bez_points[3][1]);  
		B0[2][2] = ((1-u) * bez_points[2][2]) + (u * bez_points[3][2]);

		B0[3][0] = ((1-u) * bez_points[3][0]) + (u * bez_points[4][0]);  // =d
		B0[3][1] = ((1-u) * bez_points[3][1]) + (u * bez_points[4][1]);  
		B0[3][2] = ((1-u) * bez_points[3][2]) + (u * bez_points[4][2]);

		drawline(B0[0][0], B0[0][1], B0[1][0], B0[1][1]);
	//	drawline(B0[1][0], B0[1][1], B0[2][0], B0[2][1]);
	//	drawline(B0[2][0], B0[2][1], B0[3][0], B0[3][1]);
		
		///////////////////////////////////////////////

		B1[0][0] = ((1-u) * B0[0][0]) + (u * B0[1][0]);  // =e
		B1[0][1] = ((1-u) * B0[0][1]) + (u * B0[1][1]);  
		B1[0][2] = ((1-u) * B0[0][2]) + (u * B0[1][2]);

		B1[1][0] = ((1-u) * B0[1][0]) + (u * B0[2][0]);  // =f
		B1[1][1] = ((1-u) * B0[1][1]) + (u * B0[2][1]);  
		B1[1][2] = ((1-u) * B0[1][2]) + (u * B0[2][2]);

		B1[2][0] = ((1-u) * B0[2][0]) + (u * B0[3][0]);  // =g
		B1[2][1] = ((1-u) * B0[2][1]) + (u * B0[3][1]);  
		B1[2][2] = ((1-u) * B0[2][2]) + (u * B0[3][2]);

		drawline(B1[0][0], B1[0][1], B1[1][0], B1[1][1]);
	//	drawline(B1[1][0], B1[1][1], B1[2][0], B1[2][1]);
		
		///////////////////////////////////////////////

		B2[0][0] = ((1-u) * B1[0][0]) + (u * B1[1][0]);  // =h
		B2[0][1] = ((1-u) * B1[0][1]) + (u * B1[1][1]);  
		B2[0][2] = ((1-u) * B1[0][2]) + (u * B1[1][2]);

		B2[1][0] = ((1-u) * B1[1][0]) + (u * B1[2][0]);  // =i
		B2[1][1] = ((1-u) * B1[1][1]) + (u * B1[2][1]);  
		B2[1][2] = ((1-u) * B1[1][2]) + (u * B1[2][2]);

		drawline(B2[0][0], B2[0][1], B2[1][0], B2[1][1]);
		
		///////////////////////////////////////////////

		if ( count < 20 )
		{
			B3[count][0] = ((1-u) * B2[0][0]) + (u * B2[1][0]);  // =j
			B3[count][1] = ((1-u) * B2[0][1]) + (u * B2[1][1]);  
			B3[count][2] = ((1-u) * B2[0][2]) + (u * B2[1][2]);

			count++;
		}
	}

//	for ( i=0; i<count; i++)
//	{
//		glPointSize(5);
//
//		glBegin(GL_POINTS);
//			glColor3f(0.0f, 0.0f, 1.0f);
//			glVertex2f(B3[i][0], B3[i][1]);		/* By now the z component no longer exists */ 
//		glEnd();
//	}

	for ( i=0; i<count-1; i++)
	{
		drawline(B3[i][0], B3[i][1], B3[i+1][0], B3[i+1][1]);
	}

	drawline(bez_points[0][0], bez_points[0][1], bez_points[1][0], bez_points[1][1]);
	drawline(bez_points[1][0], bez_points[1][1], bez_points[2][0], bez_points[2][1]);
	drawline(bez_points[2][0], bez_points[2][1], bez_points[3][0], bez_points[3][1]);
	drawline(bez_points[3][0], bez_points[3][1], bez_points[4][0], bez_points[4][1]);

	glPointSize(10);

	glBegin(GL_POINTS);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(bez_points[0][0], bez_points[0][1]);
		glVertex2f(bez_points[1][0], bez_points[1][1]);
		glVertex2f(bez_points[2][0], bez_points[2][1]);
		glVertex2f(bez_points[3][0], bez_points[3][1]);
		glVertex2f(bez_points[4][0], bez_points[4][1]);
	glEnd();

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function Name:
// --------------
//
// Created On: 3/28/02
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void draw_bezier_sphere (void)
{
	Matrix3D *bez_rotate;							/* Create the bez_rotate matrix */
	int row;							/* Used for looping purposes */
	float rotate_by;
	int bezCount;					/* Used for looping */
	float bezTemp[4];

	bez_rotate = (Matrix3D *) malloc(sizeof(Matrix3D));	/* Initialise the bez_rotate matrix */

	for (rotate_by = 30; rotate_by<370; rotate_by += 30)
	{
		bez_rotate->c[0][0] = cos(rotate_by * DEG2RAD);						/* Set the bez_rotate matrix with the necessary values */
		bez_rotate->c[0][1] = 0;
		bez_rotate->c[0][2] = -sin(rotate_by * DEG2RAD);
		bez_rotate->c[0][3] = 0;

		bez_rotate->c[1][0] = 0;
		bez_rotate->c[1][1] = 1;
		bez_rotate->c[1][2] = 0;
		bez_rotate->c[1][3] = 0;

		bez_rotate->c[2][0] = sin(rotate_by * DEG2RAD);
		bez_rotate->c[2][1] = 0;
		bez_rotate->c[2][2] = cos(rotate_by * DEG2RAD);
		bez_rotate->c[2][3] = 0;

		bez_rotate->c[3][0] = 0;
		bez_rotate->c[3][1] = 0;
		bez_rotate->c[3][2] = 0;
		bez_rotate->c[3][3] = 1;

		for(bezCount=0; bezCount < NUMBEZCP; bezCount++)	/* Do the calculations */
		{
			bezTemp[0] = bez_points[bezCount][0];
			bezTemp[1] = bez_points[bezCount][1];
			bezTemp[2] = bez_points[bezCount][2];
			bezTemp[3] = bez_points[bezCount][3];

			for(row=0; row<4; row++)
			{
			bez_points[bezCount][row] = (bezTemp[0] * bez_rotate->c[0][row]) + (bezTemp[1] * bez_rotate->c[1][row]) + (bezTemp[2] * bez_rotate->c[2][row]) + (bezTemp[3] * bez_rotate->c[3][row]);
			}	
		}

		bezier();
	}

	free(bez_rotate);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function Name:
// --------------
//
// Created On: 3/27/02
//
////////////////////////////////////////////////////////////////////////////////////////////////////

int back_face_culling (int n)
{
	float vectorA[3];
	float modulusA, modulusB, dotprod, angle;

	vectorA[0] = normals[n][0];
	vectorA[1] = normals[n][1];
	vectorA[2] = normals[n][2];

	modulusA = sqrt( (vectorA[0] * vectorA[0]) + (vectorA[1] * vectorA[1]) + (vectorA[2] * vectorA[2]) );
	modulusB = sqrt( (cam_pos[0] * cam_pos[0]) + (cam_pos[1] * cam_pos[1]) + (cam_pos[2] * cam_pos[2]) );

	dotprod = (vectorA[0] * cam_pos[0]) + (vectorA[1] * cam_pos[1]) + (vectorA[2] * cam_pos[2]);

	angle = dotprod / (modulusA * modulusB);
	angle = cos( angle * DEG2RAD);

	if ( angle < 90 )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}	

/********************************************************************************************

 drawline
 --------

 Function which uses linear interpolation to draw points to form a line.

********************************************************************************************/

void drawline(float xa, float ya, float xb, float yb)
{
	float x, y, lin_interpolant;

	glLineWidth(1.);
	glColor3f(1.0, 1.0, 1.0);

	for (lin_interpolant=0; lin_interpolant<1; lin_interpolant=lin_interpolant + 0.0005)
	{
		x = ((1-lin_interpolant) * xa) + (lin_interpolant * xb);
		y = ((1-lin_interpolant) * ya) + (lin_interpolant * yb);	

		glPointSize(1);
		glBegin(GL_POINTS);
			glVertex2f(x, y);
		glEnd();
	}

	return;	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function Name:	drawpoly
// --------------dd
//
// Created On: 3/22/02
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void drawpoly(float xa, float ya, float xb, float yb, float xc, float yc, int normal) /* Pass in the three co-ords of the triangle */
{
	float u, v, pt1x, pt1y, pt2x, pt2y, polyPointx, polyPointy;
	PolyPoints points;
//	int i;

	v = 0;

	pt1x = 0;
	pt1y = 0;

	pt2x = 0;
	pt2y = 0;

	points.x[0] = xa;
	points.x[1] = xb;
	points.x[2] = xc;

	points.y[0] = ya;
	points.y[1] = yb;
	points.y[2] = yc;

	points = bubbleSort(points); // returns the above list sorted into order, largest y first

//	for ( i=0; i<=SIZE - 1; i++)  // check to make sure that the sort has worked
//	{
//		printf("\n%.2f", points.y[i]);
//	}
//
//	printf("\n ");

//	colour = flatShade(normal);	
//	glBegin(GL_POLYGON);
//		glVertex2f(xa, ya);
//		glVertex2f(xb, yb);
//		glVertex2f(xc, yc);
//	glEnd();

	for (u=0; u<1; u=u + 0.5)
	{
		pt1x = ((1-u) * points.x[2]) + (u * points.x[0]);  // pt1x = ((1-u) * xb) + (u * xa);
		pt1y = ((1-u) * points.y[2]) + (u * points.y[0]);  //	pt1y = ((1-u) * yb) + (u * ya); makes curves
			
		pt2x = ((1-u) * points.x[2]) + (u * points.x[1]);  
		pt2y = ((1-u) * points.y[2]) + (u * points.y[1]);

		for (v=0; v<1; v=v + 0.005)	
		{
			polyPointx = ((1-v) * pt2x) + (v * pt1x);
			polyPointy = ((1-v) * pt2y) + (v * pt1y);

			glPointSize(1);

			glColor3f(0.0, 0.0, 1.0);

			glBegin(GL_POINTS);
				glPointSize(1);
				glVertex2f(polyPointx, polyPointy);
			glEnd();
		}
	}
	
	return;	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function Name:
// --------------
//
// Created On: 3/26/02
//
////////////////////////////////////////////////////////////////////////////////////////////////////

PolyPoints bubbleSort ( PolyPoints list )
{
	int i, passNo, temp, tempx;

	for ( passNo=0; passNo<= SIZE - 1; passNo++ ) 
	{	
		for ( i=0; i<= SIZE; i++)
		{
			if ( list.y[i] > list.y[i + 1] )
			{
				temp = list.y[i];
				list.y[i] = list.y[i + 1];
				list.y[i + 1] = temp;

				tempx = list.x[i];			//remember to switch the x's round as well to maintain consistency
				list.x[i] = list.x[i + 1];
				list.x[i + 1] = tempx;
			}
		}
	}

//	for ( i=0; i<=SIZE - 1; i++)
//	{
//		printf("\n%.2f", list.y[i]);
//	}
//
//	printf("\n ");

	return list;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function Name:
// --------------
//
// Created On: 3/25/02
//
////////////////////////////////////////////////////////////////////////////////////////////////////

float flatShade(int n)
{
	float modulusA, modulusB, dotprod, temp;
	float vectorA[3];

	vectorA[0] = normals[n][0];
	vectorA[1] = normals[n][1];
	vectorA[2] = normals[n][2];

	modulusA = sqrt( (vectorA[0] * vectorA[0]) + (vectorA[1] * vectorA[1]) + (vectorA[2] * vectorA[2]) );
	modulusB = sqrt( (light[0] * light[0]) + (light[1] * light[1]) + (light[2] * light[2]) );

	dotprod = (vectorA[0] * light[0]) + (vectorA[1] * light[1]) + (vectorA[2] * light[2]);

	temp = dotprod / (modulusA * modulusB);

	return temp;
}

/********************************************************************************************

 keyboard
 --------

 Function to accept keyboard input. This is specified as a GL call back function, so it is called
 when there is imput from then keyboard. If the input matches any of those below, then appropriate
 actions are taken. The function takes the contents of key as a parameter and also takes the 
 current x and y co-ordinates.

********************************************************************************************/

void keyboard(unsigned char key, int x, int y)
{
	switch (key)							/* Key is the variable to switch between */
	{
		case 'x':	cam_pos[0] = cam_pos[0] + 0.5;	/* Zoom out if x is pressed */

					break;

		case 'z':	cam_pos[0] = cam_pos[0] - 0.5;	/* Zoom in if z is pressed */

					break;

		case 's':	translate[2] = translate[2] + 0.5;	/* Zoom in if z is pressed */

					break;

		case 'w':	translate[2] = translate[2] - 0.5;	/* Zoom in if z is pressed */

					break;

		case 'a':	translate[0] = translate[0] + 0.5;	/* Zoom in if z is pressed */

					break;

		case 'd':	translate[0] = translate[0] - 0.5;	/* Zoom in if z is pressed */

					break;

		case 'y':	bez_points[0][0] = bez_points[0][0] + 1;	/* Zoom out if x is pressed */

					break;

		case 'u':	bez_points[0][0] = bez_points[0][0] - 1;	/* Zoom in if z is pressed */

					break;

		case 'h':	bez_points[1][1] = bez_points[1][1] + 1;	/* Zoom out if x is pressed */

					break;

		case 'j':	bez_points[1][1] = bez_points[1][1] - 1;	/* Zoom in if z is pressed */

					break;
		
		default:    break;
	}           
    
	glutPostRedisplay();					/* Redraw the scene immediately */
}

/********************************************************************************************

 special
 -------

 Function to accept special keyboard input. This includes the F* buttons, and the left/right
 up/down buttons. This helps to increase the range of buttons that can be used for controlling
 the scene. It takes the same parameters as keyboard, but is for special characters only, else 
 keyboard will be called.

********************************************************************************************/

void special(int key, int x, int y)
{
	switch(key)								/* Key is the variable to switch between */
	{
		case GLUT_KEY_LEFT:	cam_pos[1] = cam_pos[1] + 5;	/* If up is selected, pan the view up */
							
							break;

		case GLUT_KEY_RIGHT:	cam_pos[1] = cam_pos[1] - 5;	/* If down is selected, pan the view down */
							
							break;

		case GLUT_KEY_UP:	cam_pos[2] = cam_pos[2] + 5;	/* If left is selected, rotate the view left */
							
							break;

		case GLUT_KEY_DOWN: cam_pos[2] = cam_pos[2] - 5;	/* If right is selected, rotate the view right */
							
							break;

		case GLUT_KEY_F1:	printMenu();				/* If F1 is pressed, reprint the main menu */

							break;
	
		default:	break;								/* Again, the default value to break out */
	}

	glutPostRedisplay();								/* Redraw the scene immediately */
}

/********************************************************************************************

 mainMenuFunc
 ------------

 Function to draw a floating menu on the screen that the user specifies by left clicking at the
 desired location for the menu. The options allow the user to reset the view or exit the system
 entirely.

********************************************************************************************/

void mainMenuFunc(int item)
{
	switch (item) 
	{
		  case 0:	readInitialValues();	/* Reset the viewing system */

					break;

		  case 1:	exit(EXIT_SUCCESS);		/* Exit the program */

					break;

		  default:	break;					/* Again, the default value to break out */
   }

   glutPostRedisplay();						/* Redraw the scene immediately */
}

/********************************************************************************************

 mouse
 -----

 Function to record the position of the mouse when a button is pressed. This is needed for mouse
 control over the scene as the system needs to know the current and old mouse positions. A larger
 difference will mean the user is moving the mouse faster, therefore wishing the scene to be 
 rotated and updated faster.

********************************************************************************************/

void mouse(int button, int state, int x, int y)
{ 
	old_mouse[0] = x;						/* when a mouse button is pressed, record the position of the mouse */
	old_mouse[1] = y;

	glutPostRedisplay();					/* Redraw the scene immediately */
}

/********************************************************************************************

 motion
 ------

 When the mouse is moved with a button pressed, record the difference between the previous and the 
 current positions. this is used to map an angle of rotation by which the object is going to be rotated:

 - rotation along X axis, by moving mouse vertically
 - rotation along Y axis, by moving mouse horizontally

********************************************************************************************/

void motion(int x, int y)
{
	cam_pos[1] += (old_mouse[2] - y) * .5f;
	cam_pos[2] += (old_mouse[1] - x) * .5f;
 
	old_mouse[1] = x;						/* Reinitialise the old mouse position to the current position */
	old_mouse[2] = y;

	glutPostRedisplay();					/* Redraw the scene immediately */
}

/********************************************************************************************

 alloc_mem_2Darray_float
 -----------------------

 Function which dynamically allocates memory as needed to the system. There are two versions of 
 this function, one which allocates floats, for vertices to be stored. The other stores ints
 for the polygons. 

 NOTE: If this project was being written in C++, function overloading could be used to return
 the different types.

********************************************************************************************/

float **alloc_mem_2Darray_float(int row, int col) 
{ 
	int i;									/* Variable used for looping */
	float **p;								/* Pointer to the soon-to-be-created memory block */

	p = (float **)malloc(sizeof(float *)*row);	/* Allocates the necessary memory for type float */

	for (i = 0; i < row; i++)				/* For eaxh of the new rows, create cols of floats as needed */
	{
	   *(p + i) = (float *)malloc(sizeof(float)*col); 
	}

	return p;								/* Return the pointer to the new memory chunk */
} 

/********************************************************************************************

 alloc_mem_2Darray_int
 -----------------------

 Integer version of the above function.

********************************************************************************************/

int **alloc_mem_2Darray_int(int row, int col) 
{ 
	int i;									/* Variable used for looping */
	int **p;								/* Pointer to the soon-to-be-created memory block */

	p = (int **)malloc(sizeof(int *)*row);	/* Allocates the necessary memory for type int */

	for (i = 0; i < row; i++)				/* For eaxh of the new rows, create cols of floats as needed */
	{
	   *(p + i) = (int *)malloc(sizeof(int)*col); 
	}

	return p;								/* Return the pointer to the new memory chunk */
} 

/********************************************************************************************

 matrix_zoom
 -----------

 Function to create and manipulate a matrix used for calculating a zoom value for every vertex
 in the scene. This function calls another function matrix_calc, which takes a matrix as a 
 parameter and multiplies it with every vector in the scene. 

********************************************************************************************/

void matrix_t1(void)
{
	Matrix3D *t1;							/* Create the t1 matrix */
	int row, col;							/* Used for looping purposes */

	t1 = (Matrix3D *) malloc(sizeof(Matrix3D));	/* Initialise the t1 matrix */

//	printf("\n");							/* Print a bit of space in the command window */
//	printf("t1 MATRIX\n");

	t1->c[0][0] = 1;						/* Set the t1 matrix with the necessary values */
	t1->c[0][1] = 0;
	t1->c[0][2] = 0;
	t1->c[0][3] = 0;

	t1->c[1][0] = 0;
	t1->c[1][1] = 1;
	t1->c[1][2] = 0;
	t1->c[1][3] = 0;

	t1->c[2][0] = 0;
	t1->c[2][1] = 0;
	t1->c[2][2] = 1;
	t1->c[2][3] = 0;

	t1->c[3][0] = -(cam_pos[0]) * (sin(cam_pos[2] * DEG2RAD)) * (cos(cam_pos[1] * DEG2RAD));
	t1->c[3][1] = -(cam_pos[0]) * (sin(cam_pos[2] * DEG2RAD)) * (sin(cam_pos[1] * DEG2RAD));
	t1->c[3][2] = -(cam_pos[0]) * (cos(cam_pos[2] * DEG2RAD));
	t1->c[3][3] = 1;

	for(row=0; row<4; row++)				/* Print the t1 matrix. This aids development */
	{
		for(col=0; col<4; col++)
		{
//			printf("%.2f\t", t1->c[row][col]);
		}
		
//		printf("\n");
	}

	matrix_calc(VIEW, t1);				/* Perform the necessary calculations to the vertices. */

	free(t1);								/* Free up the memory that was allocated to this matrix */

	return;
}

/********************************************************************************************/

void matrix_t2(void)
{
	Matrix3D *t2;							/* Create the t2 matrix */
	int row, col;							/* Used for looping purposes */

	t2 = (Matrix3D *) malloc(sizeof(Matrix3D));	/* Initialise the t2 matrix */

//	printf("\n");							/* Print a bit of space in the command window */
//	printf("t2 MATRIX\n");

	t2->c[0][0] = sin(cam_pos[1] * DEG2RAD);						/* Set the t2 matrix with the necessary values */
	t2->c[0][1] = cos(cam_pos[1] * DEG2RAD);
	t2->c[0][2] = 0;
	t2->c[0][3] = 0;

	t2->c[1][0] = -cos(cam_pos[1] * DEG2RAD);
	t2->c[1][1] = sin(cam_pos[1] * DEG2RAD);
	t2->c[1][2] = 0;
	t2->c[1][3] = 0;

	t2->c[2][0] = 0;
	t2->c[2][1] = 0;
	t2->c[2][2] = 1;
	t2->c[2][3] = 0;

	t2->c[3][0] = 0;
	t2->c[3][1] = 0;
	t2->c[3][2] = 0;
	t2->c[3][3] = 1;

	for(row=0; row<4; row++)				/* Print the t2 matrix. This aids development */
	{
		for(col=0; col<4; col++)
		{
//			printf("%.2f\t", t2->c[row][col]);
		}
		
//		printf("\n");
	}

	matrix_calc(VIEW, t2);				/* Perform the necessary calculations to the vertices. */

	free(t2);								/* Free up the memory that was allocated to this matrix */

	return;
}

/********************************************************************************************/

void matrix_t3(void)
{
	Matrix3D *t3;							/* Create the t3 matrix */
	int row, col;							/* Used for looping purposes */

	t3 = (Matrix3D *) malloc(sizeof(Matrix3D));	/* Initialise the t3 matrix */

//	printf("\n");							/* Print a bit of space in the command window */
//	printf("t3 MATRIX\n");

	t3->c[0][0] = 1;						/* Set the t3 matrix with the necessary values */
	t3->c[0][1] = 0;
	t3->c[0][2] = 0;
	t3->c[0][3] = 0;

	t3->c[1][0] = 0;
	t3->c[1][1] = -cos(cam_pos[2] * DEG2RAD);
	t3->c[1][2] = -sin(cam_pos[2] * DEG2RAD);
	t3->c[1][3] = 0;

	t3->c[2][0] = 0;
	t3->c[2][1] = sin(cam_pos[2] * DEG2RAD);
	t3->c[2][2] = -cos(cam_pos[2] * DEG2RAD);
	t3->c[2][3] = 0;

	t3->c[3][0] = 0;
	t3->c[3][1] = 0;
	t3->c[3][2] = 0;
	t3->c[3][3] = 1;

	for(row=0; row<4; row++)				/* Print the t3 matrix. This aids development */
	{
		for(col=0; col<4; col++)
		{
//			printf("%.2f\t", t3->c[row][col]);
		}
		
//		printf("\n");
	}

	matrix_calc(VIEW, t3);				/* Perform the necessary calculations to the vertices. */

	free(t3);								/* Free up the memory that was allocated to this matrix */

	return;
}

/********************************************************************************************/

void matrix_t4(void)
{
	Matrix3D *t4;							/* Create the t4 matrix */
	int row, col;							/* Used for looping purposes */

	t4 = (Matrix3D *) malloc(sizeof(Matrix3D));	/* Initialise the t4 matrix */

//	printf("\n");							/* Print a bit of space in the command window */
//	printf("t4 MATRIX\n");

	t4->c[0][0] = -1;						/* Set the t4 matrix with the necessary values */
	t4->c[0][1] = 0;
	t4->c[0][2] = 0;
	t4->c[0][3] = 0;

	t4->c[1][0] = 0;
	t4->c[1][1] = 1;
	t4->c[1][2] = 0;
	t4->c[1][3] = 0;

	t4->c[2][0] = 0;
	t4->c[2][1] = 0;
	t4->c[2][2] = 1;
	t4->c[2][3] = 0;

	t4->c[3][0] = 0;
	t4->c[3][1] = 0;
	t4->c[3][2] = 0;
	t4->c[3][3] = 1;

	for(row=0; row<4; row++)				/* Print the t4 matrix. This aids development */
	{
		for(col=0; col<4; col++)
		{
//			printf("%.2f\t", t4->c[row][col]);
		}
		
//		printf("\n");
	}

	matrix_calc(VIEW, t4);				/* Perform the necessary calculations to the vertices. */

	free(t4);								/* Free up the memory that was allocated to this matrix */

	return;
}

/********************************************************************************************/

void matrix_tpers(void)
{
	Matrix3D *tpers;							/* Create the tpers matrix */
	int row, col;							/* Used for looping purposes */

	tpers = (Matrix3D *) malloc(sizeof(Matrix3D));	/* Initialise the tpers matrix */

//	printf("\n");							/* Print a bit of space in the command window */
//	printf("tpers MATRIX\n");

	tpers->c[0][0] = 1;						/* Set the tpers matrix with the necessary values */
	tpers->c[0][1] = 0;
	tpers->c[0][2] = 0;
	tpers->c[0][3] = 0;

	tpers->c[1][0] = 0;
	tpers->c[1][1] = 1;
	tpers->c[1][2] = 0;
	tpers->c[1][3] = 0;

	tpers->c[2][0] = 0;
	tpers->c[2][1] = 0;
	tpers->c[2][2] = 0;
	tpers->c[2][3] = 1/d;

	tpers->c[3][0] = 0;
	tpers->c[3][1] = 0;
	tpers->c[3][2] = 0;
	tpers->c[3][3] = 1;

	for(row=0; row<4; row++)				/* Print the tpers matrix. This aids development */
	{
		for(col=0; col<4; col++)
		{
//			printf("%.2f\t", tpers->c[row][col]);
		}
		
//		printf("\n");
	}

	matrix_calc(PERSP, tpers);				/* Perform the necessary calculations to the vertices. */

	free(tpers);								/* Free up the memory that was allocated to this matrix */

	return;
}

/********************************************************************************************/

void matrix_translate (void)
{
	Matrix3D *tpers;							/* Create the tpers matrix */
	int row, col;							/* Used for looping purposes */

	tpers = (Matrix3D *) malloc(sizeof(Matrix3D));	/* Initialise the tpers matrix */

//	printf("\n");							/* Print a bit of space in the command window */
//	printf("tpers MATRIX\n");

	tpers->c[0][0] = 1;						/* Set the tpers matrix with the necessary values */
	tpers->c[0][1] = 0;
	tpers->c[0][2] = 0;
	tpers->c[0][3] = 0;

	tpers->c[1][0] = 0;
	tpers->c[1][1] = 1;
	tpers->c[1][2] = 0;
	tpers->c[1][3] = 0;

	tpers->c[2][0] = 0;
	tpers->c[2][1] = 0;
	tpers->c[2][2] = 1;
	tpers->c[2][3] = 0;

	tpers->c[3][0] = translate[0];
	tpers->c[3][1] = translate[1];
	tpers->c[3][2] = translate[2];
	tpers->c[3][3] = 1;

	for(row=0; row<4; row++)				/* Print the tpers matrix. This aids development */
	{
		for(col=0; col<4; col++)
		{
//			printf("%.2f\t", tpers->c[row][col]);
		}
		
//		printf("\n");
	}

	matrix_calc(VIEW, tpers);				/* Perform the necessary calculations to the vertices. */

	free(tpers);								/* Free up the memory that was allocated to this matrix */

	return;
}

/********************************************************************************************

 matrix_calc
 -----------

 Function to do the vector/matrix multiplication. This is where most of the processing goes on, as
 every single vertex is multiplied against every matrix! The function takes two parameters - transformations
 is an enumerated type which will switch between the necessary calculations to be performed. The function
 also takes a pointer to each matrix passed to it.

********************************************************************************************/

void matrix_calc(enum Transformations TRANS, Matrix3D *m)
{
	int verCount, row, normalCount, bezCount;					/* Used for looping */
	float temp[4], bezTemp[4], normalTemp[3];
	
	switch(TRANS)							/* Enumerated type to switch between */
	{
		case VIEW:							/* If any of these, do the following */

		for(verCount=0; verCount < nVertices; verCount++)	/* Do the calculations */
		{
			temp[0] = vertex[verCount][0];
			temp[1] = vertex[verCount][1];
			temp[2] = vertex[verCount][2];
			temp[3] = vertex[verCount][3];

			for(row=0; row<4; row++)
			{
				vertex[verCount][row] = (temp[0] * m->c[0][row]) + (temp[1] * m->c[1][row]) + (temp[2] * m->c[2][row]) + (temp[3] * m->c[3][row]);
			}	
		}

		for(normalCount=0; normalCount < nFaces; normalCount++)	
		{
			normalTemp[0] = normals[normalCount][0];
			normalTemp[1] = normals[normalCount][1];
			normalTemp[2] = normals[normalCount][2];
		
			for(row=0; row<4; row++)
			{
				normals[normalCount][row] = (normalTemp[0] * m->c[0][row]) + (normalTemp[1] * m->c[1][row]) + (normalTemp[2] * m->c[2][row]);
			}
		}

		for(bezCount=0; bezCount < NUMBEZCP; bezCount++)	/* Do the calculations */
		{
			bezTemp[0] = bez_points[bezCount][0];
			bezTemp[1] = bez_points[bezCount][1];
			bezTemp[2] = bez_points[bezCount][2];
			bezTemp[3] = bez_points[bezCount][3];

			for(row=0; row<4; row++)
			{
				bez_points[bezCount][row] = (bezTemp[0] * m->c[0][row]) + (bezTemp[1] * m->c[1][row]) + (bezTemp[2] * m->c[2][row]) + (bezTemp[3] * m->c[3][row]);
			}	
		}

		break;
		
		case PERSP:							/* If perspective, do this, as well as convert homogeneous vectors */

		for(verCount=0; verCount < nVertices; verCount++)	/* Do the calculations */
		{
			temp[0] = vertex[verCount][0];
			temp[1] = vertex[verCount][1];
			temp[2] = vertex[verCount][2];
			temp[3] = vertex[verCount][3];

			for(row=0; row<4; row++)
			{
				vertex[verCount][row] = (temp[0] * m->c[0][row]) + (temp[1] * m->c[1][row]) + (temp[2] * m->c[2][row]) + (temp[3] * m->c[3][row]);
			}

			vertex[verCount][0] = vertex[verCount][0] / vertex[verCount][3];	/* Perform the perspective divide */
			vertex[verCount][1] = vertex[verCount][1] / vertex[verCount][3];
			vertex[verCount][2] = vertex[verCount][2] / vertex[verCount][3];
			vertex[verCount][3] = vertex[verCount][3] / vertex[verCount][3];

			temp[0] = 0;
			temp[1] = 0;
			temp[2] = 0;
			temp[3] = 0;
		}

		for(bezCount=0; bezCount < NUMBEZCP; bezCount++)	/* Do the calculations */
		{
			bezTemp[0] = bez_points[bezCount][0];
			bezTemp[1] = bez_points[bezCount][1];
			bezTemp[2] = bez_points[bezCount][2];
			bezTemp[3] = bez_points[bezCount][3];

			for(row=0; row<4; row++)
			{
				bez_points[bezCount][row] = (bezTemp[0] * m->c[0][row]) + (bezTemp[1] * m->c[1][row]) + (bezTemp[2] * m->c[2][row]) + (bezTemp[3] * m->c[3][row]);
			}	

			bez_points[bezCount][0] = bez_points[bezCount][0] / bez_points[bezCount][3];	/* Perform the perspective divide */
			bez_points[bezCount][1] = bez_points[bezCount][1] / bez_points[bezCount][3];
			bez_points[bezCount][2] = bez_points[bezCount][2] / bez_points[bezCount][3];
			bez_points[bezCount][3] = bez_points[bezCount][3] / bez_points[bezCount][3];
		}

		break;

		default: break;
	}

	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function Name:
// --------------
//
// Created On: 3/25/02
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void calcNormals (int pointA, int pointB, int normalCount)
{
	normals[normalCount][0] = (vertex[pointA][1] * vertex[pointB][2]) - (vertex[pointB][1] * vertex[pointA][2]);
	normals[normalCount][1] = -(vertex[pointA][0] * vertex[pointB][2]) - (vertex[pointB][0] * vertex[pointA][2]);
	normals[normalCount][2] = (vertex[pointA][0] * vertex[pointB][1]) - (vertex[pointB][0] * vertex[pointA][1]);
}	

/********************************************************************************************

 reshape
 -------

 Function to do the vector/matrix multiplication. This is where most of the processing goes on, as
 every single vertex is multiplied against every matrix! The function takes two parameters - transformations
 is an enumerated type which will switch between the necessary calculations to be performed. The function
 also takes a pointer to each matrix passed to it.

********************************************************************************************/
/*
void reshape(int width, int height)
{
	static float xRatio = SCALE / ((float)NPIXELS);
	static float yRatio = SCALE / ((float)NPIXELS);

	float xScale = ((float) width) * xRatio;
	float yScale = ((float) height) * yRatio;

	glViewport(0, 0, width, height);

	gluOrtho2D(-SCALE, SCALE, -SCALE, SCALE);
}
*/

/********************************************************************************************/
/*********************** END OF MAIN PROGRAM ************************************************/
/********************************************************************************************/
