// open_gl.cpp: implementation of the open_gl class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/open_gl.h"
#include <gl\glut.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>

#define XPIXELS		768
#define YPIXELS		768	
#define SCALE		400
#define DEG2RAD		0.0174532925199

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

open_gl::open_gl()
{ 
	fp_x = 0;
	fp_y = 0;
	fp_z = 0;

	wireframe = 1;
	water_animation = 14;
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
// This function draws the cursor that is seen when the player selects 
// a unit.
void open_gl::gl_draw_cursor(float x, float y, float z)
{
	static int rotate;			

	rotate++;

	glColor3f(1.0, 0.0, 0.0);
	
	glPushMatrix();
		glTranslatef(x-25, y, z);
		glRotatef( 90, 0.0, 1.0, 0.0 );
		glutSolidCone(8, 8, 8, 1);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(x+25, y, z);
		glRotatef( -90, 0.0, 1.0, 0.0 );
		glutSolidCone(8, 8, 8, 1);
	glPopMatrix();

	glPopMatrix();

	glPushMatrix();
		glTranslatef(x, y, z-25);
		glutSolidCone(8, 8, 8, 1);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(x, y, z+25);
		glRotatef( -180, 1.0, 0.0, 0.0 );
		glutSolidCone(8, 8, 8, 1);
	glPopMatrix();

	if (rotate > 360)
	{
		rotate = 0;
	}
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_draw_sphere(float x, float y, float z, float size, float red, float green, float blue)
{
	glColor3f(red, green, blue);

	glPushMatrix();
		glTranslatef(x, y, z);
		glutSolidSphere(size, 64, 64);
	glPopMatrix();
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_draw_object(float x, float y, float z, int mode, int size, float red, float green, float blue)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	glColor3f(red, green, blue);

	switch (mode)
	{
		case SPHERE:	glutSolidSphere(size, 64, 64);
		
						break;

		case CUBE:		glutSolidCube(size);

						break;

		default:	break;							//Again, the default value to break out 
	}

	glPopMatrix();
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_draw_point(float x, float y, float z)
{
//	glPointSize(10);

//	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_POINTS);
		glVertex3f(x, y, z);
	glEnd();
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_draw_line(float x1, float y1, float z1, float x2, float y2, float z2)
{
	glLineWidth(2);

	glEnable(GL_LINE_SMOOTH);

	glBegin(GL_LINES);
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
	glEnd();
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_draw_polygon(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
	glPointSize(2);

	glBegin(GL_POLYGON);
		glNormal3f(x1, y1, z1);
		glVertex3f(x1, y1, z1);

		glNormal3f(x2, y2, z2);
		glVertex3f(x2, y2, z2);

		glNormal3f(x3, y3, z3);
		glVertex3f(x3, y3, z3);
	glEnd();
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_draw_textured_polygon(float x1, float y1, float z1,
		  							   float x2, float y2, float z2,
									   float x3, float y3, float z3,
									   float x4, float y4, float z4,
									   int hud)
{
	if(hud == 1)
	{
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		glEnable(GL_LIGHTING);
	}

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

	if(hud == 1)
	{
		glDisable(GL_TEXTURE_2D);
	}
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_draw_plane(float size, float y)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glBlendFunc(GL_DST_COLOR,GL_ZERO); // USE FOR MASKING

	glColor4f(1.0, 1.0, 1.0, 0.5);

	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
		glTexCoord2f(water_animation, water_animation);
		glNormal3f(size, y, size);
		glVertex3f(size, y, size);
		
		glTexCoord2f(water_animation, 0.0f);
		glNormal3f(size, y, -size);
		glVertex3f(size, y, -size);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(-size, y, -size);
		glVertex3f(-size, y, -size);

		glTexCoord2f(0.0f, water_animation);
		glNormal3f(-size, y, size);
		glVertex3f(-size, y, size);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_initialise_perspective(void)
{
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

	glEnable(GL_LIGHTING);	// Enable GL lighting
	glEnable(GL_LIGHT0);	// Enable the light we just defined.
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_perspective_viewing_system(void)
{
	float sceneroty = 360.0f - yrot;

	glLoadIdentity();

//	glTranslatef(0.0, 0.0, heading);	// Standard translation
//	glRotatef( x_pos, 1.0, 0.0, 0.0 );			// Standard primary rotation
//	glRotatef( y_pos, 0.0, 1.0, 0.0 );			// Secondary rotation.

	// Standard translation
	glRotatef( -x_pos, 1.0, 0.0, 0.0 );			// Secondary rotation.
//	glRotatef( sceneroty, 0.0, 1.0, 0.0 );			// Standard primary rotation
	glTranslatef(fp_x, fp_y, fp_z);
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_initialise_2d(void)
{
	glMatrixMode(GL_PROJECTION);			// Define the basic projection system for OpenGL 
	glLoadIdentity();
	gluPerspective(45, 1.0, 0.1, 10000.0);	// Set a perspective view system 
	glMatrixMode(GL_MODELVIEW);				// Set the modelview matrix mode. This means all succeeding 
	glLoadIdentity();	
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_2d_viewing_system(float zoom, int xrot, int yrot, float cam1, float cam2)
{
	glLoadIdentity();					// Load identity matrix
	glTranslatef(0.0, 0.0, -2);			// Translate away from the camera
	glTranslatef(cam1, cam2, 0.0);		// Translate up/down, left/right
	glTranslatef(0.0, 0.0, -zoom);		// Zoom in or out
	glRotatef( xrot, 1.0, 0.0, 0.0 );	// Standard rotation viewng system
	glRotatef( yrot, 0.0, 1.0, 0.0 );
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_keyboard(unsigned char key_press)
{
	switch (key_press)							// Key is the variable to switch between. Given to us by GL
	{
		case 'w':	//heading ++;

					fp_x += (float)sin(heading * DEG2RAD) * 5.0f;
				//	fp_y += (float)cos(y_pos * DEG2RAD) * 1.5f;
					fp_z += (float)cos(heading * DEG2RAD) * 5.0f;

					break;

		case 's':	//heading --;

					fp_x -= (float)sin(heading * DEG2RAD) * 5.0f;
				//	fp_y -= (float)sin(y_pos * DEG2RAD) * 1.5f;
					fp_z -= (float)cos(heading * DEG2RAD) * 5.0f;

					break;

		case 'a':	fp_y -= 2;

					break;

		case 'd':	fp_y += 2;

					break;

		case 'f':   glutFullScreen ();

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

void open_gl::gl_light(float x, float y, float z, float ambience, float specular, float white_light1)
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

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_mouse(int x, int y, int button_new, int state_new)
{
	old_x = x; // Capture the coordinates of the mouse.
	old_y = y;
	state = state_new;
	button = button_new;
	
	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_motion(int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
	/*	x_pos += (old_y - y) / 2.0;	// Effect the orbitting angles for the camera. 
		y_pos += (old_x - x) / 2.0;

		if ( x < old_x)
		{
			heading += old_x - x;	 The faster the user moves the mouse, the greater the movement 
			yrot = heading;					 the user is trying to move faster, therefore the difference is geater 
		}
		else if ( x > old_x)
		{
			heading -= x - old_x;
			yrot = heading;
		}

		old_x = x;					// Set the old coordinates ready for use next time.
		old_y = y;	USE THIS STUFF FOR FP CAMERA VIEWING SYSTEM	*/	

		fp_x += (old_x - x) / 2.0;
		fp_z += (old_y - y) / 2.0;

		old_x = x;					// Set the old coordinates ready for use next time.
		old_y = y;

		if (fp_x < -600)
		{
			fp_x = -600;
		}

		else if (fp_x > 625)
		{
			fp_x = 625;
		}

		if (fp_z < -625)
		{
			fp_z = -625;
		}

		else if (fp_z > 800)
		{
			fp_z = 800;
		}
	}

	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		fp_y += (old_y - y) / 2.0;

		old_x = x;					// Set the old coordinates ready for use next time.
		old_y = y;

		if (fp_y < -500)
		{
			fp_y = -500;
		}
		else if (fp_y > -25)
		{
			fp_y = -25;
		}
	}
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_text_renderer(float x, float y, void *font, char *string, float r, float g, float b)
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

void open_gl::gl_idle(void)
{	
	static int switch_tag;

	if ( water_animation > 20 )
	{
		switch_tag = 0;
	}
	else if ( water_animation < 15 )
	{
		switch_tag = 1;
	}

	if (switch_tag == 0)
	{
		water_animation -= 0.005;
	}
	else if (switch_tag == 1)
	{
		water_animation += 0.005;
	}
}

//////////////////////////////////////////////////////////////////////

bool open_gl::gl_load_bitmap(char* szFileName)
{
	HANDLE hFileHandle;
	BITMAPINFO *pBitmapInfo = NULL;
	unsigned long lInfoSize = 0;
	unsigned long lBitSize = 0;
	int nTextureWidth;
	int nTextureHeight;
	BYTE *pBitmapData = NULL;


	// Open the Bitmap file
	hFileHandle = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,
		NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL);

	// Check for open failure (most likely file does not exist).
	if(hFileHandle == INVALID_HANDLE_VALUE)
		return FALSE;

	// File is Open. Read in bitmap header information
	BITMAPFILEHEADER	bitmapHeader;
	DWORD dwBytes;
	ReadFile(hFileHandle,&bitmapHeader,sizeof(BITMAPFILEHEADER),	
		&dwBytes,NULL);

	__try {
		if(dwBytes != sizeof(BITMAPFILEHEADER))
			return FALSE;

		// Check format of bitmap file
		if(bitmapHeader.bfType != 'MB')
			return FALSE;

		// Read in bitmap information structure
		lInfoSize = bitmapHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
		pBitmapInfo = (BITMAPINFO *) new BYTE[lInfoSize];

		ReadFile(hFileHandle,pBitmapInfo,lInfoSize,&dwBytes,NULL);

		if(dwBytes != lInfoSize)
			return FALSE;


		nTextureWidth = pBitmapInfo->bmiHeader.biWidth;
		nTextureHeight = pBitmapInfo->bmiHeader.biHeight;
		lBitSize = pBitmapInfo->bmiHeader.biSizeImage;

		if(lBitSize == 0)
			lBitSize = (nTextureWidth *
               pBitmapInfo->bmiHeader.biBitCount + 7) / 8 *
  			  abs(nTextureHeight);
	
		// Allocate space for the actual bitmap
		pBitmapData = new BYTE[lBitSize];

		// Read in the bitmap bits
		ReadFile(hFileHandle,pBitmapData,lBitSize,&dwBytes,NULL);

		if(lBitSize != dwBytes)
			{
			if(pBitmapData)
				delete [] (BYTE *) pBitmapData;
			pBitmapData = NULL;
			
			return FALSE;
			}
		}
	__finally // Fail or success, close file and free working memory
		{
		CloseHandle(hFileHandle);

		if(pBitmapInfo != NULL)
			delete [] (BYTE *)pBitmapInfo;
		}

   
	// This is specific to the binary format of the data read in.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, nTextureWidth, nTextureHeight, 0,
                 GL_BGR_EXT, GL_UNSIGNED_BYTE, pBitmapData);

	if(pBitmapData)
		delete [] (BYTE *) pBitmapData;

	glEnable(GL_LIGHTING);	// Enable GL lighting
	glEnable(GL_LIGHT0);	// Enable the light we just defined.

	return TRUE;
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_setup_textures(int climate)
{
	tex_grass = glGenLists(30);
	tex_river_bank = tex_grass +1; 
	tex_water = tex_river_bank +1;
	tex_tree = tex_water +1;
	tex_sky_box = tex_tree +1;
	tex_hq = tex_sky_box +1;
	tex_player_one = tex_hq +1;
	tex_player_two = tex_player_one +1;
	tex_neutral = tex_player_two +1;
	tex_grass_grid = tex_neutral +1;
	tex_hud = tex_grass_grid +1;
	tex_smoke = tex_hud +1;
	tex_smoke_one = tex_smoke +1;
	tex_build_barracks = tex_smoke_one +1;
	tex_build_gtower = tex_build_barracks +1;
	tex_build_war = tex_build_gtower +1;
	tex_tank_build = tex_build_war +1;
	tex_trooper_build = tex_tank_build +1;
	tex_artillery_build = tex_trooper_build +1;
	tex_player_one_hq = tex_artillery_build +1;
	tex_player_two_hq = tex_player_one_hq +1;
	tex_player_one_barracks = tex_player_two_hq +1;
	tex_player_two_barracks = tex_player_one_barracks +1;
	tex_player_one_trooper = tex_player_two_barracks +1;
	tex_player_two_trooper = tex_player_one_trooper +1;
	tex_player_one_guard_tower = tex_player_two_trooper +1;
	tex_player_two_guard_tower = tex_player_one_guard_tower +1;
	tex_player_one_tank = tex_player_two_guard_tower +1;
	tex_player_two_tank = tex_player_one_tank +1;

	if (climate == 0)
	{
		glNewList(tex_grass_grid,GL_COMPILE);
			gl_load_bitmap((char*)"Data/Textures/tex_grass_grid.bmp");
		glEndList();
	}

	else if (climate == 1)
	{
		glNewList(tex_grass_grid,GL_COMPILE);
			gl_load_bitmap((char*)"Data/Textures/tex_ice_grid.bmp");
		glEndList();
	}

	else if (climate == 2)
	{
		glNewList(tex_grass_grid,GL_COMPILE);
			gl_load_bitmap((char*)"Data/Textures/tex_sand_grid.bmp");
		glEndList();
	}

	if (climate == 0)
	{
		glNewList(tex_grass,GL_COMPILE);
			gl_load_bitmap((char*)"Data/Textures/tex_grass.bmp");
		glEndList();
	}

	else if (climate == 1)
	{
		glNewList(tex_grass,GL_COMPILE);
			gl_load_bitmap((char*)"Data/Textures/tex_ice.bmp");
		glEndList();
	}

	else if (climate == 2)
	{
		glNewList(tex_grass,GL_COMPILE);
			gl_load_bitmap((char*)"Data/Textures/tex_sand.bmp");
		glEndList();
	}

	glNewList(tex_river_bank,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_sand_grid.bmp");
	glEndList();

	glNewList(tex_water,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_water.bmp");
	glEndList();

	if (climate == 0)
	{
		glNewList(tex_tree,GL_COMPILE);
			gl_load_bitmap((char*)"Data/Textures/tex_tree.bmp");
		glEndList();
	}

	if (climate == 1)
	{
		glNewList(tex_tree,GL_COMPILE);
			gl_load_bitmap((char*)"Data/Textures/tex_tree_snowy.bmp");
		glEndList();
	}

	if (climate == 2)
	{
		glNewList(tex_tree,GL_COMPILE);
			gl_load_bitmap((char*)"Data/Textures/tex_tree_palm.bmp");
		glEndList();
	}

	glNewList(tex_sky_box,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_sky_box.bmp");
	glEndList();

	glNewList(tex_hq,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_hq.bmp");
	glEndList();

	glNewList(tex_player_one,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_player_one01.bmp");
	glEndList();

	glNewList(tex_player_two,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_player_two01.bmp");
	glEndList();

	glNewList(tex_neutral,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_neutral.bmp");
	glEndList();

	glNewList(tex_hud,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_hud1.bmp");
	glEndList();

	glNewList(tex_smoke,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_smoke.bmp");
	glEndList();

	glNewList(tex_smoke_one,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_smoke1.bmp");
	glEndList();

	glNewList(tex_build_barracks,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_barracks_build.bmp");
	glEndList();

	glNewList(tex_build_gtower,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_guard_build.bmp");
	glEndList();

	glNewList(tex_build_war,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_war_build.bmp");
	glEndList();

	glNewList(tex_tank_build,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_tank_build.bmp");
	glEndList();

	glNewList(tex_trooper_build,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_trooper_build.bmp");
	glEndList();

	glNewList(tex_artillery_build,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_artillery_build.bmp");
	glEndList();

	glNewList(tex_player_one_hq,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_player_one_hq.bmp");
	glEndList();

	glNewList(tex_player_two_hq,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_player_two_hq.bmp");
	glEndList();

	glNewList(tex_player_one_barracks,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_player_one_barracks.bmp");
	glEndList();

	glNewList(tex_player_two_barracks,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_player_two_barracks.bmp");
	glEndList();

	glNewList(tex_player_one_trooper,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_player_one_trooper.bmp");
	glEndList();

	glNewList(tex_player_two_trooper,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_player_two_trooper.bmp");
	glEndList();

	glNewList(tex_player_one_guard_tower,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_player_one_guard_tower.bmp");
	glEndList();

	glNewList(tex_player_two_guard_tower,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_player_two_guard_tower.bmp");
	glEndList();

	glNewList(tex_player_one_tank,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_player_one_tank.bmp");
	glEndList();

	glNewList(tex_player_two_tank,GL_COMPILE);
		gl_load_bitmap((char*)"Data/Textures/tex_player_two_tank.bmp");
	glEndList();

	gl_setup_texture_parameters();
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_setup_texture_parameters(void)
{
	// Set Texture mapping parameters
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glEnable(GL_CULL_FACE);		// Do not draw inside of cube
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_draw_textured_skybox(void)
{
	float fSize = 1500.0f;

	glColor3f(0.75, 0.75, 0.75);

	glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);

	// Front face of Cube
//	glCallList(tex_skybox_front);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(fSize,fSize, fSize);
		glVertex3f(fSize,fSize, fSize);

		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(fSize,-fSize, fSize);
		glVertex3f(fSize,-fSize, fSize);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(-fSize, -fSize, fSize);
		glVertex3f(-fSize, -fSize, fSize);

		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(-fSize, fSize, fSize);
		glVertex3f(-fSize, fSize, fSize);
	glEnd();

	// Back face of Cube
//	glCallList(tex_skybox_back);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(-fSize, fSize, -fSize);
		glVertex3f(-fSize, fSize, -fSize);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(-fSize, -fSize, -fSize);
		glVertex3f(-fSize, -fSize, -fSize);

		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(fSize,-fSize, -fSize);
		glVertex3f(fSize,-fSize, -fSize);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(fSize,fSize, -fSize);
		glVertex3f(fSize,fSize, -fSize);
	glEnd();

	// Top Face of Cube
//	glCallList(tex_skybox_top);
	glBegin(GL_QUADS);

		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(-fSize, fSize, -fSize);
		glVertex3f(-fSize, fSize, -fSize);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(fSize, fSize, -fSize);
		glVertex3f(fSize, fSize, -fSize);

		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(fSize, fSize, fSize);
		glVertex3f(fSize, fSize, fSize);



		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(-fSize, fSize, fSize);
		glVertex3f(-fSize, fSize, fSize);
	glEnd();


	// Bottom Face of Cube
//	glCallList(tex_skybox_bottom);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(-fSize, -fSize, fSize);
		glVertex3f(-fSize, -fSize, fSize);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(fSize, -fSize, fSize);
		glVertex3f(fSize, -fSize, fSize);
		glTexCoord2f(0.0f, 1.0f);

		glNormal3f(fSize, -fSize, -fSize);
		glVertex3f(fSize, -fSize, -fSize);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(-fSize, -fSize, -fSize);
		glVertex3f(-fSize, -fSize, -fSize);
	glEnd();


	// Left hand side of cube
//	glCallList(tex_skybox_left);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(-fSize, fSize, fSize);
		glVertex3f(-fSize, fSize, fSize);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(-fSize, -fSize, fSize);
		glVertex3f(-fSize, -fSize, fSize);

		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(-fSize, -fSize, -fSize);
		glVertex3f(-fSize, -fSize, -fSize);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(-fSize, fSize, -fSize);
		glVertex3f(-fSize, fSize, -fSize);
	glEnd();


	// Right hand side of cube
//	glCallList(tex_skybox_right);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(fSize, fSize, -fSize);
		glVertex3f(fSize, fSize, -fSize);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(fSize, -fSize, -fSize);
		glVertex3f(fSize, -fSize, -fSize);

		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(fSize, -fSize, fSize);
		glVertex3f(fSize, -fSize, fSize);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(fSize, fSize, fSize);
		glVertex3f(fSize, fSize, fSize);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_draw_textured_cube(float fSize)
{
	glColor3f(0.75, 0.75, 0.75);

	glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);

	// Front face of Cube
//	glCallList(tex_bricks);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(-fSize, fSize, fSize);
		glVertex3f(-fSize, fSize, fSize);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(-fSize, -fSize, fSize);
		glVertex3f(-fSize, -fSize, fSize);

		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(fSize,-fSize, fSize);
		glVertex3f(fSize,-fSize, fSize);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(fSize,fSize, fSize);
		glVertex3f(fSize,fSize, fSize);
	glEnd();

	// Back face of Cube
//	glCallList(tex_bricks);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(fSize,fSize, -fSize);
		glVertex3f(fSize,fSize, -fSize);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(fSize,-fSize, -fSize);
		glVertex3f(fSize,-fSize, -fSize);

		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(-fSize, -fSize, -fSize);
		glVertex3f(-fSize, -fSize, -fSize);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(-fSize, fSize, -fSize);
		glVertex3f(-fSize, fSize, -fSize);
	glEnd();

	// Top Face of Cube
//	glCallList(tex_bricks);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(-fSize, fSize, fSize);
		glVertex3f(-fSize, fSize, fSize);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(fSize, fSize, fSize);
		glVertex3f(fSize, fSize, fSize);

		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(fSize, fSize, -fSize);
		glVertex3f(fSize, fSize, -fSize);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(-fSize, fSize, -fSize);
		glVertex3f(-fSize, fSize, -fSize);
	glEnd();


	// Bottom Face of Cube
//	glCallList(tex_bricks);
	glBegin(GL_QUADS);

		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(-fSize, -fSize, -fSize);
		glVertex3f(-fSize, -fSize, -fSize);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(fSize, -fSize, -fSize);
		glVertex3f(fSize, -fSize, -fSize);

		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(fSize, -fSize, fSize);
		glVertex3f(fSize, -fSize, fSize);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(-fSize, -fSize, fSize);
		glVertex3f(-fSize, -fSize, fSize);
	glEnd();


	// Left hand side of cube
//	glCallList(tex_bricks);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(-fSize, fSize, -fSize);
		glVertex3f(-fSize, fSize, -fSize);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(-fSize, -fSize, -fSize);
		glVertex3f(-fSize, -fSize, -fSize);

		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(-fSize, -fSize, fSize);
		glVertex3f(-fSize, -fSize, fSize);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(-fSize, fSize, fSize);
		glVertex3f(-fSize, fSize, fSize);
	glEnd();


	// Right hand side of cube
//	glCallList(tex_bricks);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(fSize, fSize, fSize);
		glVertex3f(fSize, fSize, fSize);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(fSize, -fSize, fSize);
		glVertex3f(fSize, -fSize, fSize);

		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(fSize, -fSize, -fSize);
		glVertex3f(fSize, -fSize, -fSize);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(fSize, fSize, -fSize);
		glVertex3f(fSize, fSize, -fSize);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_draw_textured_2d_square(float size, float y_component, double x, double y, double z)
{
	glColor3f(0.75, 0.75, 0.75);

	glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
		glRotatef( 180, 0.0, 0.0, 1.0 );
		glTranslatef(x,y,z);

	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-size, -y_component, 0);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(size, -y_component, 0);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(size, y_component, 0);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-size, y_component, 0);
	glEnd();

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_draw_textured_2d_plane(void)
{
	float fSize = 500.0f;

	glColor3f(0.75, 0.75, 0.75);

	glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(-fSize, 0, fSize);
		glVertex3f(-fSize, 0, fSize);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(fSize, 0, fSize);
		glVertex3f(fSize, 0, fSize);

		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(fSize, 0, -fSize);
		glVertex3f(fSize, 0, -fSize);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(-fSize, 0, -fSize);
		glVertex3f(-fSize, 0, -fSize);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_draw_textured_sphere(float size, int atmos)
{
	glDisable(GL_LIGHTING);

	glPushMatrix();
		glRotatef( 90, 0.0, 0.0, 1.0 );

		glColor3f(0.5, 0.5, 0.5);

		glEnable(GL_TEXTURE_2D);

		GLUquadricObj *pObj = gluNewQuadric();	
		gluQuadricTexture(pObj, true);	
		gluSphere(pObj, size, 32, 32);

		glDisable(GL_TEXTURE_2D);

		if (atmos == 1)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glColor4f(0.5, 0.5, 0.5, 0.25);
			
			gluQuadricTexture(pObj, true);	
			gluSphere(pObj, size+0.2, 32, 32);

			glDisable(GL_BLEND);
		}

	glPopMatrix();

	glEnable(GL_LIGHTING);
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_reshape(int width, int height)
{
	glViewport(0, 0, width, height);				/* Create the viewport with the new values */

	glMatrixMode(GL_PROJECTION);					/* Re-initialise the projection mode */
	glLoadIdentity();
	gluPerspective(45, (float)width / (float)height, 0.1, 10000.0); /* Set the new perspective matrix */
	glMatrixMode(GL_MODELVIEW);						/* Reset the modelview mode */
	glLoadIdentity();
}

//////////////////////////////////////////////////////////////////////

void open_gl::gl_setup_fog(float start, float end)
{
	GLfloat fogColor[4] = {0.75, 0.75, 0.75, 1.0};
		
	glEnable(GL_FOG);

	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.35);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, start);
	glFogf(GL_FOG_END, end);

	glClearColor(0.75, 0.75, 0.75, 1.0);
}