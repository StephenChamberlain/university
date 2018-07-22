///////////////////////////////////////////////////////////////////
//
//		INCLUDES
//
///////////////////////////////////////////////////////////////////
#pragma comment(lib, "jpeg.lib")

#include <gl\glut.h>		// Glut 		
#include <gl\gl.h>			// OpenGl 
#include <iostream>			// Input, output 
#include <fstream>			// File management 
#include <windows.h>		// Windows. Some windows functions are used
#include <time.h>			// Used for timing
#include <math.h>			// Used for cos, sin etc

#include "../header/OpenGL.h"
#include "../header/Useful.h"
#include "../header/Frustum.h"
#include "../header/Vector3.h"
#include "../header/Ball.h"
#include "../header/jpeglib.h"

///////////////////////////////////////////////////////////////////
//
//		STANDARD NAMESPACE
//
///////////////////////////////////////////////////////////////////

using namespace std;		// Standard namespace

///////////////////////////////////////////////////////////////////
//
//		DEFINES
//
///////////////////////////////////////////////////////////////////

#define	NUM_BALLS		15
#define	MAX_TEXTURES	10

///////////////////////////////////////////////////////////////////
//
//		GLOBALS
//
///////////////////////////////////////////////////////////////////

int				xPixels;
int				yPixels;
int				gridSize;
int				oldMouseX;
int				oldMouseY;

float			xRotation;
float			yRotation;
float			lightPosition[4] = {0, 10, 0, 10};

char			mainTexturePath[128];

unsigned char*	imageMap;

BYTE			heightMap[1024 * 1024];
UINT			texture[MAX_TEXTURES];

COpenGL			OpenGL;
CUseful			Useful;
CFrustum		Frustum;
CVector3		Camera;
CBall			Balls[NUM_BALLS];

///////////////////////////////////////////////////////////////////
//
//		PROTOTYPES
//
///////////////////////////////////////////////////////////////////

void display (void);
void idle (void);
void loadConfig(void);
void reshape(int, int);
void keyboard (unsigned char, int, int);
void mouse (int, int, int, int);	// Mouse callback function
void motion (int, int);				// Motion callback function
void init(void);
void loadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap);
void createTexture(UINT textureArray[], LPSTR strFileName, int textureID);
void decodeJPG(jpeg_decompress_struct* cinfo, tImageJPG *pImageData);
tImageJPG *loadJPG(const char *filename);

///////////////////////////////////////////////////////////////////
//
//		FUNCTIONS
//
///////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	loadConfig();

	glutInit(&argc, argv);
	glutInitWindowSize(xPixels, yPixels);
	glutInitWindowPosition(0,0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Stephen Chamberlain: Bouncing Balls");

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);				// Register mouse activity
	glutMotionFunc(motion);				// Register mouse motion
	glutIdleFunc(idle);

	glutMainLoop();

	return 0;             
}

///////////////////////////////////////////////////////////////////

void init(void)
{
	int i;
	float ambience[4] = {0.3f, 0.3f, 0.3f, 1.0};		// The color of the light in the world
	float diffuse[4] = {0.5f, 0.5f, 0.5f, 1.0};
	time_t simTime;

	simTime = time (NULL);

	srand(simTime);

	for (i=0; i<NUM_BALLS; i++) 
	{
		Balls[i].x = rand()%20 - 10;
		Balls[i].y = rand()%20 - 10;
		Balls[i].z = rand()%20 - 10;
		Balls[i].size = rand()%3;
		Balls[i].velocityX = rand()%3;
		Balls[i].velocityY = rand()%3;
		Balls[i].velocityZ = rand()%3;
		Balls[i].red = rand()%255;
		Balls[i].green = rand()%255;
		Balls[i].blue = rand()%255;

		if (Balls[i].size == 0)
		{
			Balls[i].size = 1;
		}

		if (Balls[i].velocityY == 0)
		{
			Balls[i].velocityY = 1;
		}
	}

	OpenGL.GLSetup();
	OpenGL.InitPerspective();

	glEnable(GL_TEXTURE_2D);
	createTexture(texture, "Data/Textures/floor.jpg", 0);
	createTexture(texture, "Data/Textures/wall.jpg", 1);
	createTexture(texture, "Data/Textures/ceiling.jpg", 2);

	glClearColor(0, 0, 0, 1);
	glLightfv( GL_LIGHT0, GL_AMBIENT,  ambience );		// Set our ambience values (Default color without direct light)
	glLightfv( GL_LIGHT0, GL_DIFFUSE,  diffuse );
	glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );	// This Sets our light position
	glEnable(  GL_LIGHT0   );
	glEnable(  GL_LIGHTING );
	glEnable(GL_COLOR_MATERIAL);

	Frustum.CalculateFrustum();
}

///////////////////////////////////////////////////////////////////

void reshape(int width, int height)
{
	OpenGL.Reshape(width, height);
}

///////////////////////////////////////////////////////////////////

void display(void)
{
	int i;

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Frustum.CalculateFrustum();

	OpenGL.PerspectiveViewingSystem(Camera.x, Camera.y, Camera.z, xRotation, yRotation);

	glDisable(GL_TEXTURE_2D);

	for (i=0; i<NUM_BALLS; i++) 
	{
		if (Frustum.SphereInFrustum(Balls[i].x, Balls[i].y, Balls[i].z, Balls[i].size))
		{
			glColor3b(Balls[i].red, Balls[i].green, Balls[i].blue);
			OpenGL.Sphere(Balls[i].x, Balls[i].y, Balls[i].z, Balls[i].size);
		}
	}

	glColor3f(1.0, 0.0, 0.0);
	OpenGL.Line(10,0,0, -10,0,0);

	glColor3f(0.0, 1.0, 0.0);
	OpenGL.Line(0,10,0, 0,-10,0);

	glColor3f(0.0, 0.0, 1.0);
	OpenGL.Line(0,0,10, 0,0,-10);

	glEnable(GL_TEXTURE_2D);
	/// DRAW THE FLOOR

	glBindTexture(GL_TEXTURE_2D, texture[0]);

	// Display a quad texture to the screen
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-100, -30, 100);

		glTexCoord2f(0.0f, 10.0f);
		glVertex3f(-100, -30, -100);

		glTexCoord2f(10.0f, 10.0f);
		glVertex3f(100, -30, -100);

		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(100, -30, 100);
	glEnd();

	/// DRAW THE WALLS

	glBindTexture(GL_TEXTURE_2D, texture[1]);

	// Display a quad texture to the screen
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-100, -30, -100);

		glTexCoord2f(0.0f, 5.0f);
		glVertex3f(-100, 100, -100);

		glTexCoord2f(10.0f, 5.0f);
		glVertex3f(100, 100, -100);

		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(100, -30, -100);
	glEnd();

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-100, -30, 100);

		glTexCoord2f(0.0f, 5.0f);
		glVertex3f(-100, 100, 100);

		glTexCoord2f(10.0f, 5.0f);
		glVertex3f(100, 100, 100);

		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(100, -30, 100);
	glEnd();

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-100, -30, -100);

		glTexCoord2f(0.0f, 5.0f);
		glVertex3f(-100, 100, -100);

		glTexCoord2f(10.0f, 5.0f);
		glVertex3f(-100, 100, 100);

		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(-100, -30, 100);
	glEnd();

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(100, -30, -100);

		glTexCoord2f(0.0f, 5.0f);
		glVertex3f(100, 100, -100);

		glTexCoord2f(10.0f, 5.0f);
		glVertex3f(100, 100, 100);

		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(100, -30, 100);
	glEnd();

	/// DRAW THE CEILING

	glBindTexture(GL_TEXTURE_2D, texture[2]);

	// Display a quad texture to the screen
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-100, 100, 100);

		glTexCoord2f(0.0f, 10.0f);
		glVertex3f(-100, 100, -100);

		glTexCoord2f(10.0f, 10.0f);
		glVertex3f(100, 100, -100);

		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(100, 100, 100);
	glEnd();

	glutSwapBuffers();
}

///////////////////////////////////////////////////////////////////

void idle (void)
{
//	float fps;
	int i;

//	Useful.CalculateFrameRate();
//	fps = Useful.GetFramesPerSecond();
	yRotation += 0.25;

	for (i=0; i<NUM_BALLS; i++) 
	{
		Balls[i].x = Balls[i].x - Balls[i].velocityX;

		if (Balls[i].x <= (-100 + Balls[i].size))
		{
			Balls[i].velocityX = rand()%3 * -1;
		}

		else if (Balls[i].x >= (100 - Balls[i].size))
		{
			Balls[i].velocityX = rand()%3;
		}
	
		Balls[i].y = Balls[i].y - Balls[i].velocityY;

		if (Balls[i].y <= (-30 + Balls[i].size))
		{
			Balls[i].velocityY = rand()%3 * -1;
		}		
		
		else if (Balls[i].y >= (100 - Balls[i].size))
		{
			Balls[i].velocityY = rand()%3;
		}

		Balls[i].z = Balls[i].z - Balls[i].velocityZ;

		if (Balls[i].z <= (-100 + Balls[i].size))
		{
			Balls[i].velocityZ = rand()%3 * -1;
		}

		else if (Balls[i].z >= (100 - Balls[i].size))
		{
			Balls[i].velocityZ = rand()%3;
		}
	}

	glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////

void keyboard (unsigned char key, int x, int y)
{
	switch (key)							// Key is the variable to switch between. Given to us by GL
	{
		default:    break;
	}           
    
	glutPostRedisplay();	
}

///////////////////////////////////////////////////////////////////

void motion(int x, int y)
{
	xRotation += (oldMouseY - y) / 2.0;	// Effect the orbitting angles for the camera. 
	yRotation += (oldMouseX - x) / 2.0;

	oldMouseX = x;					// Set the old coordinates ready for use next time.
	oldMouseY = y;

	glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////

void mouse(int button, int state, int x, int y)
{
	oldMouseX = x; // Capture the coordinates of the mouse.
	oldMouseY = y;
	
	glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////

void loadConfig (void)
{
	char temp[64];
	int i;

	ifstream config ("Data/config.ini");

	for (i=0; i<9; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		config >> temp[i];
	}

	config >> xPixels;

	for (i=0; i<9; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		config >> temp[i];
	}

	config >> yPixels;

	for (i=0; i<9; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		config >> temp[i];
	}

	config >> Camera.x;

	for (i=0; i<9; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		config >> temp[i];
	}

	config >> Camera.y;

	for (i=0; i<9; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		config >> temp[i];
	}

	config >> Camera.z;

	for (i=0; i<11; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		config >> temp[i];
	}

	config >> xRotation;

	for (i=0; i<11; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		config >> temp[i];
	}

	config >> yRotation;

	config.close();
}

///////////////////////////////////////////////////////////////////

void loadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap)
{
	FILE *pFile = NULL;

	// Let's open the file in Read/Binary mode.
	pFile = fopen( strName, "rb" );

	// Check to see if we found the file and could open it
	if ( pFile == NULL )	
	{
		// Display our error message and stop the function
		MessageBox(NULL, "Could not find the RAW file", "Error", MB_OK);
		return;
	}

	// Here we load the .raw file into our pHeightMap data array.
	// We are only reading in '1', and the size is the (width * height)
	fread( pHeightMap, 1, nSize, pFile );

	// After we read the data, it's a good idea to check if everything read fine.
	int result = ferror( pFile );

	// Check if we received an error.
	if (result)
	{
		MessageBox(NULL, "Can't get data!", "Error", MB_OK);
	}

	// Close the file.
	fclose(pFile);
}

///////////////////////////////// CREATE TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void createTexture(UINT textureArray[], LPSTR strFileName, int textureID)
{
	if(!strFileName)									// Return from the function if no file name was passed in
		return;
	

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *	
	
	tImageJPG *pImage = loadJPG(strFileName);			// Load the image and store the data

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	

	if(pImage == NULL)									// If we can't load the file, quit!
		exit(0);

	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, &textureArray[textureID]);

	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);

	// Build Mipmaps (builds different versions of the picture for distances - looks better)
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pImage->sizeX, pImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);

	// Lastly, we need to tell OpenGL the quality of our texture map.  GL_LINEAR_MIPMAP_LINEAR
	// is the smoothest.  GL_LINEAR_MIPMAP_NEAREST is faster than GL_LINEAR_MIPMAP_LINEAR, 
	// but looks blochy and pixilated.  Good for slower computers though. 
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);	


	// Now we need to free the image data that we loaded since OpenGL stored it as a texture

	if (pImage)										// If we loaded the image
	{
		if (pImage->data)							// If there is texture data
		{
			free(pImage->data);						// Free the texture data, we don't need it anymore
		}

		free(pImage);								// Free the image structure
	}
}

///////////////////////////////// LOAD JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

tImageJPG *loadJPG(const char *filename)
{
	struct jpeg_decompress_struct cinfo;
	tImageJPG *pImageData = NULL;
	FILE *pFile;
	
	// This is the only function you should care about.  You don't need to
	// really know what all of this does (since you can't cause it's a library!) :)
	// Just know that you need to pass in the jpeg file name, and get a pointer
	// to a tImageJPG structure which contains the width, height and pixel data.
	// Be sure to free the data after you are done with it, just like a bitmap.
	
	// Open a file pointer to the jpeg file and check if it was found and opened 
	if((pFile = fopen(filename, "rb")) == NULL) 
	{
		return NULL;
	}
	
	// Create an error handler
	jpeg_error_mgr jerr;

	// Have our compression info object point to the error handler address
	cinfo.err = jpeg_std_error(&jerr);
	
	// Initialize the decompression object
	jpeg_create_decompress(&cinfo);
	
	// Specify the data source (Our file pointer)	
	jpeg_stdio_src(&cinfo, pFile);
	
	// Allocate the structure that will hold our eventual jpeg data (must free it!)
	pImageData = (tImageJPG*)malloc(sizeof(tImageJPG));

	// Decode the jpeg file and fill in the image data structure to pass back
	decodeJPG(&cinfo, pImageData);
	
	// This releases all the stored memory for reading and decoding the jpeg
	jpeg_destroy_decompress(&cinfo);
	
	// Close the file pointer that opened the file
	fclose(pFile);

	// Return the jpeg data (remember, you must free this data after you are done)
	return pImageData;
}

///////////////////////////////// DECODE JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void decodeJPG(jpeg_decompress_struct* cinfo, tImageJPG *pImageData)
{
	// Read in the header of the jpeg file
	jpeg_read_header(cinfo, TRUE);
	
	// Start to decompress the jpeg file with our compression info
	jpeg_start_decompress(cinfo);

	// Get the image dimensions and row span to read in the pixel data
	pImageData->rowSpan = cinfo->image_width * cinfo->num_components;
	pImageData->sizeX   = cinfo->image_width;
	pImageData->sizeY   = cinfo->image_height;
	
	// Allocate memory for the pixel buffer
	pImageData->data = new unsigned char[pImageData->rowSpan * pImageData->sizeY];
		
	// Here we use the library's state variable cinfo.output_scanline as the
	// loop counter, so that we don't have to keep track ourselves.
	
	// Create an array of row pointers
	unsigned char** rowPtr = new unsigned char*[pImageData->sizeY];
	for (int i = 0; i < pImageData->sizeY; i++)
		rowPtr[i] = &(pImageData->data[i*pImageData->rowSpan]);

	// Now comes the juice of our work, here we extract all the pixel data
	int rowsRead = 0;
	while (cinfo->output_scanline < cinfo->output_height) 
	{
		// Read in the current row of pixels and increase the rowsRead count
		rowsRead += jpeg_read_scanlines(cinfo, &rowPtr[rowsRead], cinfo->output_height - rowsRead);
	}
	
	// Delete the temporary row pointers
	delete [] rowPtr;

	// Finish decompressing the data
	jpeg_finish_decompress(cinfo);
}
