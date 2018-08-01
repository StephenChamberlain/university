/*******************************************************************

	Welcome to Trench War! Version 1.0!
	-----------------------------------

  Title: Trench War! Final Year Project
  Author: Stephen Chamberlain
  Course: BSc (Hons) Interactive Computer Entertainment
  Supervisor: Keith Ditchburn
  Second Reader: Richard Henry

  Below is the implementation for Trench War. List of implemented
  features:

	Frustum culling 
	Backface culling 
	Texture call lists 
	ASE file loading and texture mapping 
	Multiple window management 
	WAV and MP3 playing support 
	Picking 
	Radar 
	HUD 
	AI 
	Height maps 
	Transparency rendering 
	Dealing with cursor movement at the edge of the screen so that it scrolls properly 
	Building (Construction)

  Though this isnt an exhaustive list it gives a view of some of the major 
  elements accomplished during this project.

*******************************************************************/

////////////////////////////////////////////////////////////////////
// HEADER FILES
////////////////////////////////////////////////////////////////////

#include <GL/glut.h>		// Glut 		
#include <GL/gl.h>			// OpenGl 
#include <iostream>			// Input, output 
#include <fstream>			// File management 
#include <windows.h>		// Windows. Some windows functions are used
#include <time.h>			// Used for timing
#include <timeapi.h>
#include <math.h>			// Used for cos, sin etc
//#include <dshow.h>			// The only DirectX component of the project, used
							// for mp3 playback during the game.

////////////////////////////////////////////////////////////////////
// MY OWN CUSTOM HEADER FILES
////////////////////////////////////////////////////////////////////

#include "../header/glui.h"						// GLUI user interface header file
#include "../header/open_gl.h"					// My own OpenGL header file
#include "../header/Vertex.h"					// Class to store vertices for models
#include "../header/Face.h"						// Class for storing faces for models
#include "../header/BoundingBox.h"				// Class for storing model bounding box
#include "../header/Tree.h"						// Class for storing information about trees
#include "../header/c_terrain.h"				// Class for storing terrain information
#include "../header/Bank.h"						// Stores bank information
#include "../header/HQData.h"					// Seperate class to the other buildings for the players headquarters
#include "../header/ArtificialIntelligence.h"	// Controlling class for ai
#include "../header/Units.h"					// Stores individual unit data
#include "../header/BuildingData.h"				// All other buildings are handled with this class
#include "../header/Frustum.h"					// This is the frustum culling control class
#include "../header/Camera.h"					// Used for camera position

////////////////////////////////////////////////////////////////////
// STANDARD NAMESPACE
////////////////////////////////////////////////////////////////////

using namespace std;		// Standard namespace

////////////////////////////////////////////////////////////////////
// DEFINED CONSTANTS
////////////////////////////////////////////////////////////////////

#define XPIXELS			768		// Number of pixels in X direction
#define YPIXELS			768		// Number of pixels in Y direction
#define SCALE			400		// Scale used for the viewport
#define TERRAIN_SIZE	50		// This is the dimension [< X >] [/\ Y \/] of the terrain array

#define KILL_SUBWINDOW	0		// This is a redundant constant. It was used when researching collapsable in-game
								// toolbars and was in conjunction with GLUI. This research was unsuccessful but is
								// included along with the subwindow_callback() function.
#define START_GAME		1		// Used to switch between game modes

#define DEG2RAD		0.0174532925199	// constant value used to transform DEGREE angles into RADIAN 
#define PI			3.141592654		// A value given for PI

// As OpenGL picking works by giving a name to polygons defined between glBegin() and glEnd() this constant
// provides a suitably large enough number that it is 'out of the way' of the actual values used by the 
// selectable units, therefore removing any chance of error.
#define NON_SELECTABLE_OBJECTS	99999 
	
#define NUM_PARTICLE	300		// This is the number of particles used by the prototype particle system.
								// This was abandoned due to time constraints.

#define NUM_BUILDINGS	100		// Maximum buildings possible in the game.
#define NUM_UNITS		100		// Maximum units possible in the game.

#define TANK_SPEED		0.35	// These are speeds given to the three units.
#define ARTILLERY_SPEED	0.5	
#define TROOPER_SPEED	0.75

#define TANK_ATTACK			100	// These are the attack values of the three in game units
#define ARTILLERY_ATTACK	250
#define TROOPER_ATTACK		25

////////////////////////////////////////////////////////////////////
// ENUMERATED TYPES 
////////////////////////////////////////////////////////////////////

// Used for true and false situations.
enum boolean_values {YES, NO};	

// Used for the units class
enum units {TREE, TANK, BANK, HQ, BARRACKS, WAR_FACTORY, TOWER, ARTILLERY, TROOPER, FREE};	

// Distinction between the three agents in the game
enum entities {PLAYER_ONE_HUMAN, PLAYER_TWO_CPU, NEUTRAL};

// This is used for the AI state
enum intel_state {AGGRESIVE, PASSIVE, GATHER_RESOURCES, BUILD_BARRACKS, BUILD_WAR_FACTORY, BUILD_TOWER};

// Possible game states.
enum game_states {INITIALISE, GAME_MAIN_LOOP};

// Switches for loading the correct map. This is hard coded and a future development would be to remove this in favour
// of a menu allowing the user to loacte a map file and load it, therefore allowing complete integration with Trench Edit!
enum maps {SAMPLE, VERDUN, PASS, GB, RUSSIA};

// The different IDs for the buildings.
enum building_id {ID_BARRACKS, ID_FREE, ID_PLACE_BUILDING, ID_QUIT, ID_WAR_FACTORY, ID_TOWER, ID_TANK, ID_TROOPER, ID_ARTILLERY};

////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES
////////////////////////////////////////////////////////////////////

void display (void);				// This is the main window function
void radar_display (void);			// Radar display function. Called from display()
void initialise (void);				// Sets initial variables when the program is started or the game loop is entered
void load_map (void);				// Loads the game map
void load_camera_settings (void);	// Loads the initial camera settings
void load_light_settings (void);	// Load the initial settings
void load_models (void);			// Load the game models
void load_ase_file (char *, int);	// Load a particular ASE model
void draw_terrain_polygons (int);	// Draws the polygons which make up the terrain grid
void mouse (int, int, int, int);	// Mouse callback function
void motion (int, int);				// Motion callback function
void keyboard (unsigned char, int, int);	// Keyboard callback function
void special (int, int, int);		// Keyboard callback function
void reshape(int, int);				// Reshapes the window
void idle (void);					// Called when not rendering
void draw_trees (void);				// Any function with draw in front of it indicates a particular 
void draw_hqs (void);				// par of the scene to be rendered. This allows the display()
void draw_banks (void);				// function to be split up.
void draw_units (void);			
void draw_buildings (void);
void draw_hud (void);				
void draw_hud_backgrounds (void);
void calculate_bounding_box (int);	// Calculates the eight points a models bounding box.
float distance_between_two_vectors (float, float, float, float, float, float); // This gives the distance between two vector points
void update_banks (void);			// This updates the status of all the banks in the scene.
void search_for_best_bank (void);	// Searches for the closest bank to the current CPU unit
void create_glui_subwindow (void);	// Creates the subwindow/toolbar in the game. Uses GLUI
void create_startup_window (void);	// Creates the starting window for the player to setup initial conditions.
void subwindow_callback (int);		// Callback for the subwindow
void startup_window_callback (int);	// Callback for the starting window
float calculate_orientation (float, float, float, float, float, float);	// Calculates the rotation a unit should under go to face its target 
void calculate_frame_rate (void);	// Calculates the frame rate of the game
void passive_motion (int, int);		// Stores data from the mouse when the user moves it WITHOUT pressing a button
void deal_with_cursor_at_edges (void);	// Moves the camera when the mouse is at the edge of the screen
int retrieve_object_id (int, int);	// Gets the ID number of the clicked object
void particle_systems (void);		// Performs particle system stuff
void move_units (void);				// Moves the units in the scene.
void glui_callback (int);			// Callback for GLUI
void build (void);					// Function to build the players buildings
void build_units (void);			// Function to build the players units 
void build_cpu (void);				// Function to build the cpu's buildings
void build_cpu_units (void);		// Function to build the cpu's units
void radar_water (void);			// Draws the water on the radar
void radar_buildings (void);		// Draws the buildings on the radar as dots. 
void radar_units (void);			// Same as above but for units
void radar_trees (void);			// See above 
void radar_banks (void);
void radar_main_camera (void);		// Positions the camera for the radar above the battlefield, looking downwards
void play_music (int);				// Plays the MP3 music
void draw_building_to_place (void);	// Creates a shadowy silhouette where the currently built building is to go
float magnitude(float, float, float);	// calculates the magnitude of a vector
void setup_player_one (void);		// Setup the two players respectively.
void setup_player_two (void);
void set_texture_coord(float, float, float, float);

////////////////////////////////////////////////////////////////////
// TYPEDEF STRUCT'S
////////////////////////////////////////////////////////////////////

// The following structs are all used to store vertex, face and 
// bounding box info for all the game models. They all have several
// things in common. They all have a num_vertices and num_faces 
// variable. These are used to loop and draw all the polygons that
// make up the model. They also include space to hold all the vertex 
// and face data. A class for holding bounding box information is 
// also present in each structure.
typedef struct {
	int num_vertices;
	int num_faces;
	CVertex vertices[118];
	CFace faces[175];
	CBoundingBox bounding_box[8];
} STreeModel;	// Tree structure

typedef struct {
	int num_vertices;
	int num_faces;
	CVertex vertices[231];
	CFace faces[429];
	CBoundingBox bounding_box[8];
} SHqModel;		// Headquarters structure

typedef struct {
	int num_vertices;
	int num_faces;
	CVertex vertices[145];
	CFace faces[276];
	CBoundingBox bounding_box[8];
} STankModel;	// Tank structure

typedef struct {
	int num_vertices;
	int num_faces;
	CVertex vertices[212];
	CFace faces[368];
	CBoundingBox bounding_box[8];
} STrooperModel;	// Trooper structure

typedef struct {
	int num_vertices;
	int num_faces;
	CVertex vertices[92];
	CFace faces[173];
	CBoundingBox bounding_box[8];
} SArtilleryModel;	// Artillery structure

typedef struct {
	int num_vertices;
	int num_faces;
	CVertex vertices[189];
	CFace faces[325];
	CBoundingBox bounding_box[8];
} SBankModel;	// Bank structure

typedef struct {
	int num_vertices;
	int num_faces;
	CVertex vertices[115];
	CFace faces[213];
	CBoundingBox bounding_box[8];
} SBarracksModel;	// Barracks structure

typedef struct {
	int num_vertices;
	int num_faces;
	CVertex vertices[137];
	CFace faces[281];
	CBoundingBox bounding_box[8];
} SGtowerModel;		// Guard tower structure

typedef struct {
	int num_vertices;
	int num_faces;
	CVertex vertices[343];
	CFace faces[657];
	CBoundingBox bounding_box[8];
} SWarFactoryModel;		// War factory model

// This struct is placed inside the playerInfo struct
// and conatins all the data for the players buildings
typedef struct {
	int num_of_buildings;						// Number of buildings the player has
	CBuildingData buildings[NUM_BUILDINGS];		// Building data
} SBuildingInfo;

// Struct for drawing the cursor were the building
// would be placed. It was originlly designed to include
// some classes, that is why it is not a class itself.
typedef struct {
	int building_id;				// ID of the building about to be placed
	int ready_to_place_building;	// Is there a building ready to be placed?
	int terrain_id_i;				// These are used to store the terrain points so that
	int terrain_id_j;				// the cursor moves over the terrain.
	float x;	
	float y;
	float z;
} SPlaceBuildingStruct;

// This structure contains all the data for the players including:
typedef struct {
	int score;
	int money;
	int num_of_banks_controlled;	// Money every three seconds = money + num_banks * 20
	int num_units;					// These numbers of are used for looping purposes
	int num_barracks;
	int num_guard_towers;
	int num_war_factories;
	int num_tanks;
	int num_troopers;
	int num_artillery;
	char *name;						// This is used to hold the name of the player
	CHQData hq_info;
	CUnits player_units[NUM_UNITS];
	SBuildingInfo player_buildings;
} SPlayerInfo;

////////////////////////////////////////////////////////////////////
// GLOBALLY DEFINED CLASSES
//
// Here globally defined instances of the above structs and classes
// are declared.
//
////////////////////////////////////////////////////////////////////

STreeModel tree; // Most of these instances are for the models
SHqModel hq;
STankModel tank;
STrooperModel trooper;
SArtilleryModel artillery;
SBankModel bank;
SBarracksModel barracks;
SGtowerModel tower;
SWarFactoryModel war_factory;

SPlayerInfo player_one;		// Player
SPlayerInfo player_two;		// CPU
SPlaceBuildingStruct place_building;	// Creates the shadowy outline of the building to be placed

CBank banks[10];			// Maximum number of banks on the map
CArtificialIntelligence ai;	// AI instance
CFrustum frustum;			// Fructum instance
CCamera radar_cam;			// Radar camera
CVertex place_for_selected_unit_to_go;	// This is the class that handles unit movement

c_terrain terrain[TERRAIN_SIZE][TERRAIN_SIZE];	// Class for storing terrain data
open_gl open_gl;			// Class containing functions and variables to control OpenGL

////////////////////////////////////////////////////////////////////
// GLOBALLY DEFINED VARIABLES
////////////////////////////////////////////////////////////////////

/******* Chars *******/
char load_filename[32];		// Filename of map to load
char text_buffer[512];		// Text buffer for displaying HUD text

/******* Integers *******/
int window_main;			// Window ID
int radar_window;			// Radar ID
int player_one_start;		// Player one start point
int player_two_start;		// Player two start point		
int	climate;				// Dictates the environment and therefore what textures/tree to load
int fog_tag;				// Is fog on?
int grid_size;				// Total grid size = TERRAIN_SIZE * TERRAIN_SIZE
int num_banks;				// Number of banks on map
int	font=(int)GLUT_BITMAP_9_BY_15;	// GLUT font to use on the HUD
int best_bank_num;			// Best bank for the CPU to capture
int search_for_best_bank_switch;	// Switch dictating the best bank ID
int game_state;				// Game state switch
int terrain_grid_tag;		// Toggles the terrain grid which swaps textures
int map_name;				// Switch to load the correct map
int track_name;				// Music track name
int p_x, p_y;				// Passive X and Y values
int fullscreen_x, fullscreen_y;	// Fullscreen X and Y values
int selected_unit = -1;		// Selected unit ID
int current_render_mode;	// The current render mode: either GL_RENDER or GL_SELECT
int create_building;		// Create building switch
int current_building_to_build;	// Current building to build ID
int current_building_number;	// Current building number
int music;					// Music on/off switch
int player_one_start_point, player_two_start_point; // Used to place the built building near the players HQ
int cpu_building_number;	// Building number of the CPU's current project
int build_switch;			// Is something being built?
int unit_build_switch;		// Same as above but for units
int cpu_unit_number;		// Unit number of current unit project
int	create_unit;			// Create unit switch
int	current_unit_to_build;	// Unit to build TANK, TROOPER, ARTILLERY
int	current_unit_number;	// Unit number of the current unit to build
int place_for_selected_unit_to_go_terrain_number;	// Terrain ID number of the selected unit cursor 

/******* Floats *******/
float fog_start;			// Fog start point
float fog_end;				// and end point.
float water_y;				// Water level
float ambience;				// Ambient light levels
float white_light;			// White light levels
float time_passed;			// Total time passed
float previous_time;		// Previous time of last CPU cycle
float frame_interval = 0.0f;	// Used to calculate the frame rate
float frames_per_second;	// Frames per second count
float particle_y[10];		// Particle storage structure

/******* Booleans *****/
bool ignore_frustum = false;	// Frustum clip variable

/******* GLUI Variables */
GLUI *glui_subwin;			// Glui subwindow ID
GLUI *glui_startup_window;	// Glui startup window ID

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

	window_main = glutCreateWindow("Trench War! :::: www.geocities.com/gobbo18uk");	// Create the window and name it 
  	glutDisplayFunc(display);			// Display the window 
	glutKeyboardFunc(keyboard);			// Register the keyboard strokes
	glutSpecialFunc(special);			// Special keyboard activity e.g. arrow keys etc
	glutMouseFunc(mouse);				// Register mouse activity
	glutMotionFunc(motion);				// Register mouse motion
	glutPassiveMotionFunc(passive_motion);	// Register passive mouse motion

	glutHideWindow();					// Hide the window at the beginning

	radar_window = glutCreateSubWindow(window_main,5,5,300,300);	// Display the 'top' view
	glutDisplayFunc(radar_display);		// Radar display callback

	glutHideWindow();					// Hide the window

	create_startup_window();			// Create the starting toolbar

	create_glui_subwindow();			// Create and hide the in game toolbar
	glui_subwin->hide();

	cout << "***********************************" << endl;		// Output a starting message
	cout << "**  Welcome to Trench War! V0.1  **" << endl;
	cout << "***********************************" << endl << endl << endl;

	glutMainLoop();				// Main GLUT loop command

	return 0;
}

////////////////////////////////////////////////////////////////////
// DISPLAY FUNCTION
// ----------------
//
// This called when rendering. This draws everything in the scene
//
////////////////////////////////////////////////////////////////////

void display (void)
{
	glutSetWindow(window_main);	// Set the focus to the main window

	if (game_state == INITIALISE)
	{
		glClearColor(0.75, 0.75, 0.75, 0.0);	// Clears the main window to a pleasent blue shade 

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear the colour buffer bit

		glutSwapBuffers();
	}

	else if (game_state == GAME_MAIN_LOOP)
	{
		if (current_render_mode == GL_RENDER)
		{
			glClearColor(0.75, 0.75, 0.75, 0.0);	// Clears the main window to a pleasent blue shade 

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear the colour buffer bit
		}

		open_gl.gl_perspective_viewing_system();
		open_gl.gl_light(0, 400, 0, 0.5, 0.0, 2.45);

		calculate_frame_rate();

		deal_with_cursor_at_edges();

		if (fog_tag == 1)	// Draw fog
		{
			open_gl.gl_setup_fog(fog_end, fog_start);
		}

		glInitNames(); // Load non-selectable names 
		glPushName(NON_SELECTABLE_OBJECTS);

		glLoadName(NON_SELECTABLE_OBJECTS);
			draw_trees();
			draw_terrain_polygons(YES);
		glEnd();	//end load name
		glPopName();

		draw_buildings();	// draw everything
		draw_hqs();
		draw_banks();
		draw_units();

		if (selected_unit == 0)
		{
			open_gl.gl_draw_cursor(place_for_selected_unit_to_go.get_x(), place_for_selected_unit_to_go.get_y(), place_for_selected_unit_to_go.get_z());
		}

	//	particle_systems();

		glPushMatrix();				// Draw water
			glTranslatef(-10,0,-10);
			glCallList(open_gl.tex_water);
			open_gl.gl_draw_plane(TERRAIN_SIZE*12.2, water_y-3);
			open_gl.gl_draw_plane(TERRAIN_SIZE*12.2, water_y);
		glPopMatrix();

		if (place_building.ready_to_place_building == YES)	// Draw shadowy sihouette of building
		{
			draw_building_to_place();
		}

		if (current_render_mode == GL_RENDER)	// HUDs and picking dont mix, so when picking this isnt drawn
		{
			draw_hud_backgrounds();
			draw_hud();
		}

		glutSwapBuffers();	// Swap the frame buffers

		glPopName();

		if (current_render_mode == GL_RENDER)	// This has to be called AFTER the frame buffer switch, else the main window 
		{										// wont be drawn at all!
			radar_display();
		}
	}
}

////////////////////////////////////////////////////////////////////
// RADAR DISPLAY FUNCTION
// ----------------------
//
// Draws the radar screen in the top left hand corner,
//
////////////////////////////////////////////////////////////////////

void radar_display (void)
{
	glutSetWindow(radar_window);	// Set the focus to the radar window

	glClearColor(0.75, 0.75, 0.75, 0.0);	// Clears the main window to a pleasent blue shade 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear the colour buffer bit

	glLoadIdentity();
	glRotatef( 90, 1.0, 0.0, 0.0 );			// Secondary rotation.
	glTranslatef(radar_cam.get_x(), radar_cam.get_y(), radar_cam.get_z());

	glDisable(GL_LIGHTING);			// Disable the lighting and fog
	glDisable(GL_FOG);

	radar_water();					// Draw the radar elements
	radar_trees();
	draw_terrain_polygons(NO);		// This draws the polygons without frustu culling, as they will get clipped.

	glDisable(GL_DEPTH_TEST);		// Disable depth testing

	radar_buildings();				// Draw more elements
	radar_banks();
	radar_units();
	radar_main_camera();

	glEnable(GL_DEPTH_TEST);		// Re-enable depth testing

	glutSwapBuffers();				// Swap buffers again
}

////////////////////////////////////////////////////////////////////
// RADAR BANKS FUNCTION
// --------------------
//
// Draws all the banks on the radar
//
////////////////////////////////////////////////////////////////////

void radar_banks (void)
{	
	int i;

	glPointSize(10);

	for (i=0; i<num_banks; i++)
	{
		if(banks[i].get_controller() == NEUTRAL)
		{
			glColor3f(1.0, 1.0, 1.0);
				open_gl.gl_draw_point(banks[i].get_x(), banks[i].get_y(), banks[i].get_z());
		}

		if(banks[i].get_controller() == PLAYER_ONE_HUMAN)
		{
			glColor3f(0.0, 0.0, 1.0);
				open_gl.gl_draw_point(banks[i].get_x(), banks[i].get_y(), banks[i].get_z());
		}

		if(banks[i].get_controller() == PLAYER_TWO_CPU)
		{
			glColor3f(1.0, 0.0, 0.0);
				open_gl.gl_draw_point(banks[i].get_x(), banks[i].get_y(), banks[i].get_z());
		}
	}
}

////////////////////////////////////////////////////////////////////
// RADAR MAIN CAMERA FUNCTION
// --------------------------
//
// Draws the box on the radar screen where the main world camera is
//
////////////////////////////////////////////////////////////////////

void radar_main_camera (void)
{
	open_gl.gl_draw_line(-open_gl.fp_x-100, 10, -open_gl.fp_z-90, 
							-open_gl.fp_x+100, 10, -open_gl.fp_z-90);

	open_gl.gl_draw_line(-open_gl.fp_x+100, 10, -open_gl.fp_z-90, 
							-open_gl.fp_x+100, 10, -open_gl.fp_z+90);

	open_gl.gl_draw_line(-open_gl.fp_x+100, 10, -open_gl.fp_z+90, 
							-open_gl.fp_x-100, 10, -open_gl.fp_z+90);

	open_gl.gl_draw_line(-open_gl.fp_x-100, 10, -open_gl.fp_z+90, 
							-open_gl.fp_x-100, 10, -open_gl.fp_z-90);
}

////////////////////////////////////////////////////////////////////
// RADAR TREES FUNCTION
// --------------------
//
// Draws the trees as white dots on radar
//
////////////////////////////////////////////////////////////////////

void radar_trees (void)
{
	int i, j;

	glPointSize(2);

	glColor3f(1.0, 1.0, 1.0);

	for (i=0; i<TERRAIN_SIZE; i++)
	{
		for (j=0; j<TERRAIN_SIZE; j++)
		{
			if (terrain[i][j].tree == 1)
			{
				open_gl.gl_draw_point(terrain[i][j].x, terrain[i][j].y+10 ,terrain[i][j].z);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
// RADAR UNITS FUNCTION
// --------------------
//
// Draws the units on the screen as blue and red dots.
//
////////////////////////////////////////////////////////////////////

void radar_units (void)
{
	int i;

	glPointSize(5);

	glColor3f(0.0, 0.0, 1.0);

	for (i=0; i<player_one.num_units; i++)
	{
		if (player_one.player_units[i].get_unit_id() != FREE)
		{
			open_gl.gl_draw_point(player_one.player_units[i].get_x(),
									player_one.player_units[i].get_y()+10, 
										player_one.player_units[i].get_z());
		}
	}

	glColor3f(1.0, 0.0, 0.0);

	for (i=0; i<player_two.num_units; i++)
	{
		if (player_two.player_units[i].get_unit_id() != FREE)
		{
			open_gl.gl_draw_point(player_two.player_units[i].get_x(),
									player_two.player_units[i].get_y()+10, 
										player_two.player_units[i].get_z());
		}
	}
}

////////////////////////////////////////////////////////////////////
// RADAR BUILDINGS FUNCTION
// ------------------------
//
// Draws the buildings on the radar as blue and red dots, larger than
// the units dots.
////////////////////////////////////////////////////////////////////

void radar_buildings (void)
{
	int i;

	glPointSize(10);

	glColor3f(0.0, 0.0, 1.0);
		open_gl.gl_draw_point(player_one.hq_info.get_x(), player_one.hq_info.get_y(), player_one.hq_info.get_z());

	for (i=0; i<player_one.player_buildings.num_of_buildings; i++)
	{
		if (player_one.player_buildings.buildings[i].get_id() != ID_FREE)
		{
			open_gl.gl_draw_point(player_one.player_buildings.buildings[i].get_x(),
									player_one.player_buildings.buildings[i].get_y()+10,
									  player_one.player_buildings.buildings[i].get_z());
		}
	}

	glColor3f(1.0, 0.0, 0.0);
		open_gl.gl_draw_point(player_two.hq_info.get_x(), player_two.hq_info.get_y(), player_two.hq_info.get_z());

	for (i=0; i<player_two.player_buildings.num_of_buildings; i++)
	{
		if (player_two.player_buildings.buildings[i].get_id() != ID_FREE)
		{
			open_gl.gl_draw_point(player_two.player_buildings.buildings[i].get_x(),
									player_two.player_buildings.buildings[i].get_y()+10,
									  player_two.player_buildings.buildings[i].get_z());
		}
	}
}

////////////////////////////////////////////////////////////////////
// RADAR WATER FUNCTION
// --------------------
//
// Draws the water plane on the radar.
//
////////////////////////////////////////////////////////////////////

void radar_water (void)
{
	glColor3f(0.0, 0.25, 0.5);

	glBegin(GL_QUADS);
		glNormal3f(1000, water_y, 1000);
		glVertex3f(1000, water_y, 1000);
		
		glNormal3f(1000, water_y, -1000);
		glVertex3f(1000, water_y, -1000);

		glNormal3f(-1000, water_y, -1000);
		glVertex3f(-1000, water_y, -1000);

		glNormal3f(-1000, water_y, 1000);
		glVertex3f(-1000, water_y, 1000);
	glEnd();
}

////////////////////////////////////////////////////////////////////
// DRAW BUILDING TO PLACE FUNCTION
// -------------------------------
//
// Draws the silhouette of the building that needs placing.
//
////////////////////////////////////////////////////////////////////

void draw_building_to_place (void)
{
	int vertex_one, vertex_two, vertex_three;
	int i;

	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLoadName(NON_SELECTABLE_OBJECTS);

	if (place_building.building_id == ID_BARRACKS)
	{
		glPushMatrix();
			glTranslatef(place_building.x, place_building.y, place_building.z);

			for (i=0; i<barracks.num_faces; i++)
			{
				vertex_one = barracks.faces[i].get_vertex_one();
				vertex_two = barracks.faces[i].get_vertex_two();
				vertex_three = barracks.faces[i].get_vertex_three();
									
				glColor4f(1.0, 1.0, 1.0, 0.5);

				glBegin(GL_TRIANGLES);
					glTexCoord2f(1.0f, 1.0f);
					glNormal3f(barracks.vertices[vertex_one].get_x(), barracks.vertices[vertex_one].get_y(), barracks.vertices[vertex_one].get_z());
					glVertex3f(barracks.vertices[vertex_one].get_x(), barracks.vertices[vertex_one].get_y(), barracks.vertices[vertex_one].get_z());

					glTexCoord2f(1.0f, 0.0f);
					glNormal3f(barracks.vertices[vertex_two].get_x(), barracks.vertices[vertex_two].get_y(), barracks.vertices[vertex_two].get_z());
					glVertex3f(barracks.vertices[vertex_two].get_x(), barracks.vertices[vertex_two].get_y(), barracks.vertices[vertex_two].get_z());

					glTexCoord2f(0.0f, 0.0f);
					glNormal3f(barracks.vertices[vertex_three].get_x(), barracks.vertices[vertex_three].get_y(), barracks.vertices[vertex_three].get_z());
					glVertex3f(barracks.vertices[vertex_three].get_x(), barracks.vertices[vertex_three].get_y(), barracks.vertices[vertex_three].get_z());
				glEnd();
			}

		glPopMatrix();
	
		glDisable(GL_BLEND);
	}

	if (place_building.building_id == ID_WAR_FACTORY)
	{
		glPushMatrix();
			glTranslatef(place_building.x, place_building.y, place_building.z);

			for (i=0; i<war_factory.num_faces; i++)
			{
				vertex_one = war_factory.faces[i].get_vertex_one();
				vertex_two = war_factory.faces[i].get_vertex_two();
				vertex_three = war_factory.faces[i].get_vertex_three();
									
				glColor4f(1.0, 1.0, 1.0, 0.5);

				glBegin(GL_TRIANGLES);
					glTexCoord2f(1.0f, 1.0f);
					glNormal3f(war_factory.vertices[vertex_one].get_x(), war_factory.vertices[vertex_one].get_y(), war_factory.vertices[vertex_one].get_z());
					glVertex3f(war_factory.vertices[vertex_one].get_x(), war_factory.vertices[vertex_one].get_y(), war_factory.vertices[vertex_one].get_z());

					glTexCoord2f(1.0f, 0.0f);
					glNormal3f(war_factory.vertices[vertex_two].get_x(), war_factory.vertices[vertex_two].get_y(), war_factory.vertices[vertex_two].get_z());
					glVertex3f(war_factory.vertices[vertex_two].get_x(), war_factory.vertices[vertex_two].get_y(), war_factory.vertices[vertex_two].get_z());

					glTexCoord2f(0.0f, 0.0f);
					glNormal3f(war_factory.vertices[vertex_three].get_x(), war_factory.vertices[vertex_three].get_y(), war_factory.vertices[vertex_three].get_z());
					glVertex3f(war_factory.vertices[vertex_three].get_x(), war_factory.vertices[vertex_three].get_y(), war_factory.vertices[vertex_three].get_z());
				glEnd();
			}

		glPopMatrix();
	
		glDisable(GL_BLEND);
	}

	if (place_building.building_id == ID_TOWER)
	{
		glPushMatrix();
			glTranslatef(place_building.x, place_building.y, place_building.z);

			for (i=0; i<tower.num_faces; i++)
			{
				vertex_one = tower.faces[i].get_vertex_one();
				vertex_two = tower.faces[i].get_vertex_two();
				vertex_three = tower.faces[i].get_vertex_three();
									
				glColor4f(1.0, 1.0, 1.0, 0.5);

				glBegin(GL_TRIANGLES);
					glTexCoord2f(1.0f, 1.0f);
					glNormal3f(tower.vertices[vertex_one].get_x(), tower.vertices[vertex_one].get_y(), tower.vertices[vertex_one].get_z());
					glVertex3f(tower.vertices[vertex_one].get_x(), tower.vertices[vertex_one].get_y(), tower.vertices[vertex_one].get_z());

					glTexCoord2f(1.0f, 0.0f);
					glNormal3f(tower.vertices[vertex_two].get_x(), tower.vertices[vertex_two].get_y(), tower.vertices[vertex_two].get_z());
					glVertex3f(tower.vertices[vertex_two].get_x(), tower.vertices[vertex_two].get_y(), tower.vertices[vertex_two].get_z());

					glTexCoord2f(0.0f, 0.0f);
					glNormal3f(tower.vertices[vertex_three].get_x(), tower.vertices[vertex_three].get_y(), tower.vertices[vertex_three].get_z());
					glVertex3f(tower.vertices[vertex_three].get_x(), tower.vertices[vertex_three].get_y(), tower.vertices[vertex_three].get_z());
				glEnd();
			}

		glPopMatrix();
	
		glDisable(GL_BLEND);
	}
}

////////////////////////////////////////////////////////////////////
// DRAW BUILDINGS FUNCTION
// -----------------------
//
// Draws the buildings for the main view
//
////////////////////////////////////////////////////////////////////

void draw_buildings (void)
{
	int vertex_one, vertex_two, vertex_three;
	int i, j, k;
	bool frustum_cull;

	for (i=0; i<player_one.player_buildings.num_of_buildings; i++)
	{
		glLoadName(NON_SELECTABLE_OBJECTS);

		if (player_one.player_buildings.buildings[i].get_id() == ID_BARRACKS)  
		{
			if ((frustum_cull = frustum.sphere_in_frustum(player_one.player_buildings.buildings[i].get_x(), player_one.player_buildings.buildings[i].get_y(), player_one.player_buildings.buildings[i].get_z(),  25)) == true)
			{
				glPushMatrix();
					glTranslatef(player_one.player_buildings.buildings[i].get_x(), player_one.player_buildings.buildings[i].get_y(), player_one.player_buildings.buildings[i].get_z());

					glEnable(GL_TEXTURE_2D);
					glCallList(open_gl.tex_player_one_barracks);

					for (k=0; k<barracks.num_faces; k++)
					{
						vertex_one = barracks.faces[k].get_vertex_one();
						vertex_two = barracks.faces[k].get_vertex_two();
						vertex_three = barracks.faces[k].get_vertex_three();
									
						glColor3f(1.0, 1.0, 1.0);

						glBegin(GL_TRIANGLES);
							set_texture_coord( barracks.vertices[vertex_one].get_x(), barracks.vertices[vertex_one].get_y(), barracks.vertices[vertex_one].get_z(), 32.1662 );
							glNormal3f(barracks.vertices[vertex_one].get_x(), barracks.vertices[vertex_one].get_y(), barracks.vertices[vertex_one].get_z());
							glVertex3f(barracks.vertices[vertex_one].get_x(), barracks.vertices[vertex_one].get_y(), barracks.vertices[vertex_one].get_z());

							set_texture_coord( barracks.vertices[vertex_two].get_x(), barracks.vertices[vertex_two].get_y(), barracks.vertices[vertex_two].get_z(), 32.1662 );
							glNormal3f(barracks.vertices[vertex_two].get_x(), barracks.vertices[vertex_two].get_y(), barracks.vertices[vertex_two].get_z());
							glVertex3f(barracks.vertices[vertex_two].get_x(), barracks.vertices[vertex_two].get_y(), barracks.vertices[vertex_two].get_z());

							set_texture_coord( barracks.vertices[vertex_three].get_x(), barracks.vertices[vertex_three].get_y(), barracks.vertices[vertex_three].get_z(), 32.1662 );
							glNormal3f(barracks.vertices[vertex_three].get_x(), barracks.vertices[vertex_three].get_y(), barracks.vertices[vertex_three].get_z());
							glVertex3f(barracks.vertices[vertex_three].get_x(), barracks.vertices[vertex_three].get_y(), barracks.vertices[vertex_three].get_z());
						glEnd();
					}

			glPopMatrix();

			glDisable(GL_TEXTURE_2D);
			}
		}

		if (player_one.player_buildings.buildings[i].get_id() == ID_WAR_FACTORY)  
		{
			if ((frustum_cull = frustum.sphere_in_frustum(player_one.player_buildings.buildings[i].get_x(), player_one.player_buildings.buildings[i].get_y(), player_one.player_buildings.buildings[i].get_z(),  25)) == true)
			{
				glPushMatrix();
					glTranslatef(player_one.player_buildings.buildings[i].get_x(), player_one.player_buildings.buildings[i].get_y(), player_one.player_buildings.buildings[i].get_z());

					glEnable(GL_TEXTURE_2D);
					glCallList(open_gl.tex_player_one_hq);

					for (k=0; k<war_factory.num_faces; k++)
					{
						vertex_one = war_factory.faces[k].get_vertex_one();
						vertex_two = war_factory.faces[k].get_vertex_two();
						vertex_three = war_factory.faces[k].get_vertex_three();
									
						glColor3f(1.0, 1.0, 1.0);

						glBegin(GL_TRIANGLES);
							set_texture_coord( war_factory.vertices[vertex_one].get_x() - 8.59026, war_factory.vertices[vertex_one].get_y(), war_factory.vertices[vertex_one].get_z(), 18.0741 );
							glNormal3f(war_factory.vertices[vertex_one].get_x(), war_factory.vertices[vertex_one].get_y(), war_factory.vertices[vertex_one].get_z());
							glVertex3f(war_factory.vertices[vertex_one].get_x(), war_factory.vertices[vertex_one].get_y(), war_factory.vertices[vertex_one].get_z());

							set_texture_coord( war_factory.vertices[vertex_two].get_x() - 8.59026, war_factory.vertices[vertex_two].get_y(), war_factory.vertices[vertex_two].get_z(), 18.0741 );
							glNormal3f(war_factory.vertices[vertex_two].get_x(), war_factory.vertices[vertex_two].get_y(), war_factory.vertices[vertex_two].get_z());
							glVertex3f(war_factory.vertices[vertex_two].get_x(), war_factory.vertices[vertex_two].get_y(), war_factory.vertices[vertex_two].get_z());

							set_texture_coord( war_factory.vertices[vertex_three].get_x() - 8.59026, war_factory.vertices[vertex_three].get_y(), war_factory.vertices[vertex_three].get_z(), 18.0741 );
							glNormal3f(war_factory.vertices[vertex_three].get_x(), war_factory.vertices[vertex_three].get_y(), war_factory.vertices[vertex_three].get_z());
							glVertex3f(war_factory.vertices[vertex_three].get_x(), war_factory.vertices[vertex_three].get_y(), war_factory.vertices[vertex_three].get_z());
						glEnd();
					}

			glPopMatrix();

			glDisable(GL_TEXTURE_2D);
			}
		}

		if (player_one.player_buildings.buildings[i].get_id() == ID_TOWER)  
		{
			if ((frustum_cull = frustum.sphere_in_frustum(player_one.player_buildings.buildings[i].get_x(), player_one.player_buildings.buildings[i].get_y(), player_one.player_buildings.buildings[i].get_z(),  25)) == true)
			{
				glPushMatrix();
					glTranslatef(player_one.player_buildings.buildings[i].get_x(), player_one.player_buildings.buildings[i].get_y(), player_one.player_buildings.buildings[i].get_z());

					glEnable(GL_TEXTURE_2D);
					glCallList(open_gl.tex_player_one_guard_tower);

					for (k=0; k<tower.num_faces; k++)
					{
						vertex_one = tower.faces[k].get_vertex_one();
						vertex_two = tower.faces[k].get_vertex_two();
						vertex_three = tower.faces[k].get_vertex_three();
									
						glColor3f(1.0, 1.0, 1.0);

						glBegin(GL_TRIANGLES);
							set_texture_coord( tower.vertices[vertex_one].get_x(), tower.vertices[vertex_one].get_y() - 28.8435, tower.vertices[vertex_one].get_z(), 54.6708 );
							glNormal3f(tower.vertices[vertex_one].get_x(), tower.vertices[vertex_one].get_y(), tower.vertices[vertex_one].get_z());
							glVertex3f(tower.vertices[vertex_one].get_x(), tower.vertices[vertex_one].get_y(), tower.vertices[vertex_one].get_z());

							set_texture_coord( tower.vertices[vertex_two].get_x(), tower.vertices[vertex_two].get_y() - 28.8435, tower.vertices[vertex_two].get_z(), 54.6708 );
							glNormal3f(tower.vertices[vertex_two].get_x(), tower.vertices[vertex_two].get_y(), tower.vertices[vertex_two].get_z());
							glVertex3f(tower.vertices[vertex_two].get_x(), tower.vertices[vertex_two].get_y(), tower.vertices[vertex_two].get_z());

							set_texture_coord( tower.vertices[vertex_three].get_x(), tower.vertices[vertex_three].get_y() - 28.8435, tower.vertices[vertex_three].get_z(), 54.6708 );
							glNormal3f(tower.vertices[vertex_three].get_x(), tower.vertices[vertex_three].get_y(), tower.vertices[vertex_three].get_z());
							glVertex3f(tower.vertices[vertex_three].get_x(), tower.vertices[vertex_three].get_y(), tower.vertices[vertex_three].get_z());
						glEnd();
					}

			glPopMatrix();

			glDisable(GL_TEXTURE_2D);
			}
		}
	}

	//+++++++++++++++++++++++++ DRAW PLAYER TWO STUFF +++++++++++++++++++++++++++++++++//

	glCallList(open_gl.tex_player_two);

	for (j=0; j<player_two.player_buildings.num_of_buildings; j++)
	{
		glLoadName(NON_SELECTABLE_OBJECTS);

		if (player_two.player_buildings.buildings[j].get_id() == ID_BARRACKS)  
		{
			if ((frustum_cull = frustum.sphere_in_frustum(player_two.player_buildings.buildings[j].get_x(), player_two.player_buildings.buildings[j].get_y(), player_two.player_buildings.buildings[j].get_z(),  25)) == true)
			{
				glPushMatrix();
					glTranslatef(player_two.player_buildings.buildings[j].get_x(), player_two.player_buildings.buildings[j].get_y(), player_two.player_buildings.buildings[j].get_z());

					glEnable(GL_TEXTURE_2D);
					glCallList(open_gl.tex_player_two_barracks);

					for (k=0; k<barracks.num_faces; k++)
					{
						vertex_one = barracks.faces[k].get_vertex_one();
						vertex_two = barracks.faces[k].get_vertex_two();
						vertex_three = barracks.faces[k].get_vertex_three();
									
						glColor3f(1.0, 1.0, 1.0);

						glBegin(GL_TRIANGLES);
							set_texture_coord( barracks.vertices[vertex_one].get_x(), barracks.vertices[vertex_one].get_y(), barracks.vertices[vertex_one].get_z(), 32.1662 );
							glNormal3f(barracks.vertices[vertex_one].get_x(), barracks.vertices[vertex_one].get_y(), barracks.vertices[vertex_one].get_z());
							glVertex3f(barracks.vertices[vertex_one].get_x(), barracks.vertices[vertex_one].get_y(), barracks.vertices[vertex_one].get_z());

							set_texture_coord( barracks.vertices[vertex_two].get_x(), barracks.vertices[vertex_two].get_y(), barracks.vertices[vertex_two].get_z(), 32.1662 );
							glNormal3f(barracks.vertices[vertex_two].get_x(), barracks.vertices[vertex_two].get_y(), barracks.vertices[vertex_two].get_z());
							glVertex3f(barracks.vertices[vertex_two].get_x(), barracks.vertices[vertex_two].get_y(), barracks.vertices[vertex_two].get_z());

							set_texture_coord( barracks.vertices[vertex_three].get_x(), barracks.vertices[vertex_three].get_y(), barracks.vertices[vertex_three].get_z(), 32.1662 );
							glNormal3f(barracks.vertices[vertex_three].get_x(), barracks.vertices[vertex_three].get_y(), barracks.vertices[vertex_three].get_z());
							glVertex3f(barracks.vertices[vertex_three].get_x(), barracks.vertices[vertex_three].get_y(), barracks.vertices[vertex_three].get_z());
						glEnd();
					}

			glPopMatrix();

			glDisable(GL_TEXTURE_2D);
			}
		}

		if (player_two.player_buildings.buildings[j].get_id() == ID_WAR_FACTORY)  
		{
			if ((frustum_cull = frustum.sphere_in_frustum(player_two.player_buildings.buildings[j].get_x(), player_two.player_buildings.buildings[j].get_y(), player_two.player_buildings.buildings[j].get_z(),  25)) == true)
			{
				glPushMatrix();
					glTranslatef(player_two.player_buildings.buildings[j].get_x(), player_two.player_buildings.buildings[j].get_y(), player_two.player_buildings.buildings[j].get_z());

					glEnable(GL_TEXTURE_2D);
					glCallList(open_gl.tex_player_two_hq);

					for (k=0; k<war_factory.num_faces; k++)
					{
						vertex_one = war_factory.faces[k].get_vertex_one();
						vertex_two = war_factory.faces[k].get_vertex_two();
						vertex_three = war_factory.faces[k].get_vertex_three();
									
						glColor3f(1.0, 1.0, 1.0);

						glBegin(GL_TRIANGLES);
							set_texture_coord( war_factory.vertices[vertex_one].get_x() - 8.59026, war_factory.vertices[vertex_one].get_y(), war_factory.vertices[vertex_one].get_z(), 18.0741 );
							glNormal3f(war_factory.vertices[vertex_one].get_x(), war_factory.vertices[vertex_one].get_y(), war_factory.vertices[vertex_one].get_z());
							glVertex3f(war_factory.vertices[vertex_one].get_x(), war_factory.vertices[vertex_one].get_y(), war_factory.vertices[vertex_one].get_z());

							set_texture_coord( war_factory.vertices[vertex_two].get_x() - 8.59026, war_factory.vertices[vertex_two].get_y(), war_factory.vertices[vertex_two].get_z(), 18.0741 );
							glNormal3f(war_factory.vertices[vertex_two].get_x(), war_factory.vertices[vertex_two].get_y(), war_factory.vertices[vertex_two].get_z());
							glVertex3f(war_factory.vertices[vertex_two].get_x(), war_factory.vertices[vertex_two].get_y(), war_factory.vertices[vertex_two].get_z());

							set_texture_coord( war_factory.vertices[vertex_three].get_x() - 8.59026, war_factory.vertices[vertex_three].get_y(), war_factory.vertices[vertex_three].get_z(), 18.0741 );
							glNormal3f(war_factory.vertices[vertex_three].get_x(), war_factory.vertices[vertex_three].get_y(), war_factory.vertices[vertex_three].get_z());
							glVertex3f(war_factory.vertices[vertex_three].get_x(), war_factory.vertices[vertex_three].get_y(), war_factory.vertices[vertex_three].get_z());
						glEnd();
					}

			glPopMatrix();

			glDisable(GL_TEXTURE_2D);
			}
		}

		if (player_two.player_buildings.buildings[j].get_id() == ID_TOWER)  
		{
			if ((frustum_cull = frustum.sphere_in_frustum(player_two.player_buildings.buildings[j].get_x(), player_two.player_buildings.buildings[j].get_y(), player_two.player_buildings.buildings[j].get_z(),  25)) == true)
			{
				glPushMatrix();
					glTranslatef(player_two.player_buildings.buildings[j].get_x(), player_two.player_buildings.buildings[j].get_y(), player_two.player_buildings.buildings[j].get_z());

					glEnable(GL_TEXTURE_2D);
					glCallList(open_gl.tex_player_two_guard_tower);

					for (k=0; k<tower.num_faces; k++)
					{
						vertex_one = tower.faces[k].get_vertex_one();
						vertex_two = tower.faces[k].get_vertex_two();
						vertex_three = tower.faces[k].get_vertex_three();
									
						glColor3f(1.0, 1.0, 1.0);

						glBegin(GL_TRIANGLES);
							set_texture_coord( tower.vertices[vertex_one].get_x(), tower.vertices[vertex_one].get_y() - 28.8435, tower.vertices[vertex_one].get_z(), 54.6708 );
							glNormal3f(tower.vertices[vertex_one].get_x(), tower.vertices[vertex_one].get_y(), tower.vertices[vertex_one].get_z());
							glVertex3f(tower.vertices[vertex_one].get_x(), tower.vertices[vertex_one].get_y(), tower.vertices[vertex_one].get_z());

							set_texture_coord( tower.vertices[vertex_two].get_x(), tower.vertices[vertex_two].get_y() - 28.8435, tower.vertices[vertex_two].get_z(), 54.6708 );
							glNormal3f(tower.vertices[vertex_two].get_x(), tower.vertices[vertex_two].get_y(), tower.vertices[vertex_two].get_z());
							glVertex3f(tower.vertices[vertex_two].get_x(), tower.vertices[vertex_two].get_y(), tower.vertices[vertex_two].get_z());

							set_texture_coord( tower.vertices[vertex_three].get_x(), tower.vertices[vertex_three].get_y() - 28.8435, tower.vertices[vertex_three].get_z(), 54.6708 );
							glNormal3f(tower.vertices[vertex_three].get_x(), tower.vertices[vertex_three].get_y(), tower.vertices[vertex_three].get_z());
							glVertex3f(tower.vertices[vertex_three].get_x(), tower.vertices[vertex_three].get_y(), tower.vertices[vertex_three].get_z());
						glEnd();
					}

			glPopMatrix();

			glDisable(GL_TEXTURE_2D);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
// PARTICLE SYSTEMS FUNCTION
// -------------------------
//
// Draws the particles in the particle engine
//
////////////////////////////////////////////////////////////////////

void particle_systems (void)
{
	int i;
	bool frustum_cull;

//	glEnable(GL_BLEND);											// All of the commented code here was used to render a masked
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);			// star like image for each particle.
//	glBlendFunc(GL_DST_COLOR,GL_ZERO); // USE FOR MASKING

//	glCallList(open_gl.tex_smoke);

	glPointSize(4);

	for (i=0; i<NUM_PARTICLE; i++)
	{
		if ((frustum_cull = frustum.point_in_frustum(player_two.hq_info.get_x() +cos(i)*3, particle_y[i], player_two.hq_info.get_z())) == true)
		{
			glColor3f(1.0, 0.0, 0.0);

			open_gl.gl_draw_point(player_two.hq_info.get_x() + cos(i)*3, particle_y[i], player_two.hq_info.get_z() + sin(i)*3);
			open_gl.gl_draw_point(player_two.hq_info.get_x() + -cos(i)*3, particle_y[i], player_two.hq_info.get_z() + -sin(i)*3);

//			open_gl.gl_draw_textured_polygon(player_two.hq_info.get_x() + cos(i)*3, particle_y[i], player_two.hq_info.get_z() + sin(i)*3,
//											 player_two.hq_info.get_x() + cos(i)*3, particle_y[i], player_two.hq_info.get_z() + -sin(i)*3,
//											 player_two.hq_info.get_x() + -cos(i)*3, particle_y[i], player_two.hq_info.get_z() + -sin(i)*3,
//											 player_two.hq_info.get_x() + -cos(i)*3, particle_y[i], player_two.hq_info.get_z() + sin(i)*3, 1);
		}

		if ((frustum_cull = frustum.point_in_frustum(player_one.hq_info.get_x() + i-5, particle_y[i], player_one.hq_info.get_z() + i-5)) == true)
		{
			glColor3f(0.0, 0.0, 1.0);

			open_gl.gl_draw_point(player_one.hq_info.get_x() + cos(i)*3, particle_y[i], player_one.hq_info.get_z() + sin(i)*3);
			open_gl.gl_draw_point(player_one.hq_info.get_x() + -cos(i)*3, particle_y[i], player_one.hq_info.get_z() + -sin(i)*3);
		}
	}
}

////////////////////////////////////////////////////////////////////
// DEAL_WITH_CURSOR_AT_EDGES FUNCTION
// ----------------------------------
//
// Moves the camera 
////////////////////////////////////////////////////////////////////

void deal_with_cursor_at_edges (void)
{
	if (p_x < 10)
	{
		open_gl.fp_x += 10;
	}
/*									// This causes the screen to scroll to the right
	if (p_x > fullscreen_x -200)	// but has been commented out as it interferes 
	{								// with the toolbar.
		open_gl.fp_x -= 10;
	}
*/
	if (p_y < 10)
	{
		open_gl.fp_z += 10;
	}

	if (p_y > fullscreen_y -10)
	{
		open_gl.fp_z -= 10;
	}

	if (open_gl.fp_x < -600)
	{
		open_gl.fp_x = -600;
	}

	else if (open_gl.fp_x > 625)
	{
		open_gl.fp_x = 625;
	}

	if (open_gl.fp_z < -625)
	{
		open_gl.fp_z = -625;
	}

	else if (open_gl.fp_z > 800)
	{
		open_gl.fp_z = 800;
	}

	frustum.calculate_frustum();	// Recalculate the clipping frustum
}

////////////////////////////////////////////////////////////////////
// DRAW UNITS FUNCTION
// -------------------
//
// Draws both sides units.
//
////////////////////////////////////////////////////////////////////

void draw_units (void)
{
	int vertex_one, vertex_two, vertex_three;
	int i, k;
	bool frustum_cull;

	glEnable(GL_LIGHTING);

	for (i=0; i<100; i++)
	{
		if (player_one.player_units[i].get_unit_id() == TANK)
		{
			if ((frustum_cull = frustum.sphere_in_frustum(player_one.player_units[i].get_x(), player_one.player_units[i].get_y(), player_one.player_units[i].get_z(), 20)) == true)
			{
				glPushMatrix();
					glTranslatef(player_one.player_units[i].get_x(), player_one.player_units[i].get_y(), player_one.player_units[i].get_z());
					glRotatef(player_one.player_units[i].get_orientation(), 0, 1, 0);
	
					glLoadName(i);
					glEnable(GL_TEXTURE_2D);
					glCallList(open_gl.tex_player_one_tank);

					for (k=0; k<tank.num_faces; k++)
					{
						vertex_one = tank.faces[k].get_vertex_one();
						vertex_two = tank.faces[k].get_vertex_two();
						vertex_three = tank.faces[k].get_vertex_three();
							
						glColor3f(1.0, 1.0, 1.0);

						glBegin(GL_TRIANGLES);
							set_texture_coord( tank.vertices[vertex_one].get_x() + 4.42293, tank.vertices[vertex_one].get_y(), tank.vertices[vertex_one].get_z(), 8.67828 );
							glNormal3f(tank.vertices[vertex_one].get_x(), tank.vertices[vertex_one].get_y(), tank.vertices[vertex_one].get_z());
							glVertex3f(tank.vertices[vertex_one].get_x(), tank.vertices[vertex_one].get_y(), tank.vertices[vertex_one].get_z());

							set_texture_coord( tank.vertices[vertex_two].get_x() + 4.42293, tank.vertices[vertex_two].get_y(), tank.vertices[vertex_two].get_z(), 8.67828 );
							glNormal3f(tank.vertices[vertex_two].get_x(), tank.vertices[vertex_two].get_y(), tank.vertices[vertex_two].get_z());
							glVertex3f(tank.vertices[vertex_two].get_x(), tank.vertices[vertex_two].get_y(), tank.vertices[vertex_two].get_z());

							set_texture_coord( tank.vertices[vertex_three].get_x() + 4.42293, tank.vertices[vertex_three].get_y(), tank.vertices[vertex_three].get_z(), 8.67828 );
							glNormal3f(tank.vertices[vertex_three].get_x(), tank.vertices[vertex_three].get_y(), tank.vertices[vertex_three].get_z());
							glVertex3f(tank.vertices[vertex_three].get_x(), tank.vertices[vertex_three].get_y(), tank.vertices[vertex_three].get_z());
						glEnd();	
					}
	
					glEnd();	//end load name
					glPopName();

					glDisable(GL_TEXTURE_2D);

					if (i == selected_unit) // Draw the bounding box if this is the selected unit.
					{
						//	glVertex3f(tank.bounding_box[k].get_x(), tank.bounding_box[k].get_y(), tank.bounding_box[k].get_z());
							open_gl.gl_draw_line(tank.bounding_box[0].get_x(), tank.bounding_box[0].get_y(), tank.bounding_box[0].get_z(),
												 tank.bounding_box[7].get_x(), tank.bounding_box[7].get_y(), tank.bounding_box[7].get_z());

							open_gl.gl_draw_line(tank.bounding_box[7].get_x(), tank.bounding_box[7].get_y(), tank.bounding_box[7].get_z(),
												 tank.bounding_box[3].get_x(), tank.bounding_box[3].get_y(), tank.bounding_box[3].get_z());

							open_gl.gl_draw_line(tank.bounding_box[3].get_x(), tank.bounding_box[3].get_y(), tank.bounding_box[3].get_z(),
												 tank.bounding_box[5].get_x(), tank.bounding_box[5].get_y(), tank.bounding_box[5].get_z());

							open_gl.gl_draw_line(tank.bounding_box[5].get_x(), tank.bounding_box[5].get_y(), tank.bounding_box[5].get_z(),
												 tank.bounding_box[0].get_x(), tank.bounding_box[0].get_y(), tank.bounding_box[0].get_z());

							open_gl.gl_draw_line(tank.bounding_box[1].get_x(), tank.bounding_box[1].get_y(), tank.bounding_box[1].get_z(),
												 tank.bounding_box[2].get_x(), tank.bounding_box[2].get_y(), tank.bounding_box[2].get_z());

							open_gl.gl_draw_line(tank.bounding_box[2].get_x(), tank.bounding_box[2].get_y(), tank.bounding_box[2].get_z(),
												 tank.bounding_box[6].get_x(), tank.bounding_box[6].get_y(), tank.bounding_box[6].get_z());

							open_gl.gl_draw_line(tank.bounding_box[6].get_x(), tank.bounding_box[6].get_y(), tank.bounding_box[6].get_z(),
												 tank.bounding_box[4].get_x(), tank.bounding_box[4].get_y(), tank.bounding_box[4].get_z());

							open_gl.gl_draw_line(tank.bounding_box[4].get_x(), tank.bounding_box[4].get_y(), tank.bounding_box[4].get_z(),
												 tank.bounding_box[1].get_x(), tank.bounding_box[1].get_y(), tank.bounding_box[1].get_z());

							open_gl.gl_draw_line(tank.bounding_box[0].get_x(), tank.bounding_box[0].get_y(), tank.bounding_box[0].get_z(),
												 tank.bounding_box[6].get_x(), tank.bounding_box[6].get_y(), tank.bounding_box[6].get_z());

							open_gl.gl_draw_line(tank.bounding_box[7].get_x(), tank.bounding_box[7].get_y(), tank.bounding_box[7].get_z(),
												 tank.bounding_box[2].get_x(), tank.bounding_box[2].get_y(), tank.bounding_box[2].get_z());

							open_gl.gl_draw_line(tank.bounding_box[1].get_x(), tank.bounding_box[1].get_y(), tank.bounding_box[1].get_z(),
												 tank.bounding_box[3].get_x(), tank.bounding_box[3].get_y(), tank.bounding_box[3].get_z());

							open_gl.gl_draw_line(tank.bounding_box[4].get_x(), tank.bounding_box[4].get_y(), tank.bounding_box[4].get_z(),
												 tank.bounding_box[5].get_x(), tank.bounding_box[5].get_y(), tank.bounding_box[5].get_z());
					}

				glPopMatrix();
			}
		}

		if (player_one.player_units[i].get_unit_id() == ARTILLERY)
		{
			if ((frustum_cull = frustum.sphere_in_frustum(player_one.player_units[i].get_x(), player_one.player_units[i].get_y(), player_one.player_units[i].get_z(), 20)) == true)
			{
	

				glPushMatrix();
					glTranslatef(player_one.player_units[i].get_x(), player_one.player_units[i].get_y(), player_one.player_units[i].get_z());
					glRotatef(player_one.player_units[i].get_orientation(), 0, 1, 0);

					glLoadName(i);
					glEnable(GL_TEXTURE_2D);
					glCallList(open_gl.tex_player_one_tank);

					for (k=0; k<artillery.num_faces; k++)
					{
						vertex_one = artillery.faces[k].get_vertex_one();
						vertex_two = artillery.faces[k].get_vertex_two();
						vertex_three = artillery.faces[k].get_vertex_three();
							
						glColor3f(1.0, 1.0, 1.0);

						glBegin(GL_TRIANGLES);
							set_texture_coord( artillery.vertices[vertex_one].get_x(), artillery.vertices[vertex_one].get_y() + 9.24868, artillery.vertices[vertex_one].get_z(), 5.07129 );
							glNormal3f(artillery.vertices[vertex_one].get_x(), artillery.vertices[vertex_one].get_y(), artillery.vertices[vertex_one].get_z());
							glVertex3f(artillery.vertices[vertex_one].get_x(), artillery.vertices[vertex_one].get_y(), artillery.vertices[vertex_one].get_z());

							set_texture_coord( artillery.vertices[vertex_two].get_x(), artillery.vertices[vertex_two].get_y() + 9.24868, artillery.vertices[vertex_two].get_z(), 5.07129 );
							glNormal3f(artillery.vertices[vertex_two].get_x(), artillery.vertices[vertex_two].get_y(), artillery.vertices[vertex_two].get_z());
							glVertex3f(artillery.vertices[vertex_two].get_x(), artillery.vertices[vertex_two].get_y(), artillery.vertices[vertex_two].get_z());

							set_texture_coord( artillery.vertices[vertex_three].get_x(), artillery.vertices[vertex_three].get_y() + 9.24868, artillery.vertices[vertex_three].get_z(), 5.07129 );
							glNormal3f(artillery.vertices[vertex_three].get_x(), artillery.vertices[vertex_three].get_y(), artillery.vertices[vertex_three].get_z());
							glVertex3f(artillery.vertices[vertex_three].get_x(), artillery.vertices[vertex_three].get_y(), artillery.vertices[vertex_three].get_z());
						glEnd();	
					}

					glEnd();	//end load name
					glPopName();

					glDisable(GL_TEXTURE_2D);

					if (i == selected_unit)
					{
						//	glVertex3f(artillery.bounding_box[k].get_x(), artillery.bounding_box[k].get_y(), artillery.bounding_box[k].get_z());
							open_gl.gl_draw_line(artillery.bounding_box[0].get_x(), artillery.bounding_box[0].get_y(), artillery.bounding_box[0].get_z(),
												 artillery.bounding_box[7].get_x(), artillery.bounding_box[7].get_y(), artillery.bounding_box[7].get_z());

							open_gl.gl_draw_line(artillery.bounding_box[7].get_x(), artillery.bounding_box[7].get_y(), artillery.bounding_box[7].get_z(),
												 artillery.bounding_box[3].get_x(), artillery.bounding_box[3].get_y(), artillery.bounding_box[3].get_z());

							open_gl.gl_draw_line(artillery.bounding_box[3].get_x(), artillery.bounding_box[3].get_y(), artillery.bounding_box[3].get_z(),
												 artillery.bounding_box[5].get_x(), artillery.bounding_box[5].get_y(), artillery.bounding_box[5].get_z());

							open_gl.gl_draw_line(artillery.bounding_box[5].get_x(), artillery.bounding_box[5].get_y(), artillery.bounding_box[5].get_z(),
												 artillery.bounding_box[0].get_x(), artillery.bounding_box[0].get_y(), artillery.bounding_box[0].get_z());

							open_gl.gl_draw_line(artillery.bounding_box[1].get_x(), artillery.bounding_box[1].get_y(), artillery.bounding_box[1].get_z(),
												 artillery.bounding_box[2].get_x(), artillery.bounding_box[2].get_y(), artillery.bounding_box[2].get_z());

							open_gl.gl_draw_line(artillery.bounding_box[2].get_x(), artillery.bounding_box[2].get_y(), artillery.bounding_box[2].get_z(),
												 artillery.bounding_box[6].get_x(), artillery.bounding_box[6].get_y(), artillery.bounding_box[6].get_z());

							open_gl.gl_draw_line(artillery.bounding_box[6].get_x(), artillery.bounding_box[6].get_y(), artillery.bounding_box[6].get_z(),
												 artillery.bounding_box[4].get_x(), artillery.bounding_box[4].get_y(), artillery.bounding_box[4].get_z());

							open_gl.gl_draw_line(artillery.bounding_box[4].get_x(), artillery.bounding_box[4].get_y(), artillery.bounding_box[4].get_z(),
												 artillery.bounding_box[1].get_x(), artillery.bounding_box[1].get_y(), artillery.bounding_box[1].get_z());

							open_gl.gl_draw_line(artillery.bounding_box[0].get_x(), artillery.bounding_box[0].get_y(), artillery.bounding_box[0].get_z(),
												 artillery.bounding_box[6].get_x(), artillery.bounding_box[6].get_y(), artillery.bounding_box[6].get_z());

							open_gl.gl_draw_line(artillery.bounding_box[7].get_x(), artillery.bounding_box[7].get_y(), artillery.bounding_box[7].get_z(),
												 artillery.bounding_box[2].get_x(), artillery.bounding_box[2].get_y(), artillery.bounding_box[2].get_z());

							open_gl.gl_draw_line(artillery.bounding_box[1].get_x(), artillery.bounding_box[1].get_y(), artillery.bounding_box[1].get_z(),
												 artillery.bounding_box[3].get_x(), artillery.bounding_box[3].get_y(), artillery.bounding_box[3].get_z());

							open_gl.gl_draw_line(artillery.bounding_box[4].get_x(), artillery.bounding_box[4].get_y(), artillery.bounding_box[4].get_z(),
												 artillery.bounding_box[5].get_x(), artillery.bounding_box[5].get_y(), artillery.bounding_box[5].get_z());
					}

				glPopMatrix();
			}
		}

		if (player_one.player_units[i].get_unit_id() == TROOPER)
		{
			if ((frustum_cull = frustum.sphere_in_frustum(player_one.player_units[i].get_x(), player_one.player_units[i].get_y(), player_one.player_units[i].get_z(), 20)) == true)
			{
				glCallList(open_gl.tex_player_one_trooper);

				glPushMatrix();
					glTranslatef(player_two.player_units[i].get_x(), player_two.player_units[i].get_y(), player_two.player_units[i].get_z());
					glRotatef(player_two.player_units[i].get_orientation(), 0, 1, 0);

					glEnable(GL_TEXTURE_2D);

					for (k=0; k<trooper.num_faces; k++)
					{
						vertex_one = trooper.faces[k].get_vertex_one();
						vertex_two = trooper.faces[k].get_vertex_two();
						vertex_three = trooper.faces[k].get_vertex_three();
							
						glColor3f(1.0, 1.0, 1.0);

						glBegin(GL_TRIANGLES);
							set_texture_coord( trooper.vertices[vertex_one].get_x() - 3.93074, trooper.vertices[vertex_one].get_y(), trooper.vertices[vertex_one].get_z(), 7.95718 );
							glNormal3f(trooper.vertices[vertex_one].get_x(), trooper.vertices[vertex_one].get_y(), trooper.vertices[vertex_one].get_z());
							glVertex3f(trooper.vertices[vertex_one].get_x(), trooper.vertices[vertex_one].get_y(), trooper.vertices[vertex_one].get_z());

							set_texture_coord( trooper.vertices[vertex_two].get_x() - 3.93074, trooper.vertices[vertex_two].get_y(), trooper.vertices[vertex_two].get_z(),  7.95718  );
							glNormal3f(trooper.vertices[vertex_two].get_x(), trooper.vertices[vertex_two].get_y(), trooper.vertices[vertex_two].get_z());
							glVertex3f(trooper.vertices[vertex_two].get_x(), trooper.vertices[vertex_two].get_y(), trooper.vertices[vertex_two].get_z());

							set_texture_coord( trooper.vertices[vertex_three].get_x() - 3.93074, trooper.vertices[vertex_three].get_y(), trooper.vertices[vertex_three].get_z(),  7.95718  );
							glNormal3f(trooper.vertices[vertex_three].get_x(), trooper.vertices[vertex_three].get_y(), trooper.vertices[vertex_three].get_z());
							glVertex3f(trooper.vertices[vertex_three].get_x(), trooper.vertices[vertex_three].get_y(), trooper.vertices[vertex_three].get_z());
						glEnd();	
					}

					glEnd();	//end load name
					glPopName();

					glDisable(GL_TEXTURE_2D);

					if (i == selected_unit)
					{
						//	glVertex3f(trooper.bounding_box[k].get_x(), trooper.bounding_box[k].get_y(), trooper.bounding_box[k].get_z());
							open_gl.gl_draw_line(trooper.bounding_box[0].get_x(), trooper.bounding_box[0].get_y(), trooper.bounding_box[0].get_z(),
												 trooper.bounding_box[7].get_x(), trooper.bounding_box[7].get_y(), trooper.bounding_box[7].get_z());

							open_gl.gl_draw_line(trooper.bounding_box[7].get_x(), trooper.bounding_box[7].get_y(), trooper.bounding_box[7].get_z(),
												 trooper.bounding_box[3].get_x(), trooper.bounding_box[3].get_y(), trooper.bounding_box[3].get_z());

							open_gl.gl_draw_line(trooper.bounding_box[3].get_x(), trooper.bounding_box[3].get_y(), trooper.bounding_box[3].get_z(),
												 trooper.bounding_box[5].get_x(), trooper.bounding_box[5].get_y(), trooper.bounding_box[5].get_z());

							open_gl.gl_draw_line(trooper.bounding_box[5].get_x(), trooper.bounding_box[5].get_y(), trooper.bounding_box[5].get_z(),
												 trooper.bounding_box[0].get_x(), trooper.bounding_box[0].get_y(), trooper.bounding_box[0].get_z());

							open_gl.gl_draw_line(trooper.bounding_box[1].get_x(), trooper.bounding_box[1].get_y(), trooper.bounding_box[1].get_z(),
												 trooper.bounding_box[2].get_x(), trooper.bounding_box[2].get_y(), trooper.bounding_box[2].get_z());

							open_gl.gl_draw_line(trooper.bounding_box[2].get_x(), trooper.bounding_box[2].get_y(), trooper.bounding_box[2].get_z(),
												 trooper.bounding_box[6].get_x(), trooper.bounding_box[6].get_y(), trooper.bounding_box[6].get_z());

							open_gl.gl_draw_line(trooper.bounding_box[6].get_x(), trooper.bounding_box[6].get_y(), trooper.bounding_box[6].get_z(),
												 trooper.bounding_box[4].get_x(), trooper.bounding_box[4].get_y(), trooper.bounding_box[4].get_z());

							open_gl.gl_draw_line(trooper.bounding_box[4].get_x(), trooper.bounding_box[4].get_y(), trooper.bounding_box[4].get_z(),
												 trooper.bounding_box[1].get_x(), trooper.bounding_box[1].get_y(), trooper.bounding_box[1].get_z());

							open_gl.gl_draw_line(trooper.bounding_box[0].get_x(), trooper.bounding_box[0].get_y(), trooper.bounding_box[0].get_z(),
												 trooper.bounding_box[6].get_x(), trooper.bounding_box[6].get_y(), trooper.bounding_box[6].get_z());

							open_gl.gl_draw_line(trooper.bounding_box[7].get_x(), trooper.bounding_box[7].get_y(), trooper.bounding_box[7].get_z(),
												 trooper.bounding_box[2].get_x(), trooper.bounding_box[2].get_y(), trooper.bounding_box[2].get_z());

							open_gl.gl_draw_line(trooper.bounding_box[1].get_x(), trooper.bounding_box[1].get_y(), trooper.bounding_box[1].get_z(),
												 trooper.bounding_box[3].get_x(), trooper.bounding_box[3].get_y(), trooper.bounding_box[3].get_z());

							open_gl.gl_draw_line(trooper.bounding_box[4].get_x(), trooper.bounding_box[4].get_y(), trooper.bounding_box[4].get_z(),
												 trooper.bounding_box[5].get_x(), trooper.bounding_box[5].get_y(), trooper.bounding_box[5].get_z());
					}

				glPopMatrix();
			}
		}

		if (player_two.player_units[i].get_unit_id() == TANK)
		{
			if ((frustum_cull = frustum.sphere_in_frustum(player_two.player_units[i].get_x(), player_two.player_units[i].get_y(), player_two.player_units[i].get_z(), 20)) == true)
			{
				glCallList(open_gl.tex_player_two);

				glPushMatrix();
					glTranslatef(player_two.player_units[i].get_x(), player_two.player_units[i].get_y(), player_two.player_units[i].get_z());
					glRotatef(player_two.player_units[i].get_orientation(), 0, 1, 0);

					glEnable(GL_TEXTURE_2D);
					glCallList(open_gl.tex_player_two_tank);

					for (k=0; k<tank.num_faces; k++)
					{
						vertex_one = tank.faces[k].get_vertex_one();
						vertex_two = tank.faces[k].get_vertex_two();
						vertex_three = tank.faces[k].get_vertex_three();
							
						glColor3f(1.0, 1.0, 1.0);

						glBegin(GL_TRIANGLES);
							set_texture_coord( tank.vertices[vertex_one].get_x() + 4.42293, tank.vertices[vertex_one].get_y(), tank.vertices[vertex_one].get_z(), 8.67828 );
							glNormal3f(tank.vertices[vertex_one].get_x(), tank.vertices[vertex_one].get_y(), tank.vertices[vertex_one].get_z());
							glVertex3f(tank.vertices[vertex_one].get_x(), tank.vertices[vertex_one].get_y(), tank.vertices[vertex_one].get_z());

							set_texture_coord( tank.vertices[vertex_two].get_x() + 4.42293, tank.vertices[vertex_two].get_y(), tank.vertices[vertex_two].get_z(), 8.67828 );
							glNormal3f(tank.vertices[vertex_two].get_x(), tank.vertices[vertex_two].get_y(), tank.vertices[vertex_two].get_z());
							glVertex3f(tank.vertices[vertex_two].get_x(), tank.vertices[vertex_two].get_y(), tank.vertices[vertex_two].get_z());

							set_texture_coord( tank.vertices[vertex_three].get_x() + 4.42293, tank.vertices[vertex_three].get_y(), tank.vertices[vertex_three].get_z(), 8.67828 );
							glNormal3f(tank.vertices[vertex_three].get_x(), tank.vertices[vertex_three].get_y(), tank.vertices[vertex_three].get_z());
							glVertex3f(tank.vertices[vertex_three].get_x(), tank.vertices[vertex_three].get_y(), tank.vertices[vertex_three].get_z());
						glEnd();
					}

				glPopMatrix();

				glDisable(GL_TEXTURE_2D);
			}
		}

		if (player_two.player_units[i].get_unit_id() == ARTILLERY)
		{
			if ((frustum_cull = frustum.sphere_in_frustum(player_two.player_units[i].get_x(), player_two.player_units[i].get_y(), player_two.player_units[i].get_z(), 20)) == true)
			{
				glCallList(open_gl.tex_player_two);

				glPushMatrix();
					glTranslatef(player_two.player_units[i].get_x(), player_two.player_units[i].get_y(), player_two.player_units[i].get_z());
					glRotatef(player_two.player_units[i].get_orientation(), 0, 1, 0);

					glEnable(GL_TEXTURE_2D);
					glCallList(open_gl.tex_player_two_tank);

					for (k=0; k<artillery.num_faces; k++)
					{
						vertex_one = artillery.faces[k].get_vertex_one();
						vertex_two = artillery.faces[k].get_vertex_two();
						vertex_three = artillery.faces[k].get_vertex_three();
							
						glColor3f(1.0, 1.0, 1.0);

						glBegin(GL_TRIANGLES);
							set_texture_coord( artillery.vertices[vertex_one].get_x(), artillery.vertices[vertex_one].get_y() + 9.24868, artillery.vertices[vertex_one].get_z(), 5.07129 );
							glNormal3f(artillery.vertices[vertex_one].get_x(), artillery.vertices[vertex_one].get_y(), artillery.vertices[vertex_one].get_z());
							glVertex3f(artillery.vertices[vertex_one].get_x(), artillery.vertices[vertex_one].get_y(), artillery.vertices[vertex_one].get_z());

							set_texture_coord( artillery.vertices[vertex_two].get_x(), artillery.vertices[vertex_two].get_y() + 9.24868, artillery.vertices[vertex_two].get_z(), 5.07129 );
							glNormal3f(artillery.vertices[vertex_two].get_x(), artillery.vertices[vertex_two].get_y(), artillery.vertices[vertex_two].get_z());
							glVertex3f(artillery.vertices[vertex_two].get_x(), artillery.vertices[vertex_two].get_y(), artillery.vertices[vertex_two].get_z());

							set_texture_coord( artillery.vertices[vertex_three].get_x(), artillery.vertices[vertex_three].get_y() + 9.24868, artillery.vertices[vertex_three].get_z(), 5.07129 );
							glNormal3f(artillery.vertices[vertex_three].get_x(), artillery.vertices[vertex_three].get_y(), artillery.vertices[vertex_three].get_z());
							glVertex3f(artillery.vertices[vertex_three].get_x(), artillery.vertices[vertex_three].get_y(), artillery.vertices[vertex_three].get_z());
						glEnd();
					}

				glPopMatrix();

				glDisable(GL_TEXTURE_2D);
			}
		}

		if (player_two.player_units[i].get_unit_id() == TROOPER)
		{
			if ((frustum_cull = frustum.sphere_in_frustum(player_two.player_units[i].get_x(), player_two.player_units[i].get_y(), player_two.player_units[i].get_z(), 20)) == true)
			{
				glCallList(open_gl.tex_player_two_trooper);

				glPushMatrix();
					glTranslatef(player_two.player_units[i].get_x(), player_two.player_units[i].get_y(), player_two.player_units[i].get_z());
					glRotatef(player_two.player_units[i].get_orientation(), 0, 1, 0);

					glEnable(GL_TEXTURE_2D);

					for (k=0; k<trooper.num_faces; k++)
					{
						vertex_one = trooper.faces[k].get_vertex_one();
						vertex_two = trooper.faces[k].get_vertex_two();
						vertex_three = trooper.faces[k].get_vertex_three();
							
						glColor3f(1.0, 1.0, 1.0);

						glBegin(GL_TRIANGLES);
							set_texture_coord( trooper.vertices[vertex_one].get_x() - 3.93074, trooper.vertices[vertex_one].get_y(), trooper.vertices[vertex_one].get_z(), 7.95718 );
							glNormal3f(trooper.vertices[vertex_one].get_x(), trooper.vertices[vertex_one].get_y(), trooper.vertices[vertex_one].get_z());
							glVertex3f(trooper.vertices[vertex_one].get_x(), trooper.vertices[vertex_one].get_y(), trooper.vertices[vertex_one].get_z());

							set_texture_coord( trooper.vertices[vertex_two].get_x() - 3.93074, trooper.vertices[vertex_two].get_y(), trooper.vertices[vertex_two].get_z(),  7.95718  );
							glNormal3f(trooper.vertices[vertex_two].get_x(), trooper.vertices[vertex_two].get_y(), trooper.vertices[vertex_two].get_z());
							glVertex3f(trooper.vertices[vertex_two].get_x(), trooper.vertices[vertex_two].get_y(), trooper.vertices[vertex_two].get_z());

							set_texture_coord( trooper.vertices[vertex_three].get_x() - 3.93074, trooper.vertices[vertex_three].get_y(), trooper.vertices[vertex_three].get_z(),  7.95718  );
							glNormal3f(trooper.vertices[vertex_three].get_x(), trooper.vertices[vertex_three].get_y(), trooper.vertices[vertex_three].get_z());
							glVertex3f(trooper.vertices[vertex_three].get_x(), trooper.vertices[vertex_three].get_y(), trooper.vertices[vertex_three].get_z());
						glEnd();
					}

				glPopMatrix();

				glDisable(GL_TEXTURE_2D);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
// DISTANCE_BETWEEN_TWO_VECTORS FUNCTION
// -------------------------------------
//
// This equation provides the distance between two position vectors
// or points.
//
////////////////////////////////////////////////////////////////////

float distance_between_two_vectors(float x, float y, float z, float x2, float y2, float z2)
{
	float distance;
	float dx = x - x2;
//	float dy = y * y2;
	float dz = z - z2;

//	distance =  (x - x2) + (y - y2) + (z - z2);
	distance = sqrt((dx*dx) + (dz*dz));

/*	if (distance < 0)
	{
		distance *= -1;
	}
*/
	return distance;
}

////////////////////////////////////////////////////////////////////
// DRAW_HUD FUNCTION
// -----------------
//
// Draws the HUD on the screen
//
////////////////////////////////////////////////////////////////////

void draw_hud (void)
{
//	sprintf(text_buffer,"%.0f",frames_per_second);
//	open_gl.gl_text_renderer(10,YPIXELS - 20,(void *)font, text_buffer, 0.0, 0.0, 0.0);

//	sprintf(text_buffer,"FRAMES PER SECOND");
//	open_gl.gl_text_renderer(30,YPIXELS - 20,(void *)font, text_buffer, 0.0, 0.0, 0.0);

	sprintf(text_buffer,"BARRACKS - $500");
	open_gl.gl_text_renderer(40, 591,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"%d",player_one.num_barracks);
	open_gl.gl_text_renderer(40, 571,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"WAR FACTORY - $2000");
	open_gl.gl_text_renderer(40, 536,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"%d",player_one.num_war_factories);
	open_gl.gl_text_renderer(40, 516,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"GUARD TOWER - $750");
	open_gl.gl_text_renderer(40, 481,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"%d",player_one.num_guard_towers);
	open_gl.gl_text_renderer(40, 461,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"TANK - $1750");
	open_gl.gl_text_renderer(40, 426,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"%d",player_one.num_tanks);
	open_gl.gl_text_renderer(40, 406,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"TROOPER - $200");
	open_gl.gl_text_renderer(40, 371,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"%d",player_one.num_troopers);
	open_gl.gl_text_renderer(40, 351,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"ARTILLERY - $600");
	open_gl.gl_text_renderer(40, 316,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"%d",player_one.num_troopers);
	open_gl.gl_text_renderer(40, 296,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"BUILDING - ");
	open_gl.gl_text_renderer(XPIXELS/2, YPIXELS-20,(void *)font, text_buffer, 0.0, 0.0, 0.0);

	sprintf(text_buffer,"BUILDING - ");
	open_gl.gl_text_renderer(XPIXELS/2 +1, YPIXELS-20 +1, (void *)font, text_buffer, 1.0, 0.0, 0.0);

	switch (current_building_to_build)						
	{
		case ID_FREE:		sprintf(text_buffer,"AWAITING ORDERS");
							open_gl.gl_text_renderer(XPIXELS/2 +50, YPIXELS-20,(void *)font, text_buffer, 0.0, 0.0, 0.0);

							sprintf(text_buffer,"AWAITING ORDERS");
							open_gl.gl_text_renderer(XPIXELS/2 +51, YPIXELS-20 +1, (void *)font, text_buffer, 1.0, 0.0, 0.0);

							break;

		case ID_BARRACKS:	sprintf(text_buffer,"BARRACKS");
							open_gl.gl_text_renderer(XPIXELS/2 +50, YPIXELS-20,(void *)font, text_buffer, 0.0, 0.0, 0.0);

							sprintf(text_buffer,"BARRACKS");
							open_gl.gl_text_renderer(XPIXELS/2 +51, YPIXELS-19, (void *)font, text_buffer, 1.0, 0.0, 0.0);

							sprintf(text_buffer,"%d/500",player_one.player_buildings.buildings[current_building_number].get_hit_points());
							open_gl.gl_text_renderer(XPIXELS/2 +50, YPIXELS-45,(void *)font, text_buffer, 0.0, 0.0, 0.0);

							sprintf(text_buffer,"%d/500",player_one.player_buildings.buildings[current_building_number].get_hit_points());
							open_gl.gl_text_renderer(XPIXELS/2 +51, YPIXELS-44,(void *)font, text_buffer, 1.0, 0.0, 0.0);

							break;

		case ID_WAR_FACTORY:	sprintf(text_buffer,"WAR FACTORY");
							open_gl.gl_text_renderer(XPIXELS/2 +50, YPIXELS-20,(void *)font, text_buffer, 0.0, 0.0, 0.0);

							sprintf(text_buffer,"WAR FACTORY");
							open_gl.gl_text_renderer(XPIXELS/2 +51, YPIXELS-19, (void *)font, text_buffer, 1.0, 0.0, 0.0);

							sprintf(text_buffer,"%d/2000",player_one.player_buildings.buildings[current_building_number].get_hit_points());
							open_gl.gl_text_renderer(XPIXELS/2 +50, YPIXELS-45,(void *)font, text_buffer, 0.0, 0.0, 0.0);

							sprintf(text_buffer,"%d/2000",player_one.player_buildings.buildings[current_building_number].get_hit_points());
							open_gl.gl_text_renderer(XPIXELS/2 +51, YPIXELS-44,(void *)font, text_buffer, 1.0, 0.0, 0.0);

							break;

		case ID_TOWER:		sprintf(text_buffer,"GUARD TOWER");
							open_gl.gl_text_renderer(XPIXELS/2 +50, YPIXELS-20,(void *)font, text_buffer, 0.0, 0.0, 0.0);

							sprintf(text_buffer,"GUARD TOWER");
							open_gl.gl_text_renderer(XPIXELS/2 +51, YPIXELS-19, (void *)font, text_buffer, 1.0, 0.0, 0.0);

							sprintf(text_buffer,"%d/750",player_one.player_buildings.buildings[current_building_number].get_hit_points());
							open_gl.gl_text_renderer(XPIXELS/2 +50, YPIXELS-45,(void *)font, text_buffer, 0.0, 0.0, 0.0);

							sprintf(text_buffer,"%d/750",player_one.player_buildings.buildings[current_building_number].get_hit_points());
							open_gl.gl_text_renderer(XPIXELS/2 +51, YPIXELS-44,(void *)font, text_buffer, 1.0, 0.0, 0.0);

							break;

		default:		break;
	}

	switch (current_unit_to_build)						
	{
		case TANK:			sprintf(text_buffer,"TANK");
							open_gl.gl_text_renderer(XPIXELS/2 +150, YPIXELS-20,(void *)font, text_buffer, 0.0, 0.0, 0.0);

							sprintf(text_buffer,"TANK");
							open_gl.gl_text_renderer(XPIXELS/2 +151, YPIXELS-19, (void *)font, text_buffer, 1.0, 0.0, 0.0);

							sprintf(text_buffer,"%d/1750",player_one.player_units[current_unit_number].get_hit_points());
							open_gl.gl_text_renderer(XPIXELS/2 +150, YPIXELS-45,(void *)font, text_buffer, 0.0, 0.0, 0.0);

							sprintf(text_buffer,"%d/1750",player_one.player_units[current_unit_number].get_hit_points());
							open_gl.gl_text_renderer(XPIXELS/2 +151, YPIXELS-44,(void *)font, text_buffer, 1.0, 0.0, 0.0);

							break;

		default:		break;
	}


	sprintf(text_buffer,"SCORE");
	open_gl.gl_text_renderer(10,10,(void *)font, text_buffer, 0.0, 0.0, 0.0);

	sprintf(text_buffer,"%d",player_one.score);
	open_gl.gl_text_renderer(75,10,(void *)font, text_buffer, 0.0, 0.0, 0.0);

	sprintf(text_buffer,"MONEY");
	open_gl.gl_text_renderer(XPIXELS/2, 10,(void *)font, text_buffer, 0.0, 0.0, 0.0);

	sprintf(text_buffer,"%d",player_one.money);
	open_gl.gl_text_renderer(XPIXELS/2 + 75, 10,(void *)font, text_buffer, 0.0, 0.0, 0.0);


//	sprintf(text_buffer,"%.0f",frames_per_second);
//	open_gl.gl_text_renderer(11,YPIXELS - 19,(void *)font, text_buffer, 1.0, 0.0, 0.0);

//	sprintf(text_buffer,"FRAMES PER SECOND");
//	open_gl.gl_text_renderer(31,YPIXELS - 19,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"SCORE");
	open_gl.gl_text_renderer(11,11,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"%d",player_one.score);
	open_gl.gl_text_renderer(76,11,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"MONEY");
	open_gl.gl_text_renderer((XPIXELS/2) +1, 11,(void *)font, text_buffer, 1.0, 0.0, 0.0);

	sprintf(text_buffer,"%d",player_one.money);
	open_gl.gl_text_renderer((XPIXELS/2) + 76, 11,(void *)font, text_buffer, 1.0, 0.0, 0.0);
}

////////////////////////////////////////////////////////////////////
// DRAW_HUD_BACKGROUNDS FUNCTION
// -----------------------------
//
// Draws the hud background. The boxes at the side with pictures of 
// the units.
//
////////////////////////////////////////////////////////////////////

void draw_hud_backgrounds (void)
{
	glPushMatrix();
		open_gl.gl_2d_viewing_system(100,0,0,10,10);

	glDisable(GL_DEPTH_TEST);				/* Disable the depth test and lighting. The lighting will not */
	glDisable(GL_LIGHTING);					/* work on the fonts, and the depth test will render the scene */
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);

	//	glCallList(open_gl.tex_hud);

	//	open_gl.gl_draw_textured_polygon(-70, 45, 0, 
	//									 -45, 45, 0, 
	//									 -45, -27, 0, 
	//									 -70, -27, 0, 1);

/*		open_gl.gl_draw_textured_polygon(-70, -48, 0, 
										 -45, -48, 0, 
										 -45, -70, 0, 
										 -70, -70, 0, 1);

		open_gl.gl_draw_textured_polygon(-15, -48, 0, 
										 10, -48, 0, 
										 10, -70, 0, 
										 -15, -70, 0, 1);
*/
		glCallList(open_gl.tex_build_barracks);

		open_gl.gl_draw_textured_polygon(-61, 14, 0, 
										 -66, 14, 0, 
										 -66, 9, 0, 
										 -61, 9, 0,1);

		glCallList(open_gl.tex_build_war);

		open_gl.gl_draw_textured_polygon(-61, 8, 0, 
										 -66, 8, 0, 
										 -66, 3, 0, 
										 -61, 3, 0,1);

		glCallList(open_gl.tex_build_gtower);

		open_gl.gl_draw_textured_polygon(-61, 2, 0, 
										 -66, 2, 0, 
										 -66, -3, 0, 
										 -61, -3, 0,1);

		glCallList(open_gl.tex_tank_build);

		open_gl.gl_draw_textured_polygon(-61, -4, 0, 
										 -66, -4, 0, 
										 -66, -9, 0, 
										 -61, -9, 0,1);

		glCallList(open_gl.tex_trooper_build);

		open_gl.gl_draw_textured_polygon(-61, -10, 0, 
										 -66, -10, 0, 
										 -66, -15, 0, 
										 -61, -15, 0,1);

		glCallList(open_gl.tex_artillery_build);

		open_gl.gl_draw_textured_polygon(-61, -16, 0, 
										 -66, -16, 0, 
										 -66, -21, 0, 
										 -61, -21, 0,1);

	glEnable(GL_DEPTH_TEST);	// Enable the depth test and lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);

	glPopMatrix();
}

////////////////////////////////////////////////////////////////////
// DRAW_BANKS FUNCTION
// -------------------
//
// Draws the banks in the scene.
//
////////////////////////////////////////////////////////////////////

void draw_banks (void)
{
	int vertex_one, vertex_two, vertex_three;
	int i, k;
	bool frustum_cull;

	glLoadName(NON_SELECTABLE_OBJECTS);

	for (i=0; i<num_banks; i++)
	{
		if (banks[i].get_controller() == NEUTRAL)
		{
				if ((frustum_cull = frustum.sphere_in_frustum(banks[i].get_x(), banks[i].get_y(), banks[i].get_z(), 20)) == true)
				{
					glCallList(open_gl.tex_neutral);

					glPushMatrix();
						glTranslatef(banks[i].get_x(), banks[i].get_y(), banks[i].get_z());

						glEnable(GL_TEXTURE_2D);

						for (k=0; k<bank.num_faces; k++)
						{
							vertex_one = bank.faces[k].get_vertex_one();
							vertex_two = bank.faces[k].get_vertex_two();
							vertex_three = bank.faces[k].get_vertex_three();
								
							glColor3f(1.0, 1.0, 1.0);

							glBegin(GL_TRIANGLES);
								set_texture_coord( bank.vertices[vertex_one].get_x(), bank.vertices[vertex_one].get_y(), bank.vertices[vertex_one].get_z(), 13 );
								glNormal3f(bank.vertices[vertex_one].get_x(), bank.vertices[vertex_one].get_y(), bank.vertices[vertex_one].get_z());
								glVertex3f(bank.vertices[vertex_one].get_x(), bank.vertices[vertex_one].get_y(), bank.vertices[vertex_one].get_z());

								set_texture_coord( bank.vertices[vertex_two].get_x(), bank.vertices[vertex_two].get_y(), bank.vertices[vertex_two].get_z(), 13 );
								glNormal3f(bank.vertices[vertex_two].get_x(), bank.vertices[vertex_two].get_y(), bank.vertices[vertex_two].get_z());
								glVertex3f(bank.vertices[vertex_two].get_x(), bank.vertices[vertex_two].get_y(), bank.vertices[vertex_two].get_z());

								set_texture_coord( bank.vertices[vertex_three].get_x(), bank.vertices[vertex_three].get_y(), bank.vertices[vertex_three].get_z(), 13 );
								glNormal3f(bank.vertices[vertex_three].get_x(), bank.vertices[vertex_three].get_y(), bank.vertices[vertex_three].get_z());
								glVertex3f(bank.vertices[vertex_three].get_x(), bank.vertices[vertex_three].get_y(), bank.vertices[vertex_three].get_z());
							glEnd();
						}
		/*
						open_gl.gl_draw_point(bank.bounding_box[0].get_x(), bank.bounding_box[0].get_y(), bank.bounding_box[0].get_z());
						open_gl.gl_draw_point(bank.bounding_box[1].get_x(), bank.bounding_box[1].get_y(), bank.bounding_box[1].get_z());
						open_gl.gl_draw_point(bank.bounding_box[2].get_x(), bank.bounding_box[2].get_y(), bank.bounding_box[2].get_z());
						open_gl.gl_draw_point(bank.bounding_box[3].get_x(), bank.bounding_box[3].get_y(), bank.bounding_box[3].get_z());

						open_gl.gl_draw_point(bank.bounding_box[4].get_x(), bank.bounding_box[4].get_y(), bank.bounding_box[4].get_z());
						open_gl.gl_draw_point(bank.bounding_box[5].get_x(), bank.bounding_box[5].get_y(), bank.bounding_box[5].get_z());
						open_gl.gl_draw_point(bank.bounding_box[6].get_x(), bank.bounding_box[6].get_y(), bank.bounding_box[6].get_z());
						open_gl.gl_draw_point(bank.bounding_box[7].get_x(), bank.bounding_box[7].get_y(), bank.bounding_box[7].get_z());
		*/
					glPopMatrix();

					glDisable(GL_TEXTURE_2D);
				}
		}

		else if (banks[i].get_controller() == PLAYER_ONE_HUMAN)
		{
			if ((frustum_cull = frustum.sphere_in_frustum(banks[i].get_x(), banks[i].get_y(), banks[i].get_z(), 20)) == true)
			{
				glCallList(open_gl.tex_player_one);

				glPushMatrix();
					glTranslatef(banks[i].get_x(), banks[i].get_y(), banks[i].get_z());

					glEnable(GL_TEXTURE_2D);

					for (k=0; k<bank.num_faces; k++)
					{
						vertex_one = bank.faces[k].get_vertex_one();
						vertex_two = bank.faces[k].get_vertex_two();
						vertex_three = bank.faces[k].get_vertex_three();
							
						glColor3f(1.0, 1.0, 1.0);

							glBegin(GL_TRIANGLES);
								set_texture_coord( bank.vertices[vertex_one].get_x(), bank.vertices[vertex_one].get_y(), bank.vertices[vertex_one].get_z(), 13 );
								glNormal3f(bank.vertices[vertex_one].get_x(), bank.vertices[vertex_one].get_y(), bank.vertices[vertex_one].get_z());
								glVertex3f(bank.vertices[vertex_one].get_x(), bank.vertices[vertex_one].get_y(), bank.vertices[vertex_one].get_z());

								set_texture_coord( bank.vertices[vertex_two].get_x(), bank.vertices[vertex_two].get_y(), bank.vertices[vertex_two].get_z(), 13 );
								glNormal3f(bank.vertices[vertex_two].get_x(), bank.vertices[vertex_two].get_y(), bank.vertices[vertex_two].get_z());
								glVertex3f(bank.vertices[vertex_two].get_x(), bank.vertices[vertex_two].get_y(), bank.vertices[vertex_two].get_z());

								set_texture_coord( bank.vertices[vertex_three].get_x(), bank.vertices[vertex_three].get_y(), bank.vertices[vertex_three].get_z(), 13 );
								glNormal3f(bank.vertices[vertex_three].get_x(), bank.vertices[vertex_three].get_y(), bank.vertices[vertex_three].get_z());
								glVertex3f(bank.vertices[vertex_three].get_x(), bank.vertices[vertex_three].get_y(), bank.vertices[vertex_three].get_z());
							glEnd();
					}
	/*
					open_gl.gl_draw_point(bank.bounding_box[0].get_x(), bank.bounding_box[0].get_y(), bank.bounding_box[0].get_z());
					open_gl.gl_draw_point(bank.bounding_box[1].get_x(), bank.bounding_box[1].get_y(), bank.bounding_box[1].get_z());
					open_gl.gl_draw_point(bank.bounding_box[2].get_x(), bank.bounding_box[2].get_y(), bank.bounding_box[2].get_z());
					open_gl.gl_draw_point(bank.bounding_box[3].get_x(), bank.bounding_box[3].get_y(), bank.bounding_box[3].get_z());

					open_gl.gl_draw_point(bank.bounding_box[4].get_x(), bank.bounding_box[4].get_y(), bank.bounding_box[4].get_z());
					open_gl.gl_draw_point(bank.bounding_box[5].get_x(), bank.bounding_box[5].get_y(), bank.bounding_box[5].get_z());
					open_gl.gl_draw_point(bank.bounding_box[6].get_x(), bank.bounding_box[6].get_y(), bank.bounding_box[6].get_z());
					open_gl.gl_draw_point(bank.bounding_box[7].get_x(), bank.bounding_box[7].get_y(), bank.bounding_box[7].get_z());
	*/
				glPopMatrix();

				glDisable(GL_TEXTURE_2D);
			}
		}

		else if (banks[i].get_controller() == PLAYER_TWO_CPU)
		{
			if ((frustum_cull = frustum.sphere_in_frustum(banks[i].get_x(), banks[i].get_y(), banks[i].get_z(), 20)) == true)
			{
				glCallList(open_gl.tex_player_two);

				glPushMatrix();
					glTranslatef(banks[i].get_x(), banks[i].get_y(), banks[i].get_z());

					glEnable(GL_TEXTURE_2D);

					for (k=0; k<bank.num_faces; k++)
					{
						vertex_one = bank.faces[k].get_vertex_one();
						vertex_two = bank.faces[k].get_vertex_two();
						vertex_three = bank.faces[k].get_vertex_three();
							
						glColor3f(1.0, 1.0, 1.0);

							glBegin(GL_TRIANGLES);
								set_texture_coord( bank.vertices[vertex_one].get_x(), bank.vertices[vertex_one].get_y(), bank.vertices[vertex_one].get_z(), 13 );
								glNormal3f(bank.vertices[vertex_one].get_x(), bank.vertices[vertex_one].get_y(), bank.vertices[vertex_one].get_z());
								glVertex3f(bank.vertices[vertex_one].get_x(), bank.vertices[vertex_one].get_y(), bank.vertices[vertex_one].get_z());

								set_texture_coord( bank.vertices[vertex_two].get_x(), bank.vertices[vertex_two].get_y(), bank.vertices[vertex_two].get_z(), 13 );
								glNormal3f(bank.vertices[vertex_two].get_x(), bank.vertices[vertex_two].get_y(), bank.vertices[vertex_two].get_z());
								glVertex3f(bank.vertices[vertex_two].get_x(), bank.vertices[vertex_two].get_y(), bank.vertices[vertex_two].get_z());

								set_texture_coord( bank.vertices[vertex_three].get_x(), bank.vertices[vertex_three].get_y(), bank.vertices[vertex_three].get_z(), 13 );
								glNormal3f(bank.vertices[vertex_three].get_x(), bank.vertices[vertex_three].get_y(), bank.vertices[vertex_three].get_z());
								glVertex3f(bank.vertices[vertex_three].get_x(), bank.vertices[vertex_three].get_y(), bank.vertices[vertex_three].get_z());
							glEnd();
					}
	/*
					open_gl.gl_draw_point(bank.bounding_box[0].get_x(), bank.bounding_box[0].get_y(), bank.bounding_box[0].get_z());
					open_gl.gl_draw_point(bank.bounding_box[1].get_x(), bank.bounding_box[1].get_y(), bank.bounding_box[1].get_z());
					open_gl.gl_draw_point(bank.bounding_box[2].get_x(), bank.bounding_box[2].get_y(), bank.bounding_box[2].get_z());
					open_gl.gl_draw_point(bank.bounding_box[3].get_x(), bank.bounding_box[3].get_y(), bank.bounding_box[3].get_z());

					open_gl.gl_draw_point(bank.bounding_box[4].get_x(), bank.bounding_box[4].get_y(), bank.bounding_box[4].get_z());
					open_gl.gl_draw_point(bank.bounding_box[5].get_x(), bank.bounding_box[5].get_y(), bank.bounding_box[5].get_z());
					open_gl.gl_draw_point(bank.bounding_box[6].get_x(), bank.bounding_box[6].get_y(), bank.bounding_box[6].get_z());
					open_gl.gl_draw_point(bank.bounding_box[7].get_x(), bank.bounding_box[7].get_y(), bank.bounding_box[7].get_z());
	*/
				glPopMatrix();

				glDisable(GL_TEXTURE_2D);
			}
		}
	}

	glEnd();	//end load name
//	glPopName();
}

////////////////////////////////////////////////////////////////////
// SET TEX COORD FUNCTION
// -------------------
// 
// Uses a bit of math to calculate the texture coordinates for the
// models texture mapping. This is all hard coded and not very
// efficient. Future developments would definately include a more
// robust model loading routine.
//
////////////////////////////////////////////////////////////////////

void set_texture_coord(float x, float y, float z, float tex)
{
	glTexCoord3f(   (float)x / (float)tex,
				  - (float)y / (float)tex,
				  - (float)z / (float)tex	);
}

////////////////////////////////////////////////////////////////////
// DRAW_TREES FUNCTION
// -------------------
// 
// Draws the trees in the scene
//
////////////////////////////////////////////////////////////////////

void draw_trees (void)
{
	int vertex_one, vertex_two, vertex_three;
	int i, j, k;
	bool frustum_cull;

	glCallList(open_gl.tex_tree);
	glLoadName(NON_SELECTABLE_OBJECTS);
	glEnable(GL_LIGHTING);

	for (i=0; i<TERRAIN_SIZE; i++)
	{
		for (j=0; j<TERRAIN_SIZE; j++)
		{
			if (terrain[i][j].tree == 1)
			{
				if ((frustum_cull = frustum.sphere_in_frustum(terrain[i][j].x, terrain[i][j].y - 5, terrain[i][j].z, 15)) == true)
				{
					glPushMatrix();
						glTranslatef(terrain[i][j].x, terrain[i][j].y - 5, terrain[i][j].z);
						glRotatef(terrain[i][j].tree_orientation, 0, 1, 0);

						glEnable(GL_TEXTURE_2D);

						for (k=0; k<tree.num_faces; k++)
						{
							vertex_one = tree.faces[k].get_vertex_one();
							vertex_two = tree.faces[k].get_vertex_two();
							vertex_three = tree.faces[k].get_vertex_three();
							
							glColor3f(1.0, 1.0, 1.0);

							glBegin(GL_TRIANGLES);
								glTexCoord2f(1.0f, 1.0f);
								glNormal3f(tree.vertices[vertex_one].get_x(), tree.vertices[vertex_one].get_y(), tree.vertices[vertex_one].get_z());
								glVertex3f(tree.vertices[vertex_one].get_x(), tree.vertices[vertex_one].get_y(), tree.vertices[vertex_one].get_z());

								glTexCoord2f(1.0f, 0.0f);
								glNormal3f(tree.vertices[vertex_two].get_x(), tree.vertices[vertex_two].get_y(), tree.vertices[vertex_two].get_z());
								glVertex3f(tree.vertices[vertex_two].get_x(), tree.vertices[vertex_two].get_y(), tree.vertices[vertex_two].get_z());

								glTexCoord2f(0.0f, 0.0f);
								glNormal3f(tree.vertices[vertex_three].get_x(), tree.vertices[vertex_three].get_y(), tree.vertices[vertex_three].get_z());
								glVertex3f(tree.vertices[vertex_three].get_x(), tree.vertices[vertex_three].get_y(), tree.vertices[vertex_three].get_z());
							glEnd();
						}

				/*	open_gl.gl_draw_point(tree.bounding_box[0].get_x(), tree.bounding_box[0].get_y(), tree.bounding_box[0].get_z());
					open_gl.gl_draw_point(tree.bounding_box[1].get_x(), tree.bounding_box[1].get_y(), tree.bounding_box[1].get_z());
					open_gl.gl_draw_point(tree.bounding_box[2].get_x(), tree.bounding_box[2].get_y(), tree.bounding_box[2].get_z());
					open_gl.gl_draw_point(tree.bounding_box[3].get_x(), tree.bounding_box[3].get_y(), tree.bounding_box[3].get_z());

					open_gl.gl_draw_point(tree.bounding_box[4].get_x(), tree.bounding_box[4].get_y(), tree.bounding_box[4].get_z());
					open_gl.gl_draw_point(tree.bounding_box[5].get_x(), tree.bounding_box[5].get_y(), tree.bounding_box[5].get_z());
					open_gl.gl_draw_point(tree.bounding_box[6].get_x(), tree.bounding_box[6].get_y(), tree.bounding_box[6].get_z());
					open_gl.gl_draw_point(tree.bounding_box[7].get_x(), tree.bounding_box[7].get_y(), tree.bounding_box[7].get_z());
				*/
					glPopMatrix();

					glDisable(GL_TEXTURE_2D);
				}
			}
		}
	}

	glEnd();	//end load name
//	glPopName();
}

////////////////////////////////////////////////////////////////////
// DRAW_HQS FUNCTION
// -----------------
//
// Draw headquarters models.
//
////////////////////////////////////////////////////////////////////

void draw_hqs (void)
{
	int vertex_one, vertex_two, vertex_three;
	int k;
	bool frustum_cull;

	glLoadName(NON_SELECTABLE_OBJECTS);

	if ((frustum_cull = frustum.sphere_in_frustum(player_one.hq_info.get_x(), player_one.hq_info.get_y(), player_one.hq_info.get_z(), 25)) == true)
	{
		glCallList(open_gl.tex_player_one_hq);
		glEnable(GL_LIGHTING);

		glPushMatrix();
			glTranslatef(player_one.hq_info.get_x(), player_one.hq_info.get_y(), player_one.hq_info.get_z());

			glEnable(GL_TEXTURE_2D);

			for (k=0; k<hq.num_faces; k++)
			{
				vertex_one = hq.faces[k].get_vertex_one();
				vertex_two = hq.faces[k].get_vertex_two();
				vertex_three = hq.faces[k].get_vertex_three();
							
				glColor3f(1.0, 1.0, 1.0);

				glBegin(GL_TRIANGLES);
					set_texture_coord( hq.vertices[vertex_one].get_x(), hq.vertices[vertex_one].get_y(), hq.vertices[vertex_one].get_z(), 27 );
					glNormal3f(hq.vertices[vertex_one].get_x(), hq.vertices[vertex_one].get_y(), hq.vertices[vertex_one].get_z());
					glVertex3f(hq.vertices[vertex_one].get_x(), hq.vertices[vertex_one].get_y(), hq.vertices[vertex_one].get_z());

					set_texture_coord( hq.vertices[vertex_two].get_x(), hq.vertices[vertex_two].get_y(), hq.vertices[vertex_two].get_z(), 27 );
					glNormal3f(hq.vertices[vertex_two].get_x(), hq.vertices[vertex_two].get_y(), hq.vertices[vertex_two].get_z());
					glVertex3f(hq.vertices[vertex_two].get_x(), hq.vertices[vertex_two].get_y(), hq.vertices[vertex_two].get_z());

					set_texture_coord( hq.vertices[vertex_three].get_x(), hq.vertices[vertex_three].get_y(), hq.vertices[vertex_three].get_z(), 27 );
					glNormal3f(hq.vertices[vertex_three].get_x(), hq.vertices[vertex_three].get_y(), hq.vertices[vertex_three].get_z());
					glVertex3f(hq.vertices[vertex_three].get_x(), hq.vertices[vertex_three].get_y(), hq.vertices[vertex_three].get_z());
				glEnd();
		}
	/*
		open_gl.gl_draw_point(hq.bounding_box[0].get_x(), hq.bounding_box[0].get_y(), hq.bounding_box[0].get_z());
		open_gl.gl_draw_point(hq.bounding_box[1].get_x(), hq.bounding_box[1].get_y(), hq.bounding_box[1].get_z());
		open_gl.gl_draw_point(hq.bounding_box[2].get_x(), hq.bounding_box[2].get_y(), hq.bounding_box[2].get_z());
		open_gl.gl_draw_point(hq.bounding_box[3].get_x(), hq.bounding_box[3].get_y(), hq.bounding_box[3].get_z());

		open_gl.gl_draw_point(hq.bounding_box[4].get_x(), hq.bounding_box[4].get_y(), hq.bounding_box[4].get_z());
		open_gl.gl_draw_point(hq.bounding_box[5].get_x(), hq.bounding_box[5].get_y(), hq.bounding_box[5].get_z());
		open_gl.gl_draw_point(hq.bounding_box[6].get_x(), hq.bounding_box[6].get_y(), hq.bounding_box[6].get_z());
		open_gl.gl_draw_point(hq.bounding_box[7].get_x(), hq.bounding_box[7].get_y(), hq.bounding_box[7].get_z());
	*/
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
	}

	if ((frustum_cull = frustum.sphere_in_frustum(player_two.hq_info.get_x(), player_two.hq_info.get_y(), player_two.hq_info.get_z(), 20)) == true)
	{
		glCallList(open_gl.tex_player_two_hq);

		glPushMatrix();
			glTranslatef(player_two.hq_info.get_x(), player_two.hq_info.get_y(), player_two.hq_info.get_z());

			glEnable(GL_TEXTURE_2D);

			for (k=0; k<hq.num_faces; k++)
			{
				vertex_one = hq.faces[k].get_vertex_one();
				vertex_two = hq.faces[k].get_vertex_two();
				vertex_three = hq.faces[k].get_vertex_three();
							
				glColor3f(1.0, 1.0, 1.0);

				glBegin(GL_TRIANGLES);
					set_texture_coord( hq.vertices[vertex_one].get_x(), hq.vertices[vertex_one].get_y(), hq.vertices[vertex_one].get_z(), 27 );
					glNormal3f(hq.vertices[vertex_one].get_x(), hq.vertices[vertex_one].get_y(), hq.vertices[vertex_one].get_z());
					glVertex3f(hq.vertices[vertex_one].get_x(), hq.vertices[vertex_one].get_y(), hq.vertices[vertex_one].get_z());

					set_texture_coord( hq.vertices[vertex_two].get_x(), hq.vertices[vertex_two].get_y(), hq.vertices[vertex_two].get_z(), 27 );
					glNormal3f(hq.vertices[vertex_two].get_x(), hq.vertices[vertex_two].get_y(), hq.vertices[vertex_two].get_z());
					glVertex3f(hq.vertices[vertex_two].get_x(), hq.vertices[vertex_two].get_y(), hq.vertices[vertex_two].get_z());

					set_texture_coord( hq.vertices[vertex_three].get_x(), hq.vertices[vertex_three].get_y(), hq.vertices[vertex_three].get_z(), 27 );
					glNormal3f(hq.vertices[vertex_three].get_x(), hq.vertices[vertex_three].get_y(), hq.vertices[vertex_three].get_z());
					glVertex3f(hq.vertices[vertex_three].get_x(), hq.vertices[vertex_three].get_y(), hq.vertices[vertex_three].get_z());
				glEnd();
		}
	/*
		open_gl.gl_draw_point(hq.bounding_box[0].get_x(), hq.bounding_box[0].get_y(), hq.bounding_box[0].get_z());
		open_gl.gl_draw_point(hq.bounding_box[1].get_x(), hq.bounding_box[1].get_y(), hq.bounding_box[1].get_z());
		open_gl.gl_draw_point(hq.bounding_box[2].get_x(), hq.bounding_box[2].get_y(), hq.bounding_box[2].get_z());
		open_gl.gl_draw_point(hq.bounding_box[3].get_x(), hq.bounding_box[3].get_y(), hq.bounding_box[3].get_z());

		open_gl.gl_draw_point(hq.bounding_box[4].get_x(), hq.bounding_box[4].get_y(), hq.bounding_box[4].get_z());
		open_gl.gl_draw_point(hq.bounding_box[5].get_x(), hq.bounding_box[5].get_y(), hq.bounding_box[5].get_z());
		open_gl.gl_draw_point(hq.bounding_box[6].get_x(), hq.bounding_box[6].get_y(), hq.bounding_box[6].get_z());
		open_gl.gl_draw_point(hq.bounding_box[7].get_x(), hq.bounding_box[7].get_y(), hq.bounding_box[7].get_z());
	*/
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
	}

	glEnd();	//end load name
	glPopName();
}

////////////////////////////////////////////////////////////////////
// DRAW_TERRAIN_POLYGONS FUNCTION
// ------------------------------
//
// This function draws all the terrain polygons.
//
////////////////////////////////////////////////////////////////////

void draw_terrain_polygons (int frustum_on)
{
	int i, j;
	bool in_frustum0, in_frustum1, in_frustum2, in_frustum3;

	i=0;
	glLoadName(NON_SELECTABLE_OBJECTS);

	if (terrain_grid_tag == 0)
	{
		glCallList(open_gl.tex_grass);
	}
	else if (terrain_grid_tag == 1)
	{
		glCallList(open_gl.tex_grass_grid);
	}

	glEnable(GL_TEXTURE_2D);

	if (frustum_on == YES)
	{
		for (i=0; i<TERRAIN_SIZE-1; i++)
		{
			for (j=0; j<TERRAIN_SIZE-1; j++)
			{
			//	in_frustum0 = frustum.point_in_frustum(terrain[i+1][j].x, terrain[i+1][j].y, terrain[i+1][j].z);
			//	in_frustum1 = frustum.point_in_frustum(terrain[i+1][j+1].x, terrain[i+1][j+1].y, terrain[i+1][j+1].z);
			//	in_frustum2 = frustum.point_in_frustum(terrain[i][j+1].x, terrain[i][j+1].y, terrain[i][j+1].z);
			//	in_frustum3 = frustum.point_in_frustum(terrain[i][j].x, terrain[i][j].y, terrain[i][j].z);

				in_frustum0 = frustum.sphere_in_frustum(terrain[i+1][j].x, terrain[i+1][j].y, terrain[i+1][j].z, 45);
				in_frustum1 = frustum.sphere_in_frustum(terrain[i+1][j+1].x, terrain[i+1][j+1].y, terrain[i+1][j+1].z, 45);
				in_frustum2 = frustum.sphere_in_frustum(terrain[i][j+1].x, terrain[i][j+1].y, terrain[i][j+1].z, 45);
				in_frustum3 = frustum.sphere_in_frustum(terrain[i][j].x, terrain[i][j].y, terrain[i][j].z, 45);

				if (in_frustum0 == true && in_frustum1 == true && in_frustum2 == true && in_frustum3 == true)
				{
					open_gl.gl_draw_textured_polygon(terrain[i+1][j].x, terrain[i+1][j].y, terrain[i+1][j].z,
													 terrain[i+1][j+1].x, terrain[i+1][j+1].y, terrain[i+1][j+1].z,
													 terrain[i][j+1].x, terrain[i][j+1].y, terrain[i][j+1].z,
													 terrain[i][j].x, terrain[i][j].y, terrain[i][j].z, 0);
				}
			}
		}
	}

	glDisable(GL_TEXTURE_2D);

	glEnd();	//end load name
//	glPopName();

	if (frustum_on == NO)
	{
		glDisable(GL_LIGHTING);					/* work on the fonts, and the depth test will render the scene */
		glDisable(GL_FOG);

		for (i=0; i<TERRAIN_SIZE-1; i++)
		{
			for (j=0; j<TERRAIN_SIZE-1; j++)
			{
				glColor3f(0.0, terrain[i][j].y/100, 0.0);

				glBegin(GL_QUADS);
					glNormal3f(terrain[i+1][j].x, terrain[i+1][j].y, terrain[i+1][j].z);
					glVertex3f(terrain[i+1][j].x, terrain[i+1][j].y, terrain[i+1][j].z);

					glNormal3f(terrain[i+1][j+1].x, terrain[i+1][j+1].y, terrain[i+1][j+1].z);
					glVertex3f(terrain[i+1][j+1].x, terrain[i+1][j+1].y, terrain[i+1][j+1].z);

					glNormal3f(terrain[i][j+1].x, terrain[i][j+1].y, terrain[i][j+1].z);
					glVertex3f(terrain[i][j+1].x, terrain[i][j+1].y, terrain[i][j+1].z);

					glNormal3f(terrain[i][j].x, terrain[i][j].y, terrain[i][j].z);
					glVertex3f(terrain[i][j].x, terrain[i][j].y, terrain[i][j].z);
				glEnd();
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
// SPECIAL FUNCTION
// -----------------
//
// Special callback funstion.
////////////////////////////////////////////////////////////////////

void special (int key, int x, int y)
{
	if (distance_between_two_vectors(place_building.x, place_building.y, place_building.z, 
					player_one.hq_info.get_x(), player_one.hq_info.get_y(), player_one.hq_info.get_z()) < 200
						&& place_building.ready_to_place_building == YES)
	{
		switch (key)						
		{
			// Basically, move the selected point left, right, up and down.
			case GLUT_KEY_RIGHT:	place_building.x += 10;	
				
								/*	place_building.x = terrain[place_building.terrain_id_i][place_building.terrain_id_j + TERRAIN_SIZE].x;
									place_building.y = terrain[place_building.terrain_id_i][place_building.terrain_id_j + TERRAIN_SIZE].y;
									place_building.z = terrain[place_building.terrain_id_i][place_building.terrain_id_j + TERRAIN_SIZE].z;

									place_building.terrain_id_j += TERRAIN_SIZE;
								*/
							//		cout << distance_between_two_vectors(place_building.x, place_building.y, place_building.z, 
							//		player_one.hq_info.get_x(), player_one.hq_info.get_y(), player_one.hq_info.get_z()) << endl;

									break;

			case GLUT_KEY_LEFT:		place_building.x -= 10;	
								/*
									place_building.x = terrain[place_building.terrain_id_i][place_building.terrain_id_j - TERRAIN_SIZE].x;
									place_building.y = terrain[place_building.terrain_id_i][place_building.terrain_id_j - TERRAIN_SIZE].y;
									place_building.z = terrain[place_building.terrain_id_i][place_building.terrain_id_j - TERRAIN_SIZE].z;

									place_building.terrain_id_j -= TERRAIN_SIZE;
								*/
									break;

			case GLUT_KEY_UP:		place_building.z -= 10;	
								
									break;

			case GLUT_KEY_DOWN:		place_building.z += 10;	
									
									break;

			default:	break;								
		}
	}
}

////////////////////////////////////////////////////////////////////
// KEYBOARD FUNCTION
// -----------------
//
// Keyboard callback funstion.
//
////////////////////////////////////////////////////////////////////

void keyboard (unsigned char key, int x, int y)
{
	switch (key)						
	{
		case '=':	open_gl.fp_y -= 5;

					frustum.calculate_frustum();

					break;

		case '-':	open_gl.fp_y += 5;

					frustum.calculate_frustum();

					break;

		case 'q':   ////PlaySound("Data/Sounds/BCT1.wav",NULL,SND_FILENAME | SND_SYNC );
					exit (0);

					break;

		default:	break;
	}  

	if (open_gl.fp_y < -500)
	{
		open_gl.fp_y = -500;
	}
	else if (open_gl.fp_y > -25)
	{
		open_gl.fp_y = -25;
	}

	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////
// MOUSE FUNCTION
// --------------
//
// Mouse callback function. Called when a button is activated
// If the left mouse is clicked when a building is ready it will 
// place it down. When the cursor is moved with a unit selected
// the cursor will move to the centre as well as the mouse pointer.
//
////////////////////////////////////////////////////////////////////

void mouse(int button, int state, int x, int y)
{
	int i, j;

	if (place_building.ready_to_place_building == YES)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			player_one.player_buildings.buildings[current_building_number].set_x(place_building.x);
			player_one.player_buildings.buildings[current_building_number].set_y(place_building.y);
			player_one.player_buildings.buildings[current_building_number].set_z(place_building.z);

			if(current_building_to_build == ID_BARRACKS)
			{
				player_one.player_buildings.buildings[current_building_number].set_id(ID_BARRACKS);
			}

			if(current_building_to_build == ID_WAR_FACTORY)
			{
				player_one.player_buildings.buildings[current_building_number].set_id(ID_WAR_FACTORY);
			}

			if(current_building_to_build == ID_TOWER)
			{
				player_one.player_buildings.buildings[current_building_number].set_id(ID_TOWER);
			}

			place_building.ready_to_place_building = NO;
			
			////PlaySound("Data/Sounds/PLACBLDG.wav",NULL,SND_FILENAME | SND_ASYNC  );

		/*	place_building.x = player_one.hq_info.get_x();
			place_building.y = player_one.hq_info.get_y();
			place_building.z = player_one.hq_info.get_z() + 10;*/

 			for (i=0; i<TERRAIN_SIZE; i++)
			{
				for (j=0; j<TERRAIN_SIZE; j++)
				{
					if (terrain[i][j].id_number == player_one_start_point)
					{
						place_building.x = terrain[i][j].x;
						place_building.y = terrain[i][j].y;
						place_building.z = terrain[i][j].z;
						place_building.terrain_id_i = i;
						place_building.terrain_id_j = j;
					}
				}
			}
		}
	}

	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (selected_unit != 0)
		{
			selected_unit = retrieve_object_id(x, y);
		}
		else
		{
			player_one.player_units[selected_unit].set_target_x(place_for_selected_unit_to_go.get_x());
			player_one.player_units[selected_unit].set_target_y(place_for_selected_unit_to_go.get_y());
			player_one.player_units[selected_unit].set_target_z(place_for_selected_unit_to_go.get_z());
			////PlaySound("Data/Sounds/SKING1.wav",NULL,SND_FILENAME | SND_ASYNC  );
			selected_unit = NON_SELECTABLE_OBJECTS;
		}
	}

	frustum.calculate_frustum();

	open_gl.gl_mouse(x, y, button, state);

	if (selected_unit == 0)
	{
		place_for_selected_unit_to_go.set_x(-open_gl.fp_x);
		place_for_selected_unit_to_go.set_z(-open_gl.fp_z);
		SetCursorPos(fullscreen_x/2, fullscreen_y/2);
	}

	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////
// RETRIEVE_OBJECT_ID FUNCTION
// ---------------------------
//
// Used when the player clicks the left mouse button. The basic principle
// is this:
/*
		* Setup a select buffer where all select information will go.

		* Setup the pick matrix in OpenGL.

		* Render the scene around the mouse pointer. You can adjust the
		  sensitivity of the clicking by altering the gluPickMatrix command.
		  The two numbers are the length and breadth of the pick area around 
		  the mouse cursor in pixels.

		* If anything has been clicked, it is stored in the select buffer.
		  However, in terms of this game, only the foremost selected object
		  is relevant. We therefore have to sort through all the found items
		  for the smallest Z value, therefore the closest object.

		* Once this has happened, we have the ID of the object that has
		  been clicked on.

*/
////////////////////////////////////////////////////////////////////

int retrieve_object_id(int x, int y)
{
	int objects_found = 0;								// This will hold the amount of objects clicked
	int	viewport_coords[4] = {0};
	unsigned int select_buffer[32] = {0};

	glSelectBuffer(32, select_buffer);

	glGetIntegerv(GL_VIEWPORT, viewport_coords);

	glMatrixMode(GL_PROJECTION);

	glPushMatrix();
		glLoadIdentity();

		glRenderMode(GL_SELECT);
		current_render_mode = GL_SELECT;

		gluPickMatrix(x, viewport_coords[3] - y, 5, 5, viewport_coords);

		gluPerspective(45, 1.0, 0.1, 10000.0);

		glMatrixMode(GL_MODELVIEW);

		display();

		objects_found = glRenderMode(GL_RENDER);
		current_render_mode = GL_RENDER;

		glMatrixMode(GL_PROJECTION);					
	glPopMatrix();										

	glMatrixMode(GL_MODELVIEW);

	if (objects_found > 0)
	{
		unsigned int lowest_depth = select_buffer[1];
		int selected_object = select_buffer[3];

		cout << objects_found << " objects found" << endl;

		for(int i = 1; i < objects_found; i++)
		{
			if(select_buffer[(i * 4) + 1] < lowest_depth)
			{
				lowest_depth = select_buffer[(i * 4) + 1];

				selected_object = select_buffer[(i * 4) + 3];
			}
		}

		if (selected_object != NON_SELECTABLE_OBJECTS)
		{
			place_for_selected_unit_to_go.set_x(player_one.player_units[selected_object].get_x());
			place_for_selected_unit_to_go.set_y(player_one.player_units[selected_object].get_y());
			place_for_selected_unit_to_go.set_z(player_one.player_units[selected_object].get_z());
		}

		cout << selected_object << endl;

		return selected_object;
	}

	cout << "No objects selected" << endl;

	return 0;
}

////////////////////////////////////////////////////////////////////
// MOTION FUNCTION
// ---------------
//  
// Motion callback function is called when the mouse is moved with a
// button held down.
//
////////////////////////////////////////////////////////////////////

void motion(int x, int y)
{
	frustum.calculate_frustum();

	open_gl.gl_motion(x, y);
}

////////////////////////////////////////////////////////////////////
// PASSIVE_MOTION FUNCTION
// -----------------------
//
// Passive mouse function. Stores coordinates and moves the camera 
// when the mouse is moved WITHOUT a button held down.
///
////////////////////////////////////////////////////////////////////

void passive_motion(int x, int y)
{
	int temp, j, k, l, m;
	float float_temp;

	glutSetWindow(window_main);

	if (selected_unit == 0)
	{
		if (x > p_x)
		{
			place_for_selected_unit_to_go.set_x(place_for_selected_unit_to_go.get_x() + 10);
		}
		if (x < p_x)
		{
			place_for_selected_unit_to_go.set_x(place_for_selected_unit_to_go.get_x() - 10);
		}
		if (y > p_y)
		{
			place_for_selected_unit_to_go.set_z(place_for_selected_unit_to_go.get_z() + 10);
		}
		if (y < p_y)
		{
			place_for_selected_unit_to_go.set_z(place_for_selected_unit_to_go.get_z() - 10);
		}

		temp = 999999999;

		for (j=0; j<TERRAIN_SIZE; j++)
		{
			for (k=0; k<TERRAIN_SIZE; k++)
			{
				float_temp = distance_between_two_vectors(place_for_selected_unit_to_go.get_x(), 
														   place_for_selected_unit_to_go.get_y(), 
														   place_for_selected_unit_to_go.get_z(), 
				terrain[j][k].x, terrain[j][k].y, terrain[j][k].z);	

				if (float_temp < temp)
				{
					temp = float_temp;
					place_for_selected_unit_to_go_terrain_number = terrain[j][k].id_number;
					place_for_selected_unit_to_go.set_y(terrain[j][k].y);
				}
			}
		}

		for (l=0; l<TERRAIN_SIZE; l++)
		{
			for (m=0; m<TERRAIN_SIZE; m++)
			{
				if (terrain[l][m].id_number == place_for_selected_unit_to_go_terrain_number)
				{
					place_for_selected_unit_to_go.set_y(terrain[l][m].y);
				}
			}
		}
	}

	p_x = x;
	p_y = y;

	if (x < p_x)
	{
		place_building.x += 10;
	}
	if (x > p_x)
	{
		place_building.x -= 10;
	}	

	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////
// INITIALISE FUNCTION
// -------------------
//
// Initialises the programs initial parameters
//
////////////////////////////////////////////////////////////////////

void initialise (void)
{
	int i, j, count;

	game_state = INITIALISE;		// Set the states and render modes
	current_render_mode = GL_RENDER;
	
	glutSetWindow(window_main);		// Set perspectives
	open_gl.gl_initialise_perspective();

	glutSetWindow(radar_window);
	glutReshapeWindow(fullscreen_y/5, fullscreen_y/5 );
	open_gl.gl_initialise_perspective();

	glutSetWindow(window_main);

	load_camera_settings();			// Loads the initial camera settings
	cout << "Camera settings loaded" << endl << endl; 

	load_light_settings();			// Loads the initial light settings
	cout << "Light settings loaded" << endl << endl;

	load_map();						//load the map and models
	load_models();

	open_gl.gl_setup_textures(climate);	// Load textures
	cout << "Textures loaded" << endl << endl;

	ai.set_ai_state(GATHER_RESOURCES);	// Set the AI to gather resources to start

	count = 0;							// These are all settings, switches and other variables that
	search_for_best_bank_switch = 0;	// need initialising to eliminate any possible errors.
	terrain_grid_tag = 0;
	create_building = NO;
	build_switch = FALSE;
	unit_build_switch = FALSE;
	current_building_number = 0;
	current_building_to_build = ID_FREE;
	create_building = FALSE;
	place_for_selected_unit_to_go.set_x(0.0);
	place_for_selected_unit_to_go.set_y(0.0);
	place_for_selected_unit_to_go.set_z(0.0);

 	for (i=0; i<TERRAIN_SIZE; i++)			// This loads the data from the map that is not terrain data.
	{
		for (j=0; j<TERRAIN_SIZE; j++)
		{
			if (terrain[i][j].tree == 1)
			{
				terrain[i][j].tree_orientation = rand()%360;
			}

			else if (terrain[i][j].id_number == player_one_start)
			{
				player_one.hq_info.set_x(terrain[i][j].x);
				player_one.hq_info.set_y(terrain[i][j].y);
				player_one.hq_info.set_z(terrain[i][j].z);
				player_one_start_point = terrain[i][j].id_number;
			}

			else if (terrain[i][j].id_number == player_two_start)
			{
				player_two.hq_info.set_x(terrain[i][j].x);
				player_two.hq_info.set_y(terrain[i][j].y);
				player_two.hq_info.set_z(terrain[i][j].z);
				player_two_start_point = terrain[i][j].id_number;
			}

			else if (terrain[i][j].bank == 1)
			{
				banks[count].set_x(terrain[i][j].x + 12);
				banks[count].set_y(terrain[i][j].y);
				banks[count].set_z(terrain[i][j].z + 12);
				banks[count].set_controller(NEUTRAL);

				count++;
			}
		}
	}

	setup_player_one(); // setup the two players
	setup_player_two();

	num_banks = count;

 	for (i=0; i<NUM_PARTICLE; i++)	// Setup the particle system
	{
		particle_y[i] = rand()%100;
	}

	place_building.ready_to_place_building = NO;
	place_building.x = player_one.hq_info.get_x();
	place_building.y = player_one.hq_info.get_y();
	place_building.z = player_one.hq_info.get_z() + 10;

	open_gl.fp_x = -player_one.hq_info.get_x();
	open_gl.fp_y = player_one.hq_info.get_y() - 350;
	open_gl.fp_z = -player_one.hq_info.get_z();

	radar_cam.set_x(15.0);			// Setup the radar camera
	radar_cam.set_y(-1500.0);
	radar_cam.set_z(15.0);

//	glInitNames();
	glEnable(GL_NORMALIZE);			// Allow GL normalising, which affects lighting. 

	glEnable(GL_CLIP_PLANE1);		// Enable clipping, though im sure this sint doing anything, however it may
	glEnable(GL_CLIP_PLANE2);		// depend on the system its running on.
	glEnable(GL_CLIP_PLANE3);
	glEnable(GL_CLIP_PLANE4);
	glEnable(GL_CLIP_PLANE5);

	glColorMaterial(1, 1);				// This command means that the surface colours of the objects will not be affected by the light,
										// otherwise OpenGL tends to render everything in a greyscale
	glEnable(GL_COLOR_MATERIAL);
	glCullFace(GL_BACK);
}

////////////////////////////////////////////////////////////////////
// SETUP PLAYER ONE FUNCTION
// -------------------------
//
// Sets all the parameters for player one 
//
////////////////////////////////////////////////////////////////////

void setup_player_one (void)
{
	player_one.num_units = 0;
	player_one.num_barracks = 0;
	player_one.num_guard_towers = 0;
	player_one.num_war_factories = 0;
	player_one.num_tanks = 0;
	player_one.num_troopers = 0;
	player_one.num_artillery = 0;
	player_one.player_buildings.num_of_buildings = 0;

	player_one.score = 0;
	player_one.num_of_banks_controlled = 0;
	player_one.name = (char*)"Steve";
	player_one.hq_info.set_hit_points(5000);

	player_one.player_units[0].set_unit_id(TANK);

	player_one.player_units[0].set_x(player_one.hq_info.get_x());
	player_one.player_units[0].set_y(player_one.hq_info.get_y());
	player_one.player_units[0].set_z(player_one.hq_info.get_z() + 40);

	player_one.player_units[0].set_target_x(player_one.player_units[0].get_x());
	player_one.player_units[0].set_target_y(player_one.player_units[0].get_y());
	player_one.player_units[0].set_target_z(player_one.player_units[0].get_z());

	player_one.player_units[0].set_speed(TANK_SPEED);
	player_one.player_units[0].set_hit_points(1750);
	player_one.player_units[0].set_attack_points(1000);
	player_one.num_units++;
	player_one.num_tanks++;

	player_one.player_units[1].set_unit_id(ARTILLERY);

	player_one.player_units[1].set_x(player_one.hq_info.get_x() + 35);
	player_one.player_units[1].set_y(player_one.hq_info.get_y());
	player_one.player_units[1].set_z(player_one.hq_info.get_z() + 40);

	player_one.player_units[1].set_target_x(player_one.player_units[1].get_x());
	player_one.player_units[1].set_target_y(player_one.player_units[1].get_y());
	player_one.player_units[1].set_target_z(player_one.player_units[1].get_z());

	player_one.player_units[1].set_speed(ARTILLERY_SPEED);
	player_one.player_units[1].set_hit_points(600);
	player_one.player_units[1].set_attack_points(300);
	player_one.num_units++;
	player_one.num_tanks++;
/*
	player_one.player_buildings.buildings[0].set_id(ID_TOWER);
	player_one.player_buildings.buildings[0].set_x(player_one.hq_info.get_x() - 75);
	player_one.player_buildings.buildings[0].set_y(player_one.hq_info.get_y());
	player_one.player_buildings.buildings[0].set_z(player_one.hq_info.get_z());
	player_one.num_guard_towers++;
	player_one.player_buildings.num_of_buildings++;*/
}

////////////////////////////////////////////////////////////////////
// SETUP PLAYER TWO FUNCTION
// -------------------------
//
// Sets all the parameters for player two 
//
////////////////////////////////////////////////////////////////////

void setup_player_two (void)
{
	player_two.num_units = 0;
	player_two.num_barracks = 0;
	player_two.num_guard_towers = 0;
	player_two.num_war_factories = 0;
	player_two.num_tanks = 0;
	player_two.num_troopers = 0;
	player_two.num_artillery = 0;
	player_two.player_buildings.num_of_buildings = 0;

	player_two.score = 0;
	player_two.num_of_banks_controlled = 0;
	player_two.name = (char*)"Kaiser Willhelm";
	player_two.hq_info.set_hit_points(1000);

	player_two.player_units[0].set_unit_id(TANK);

	player_two.player_units[0].set_x(player_two.hq_info.get_x());
	player_two.player_units[0].set_y(player_two.hq_info.get_y());
	player_two.player_units[0].set_z(player_two.hq_info.get_z() + 40);

	player_two.player_units[0].set_target_x(player_two.player_units[0].get_x());
	player_two.player_units[0].set_target_y(player_two.player_units[0].get_y());
	player_two.player_units[0].set_target_z(player_two.player_units[0].get_z());

	player_two.player_units[0].set_speed(TANK_SPEED);
	player_two.player_units[0].set_hit_points(1750);
	player_two.player_units[0].set_attack_points(100);
	player_two.num_units++;

	player_two.player_units[1].set_unit_id(TANK);

	player_two.player_units[1].set_x(player_two.hq_info.get_x() + 35);
	player_two.player_units[1].set_y(player_two.hq_info.get_y());
	player_two.player_units[1].set_z(player_two.hq_info.get_z() + 40);

	player_two.player_units[1].set_target_x(player_two.player_units[1].get_x());
	player_two.player_units[1].set_target_y(player_two.player_units[1].get_y());
	player_two.player_units[1].set_target_z(player_two.player_units[1].get_z());

	player_two.player_units[1].set_speed(TANK_SPEED);
	player_two.player_units[1].set_hit_points(1750);
	player_two.player_units[1].set_attack_points(100);
	player_two.num_units++;
}

////////////////////////////////////////////////////////////////////
// LOAD MODELS FUNCTION
// --------------------
//
// Loads all the models for the game.
//
////////////////////////////////////////////////////////////////////

void load_models (void)
{
	if (climate == 0 || climate == 1)	// Loads trees depending on climate. Plam if desert, pine for grass and ice
	{
		load_ase_file((char*)"Data/Models/tree_pine.ase", TREE);
	}
	else if (climate == 2)
	{
		load_ase_file((char*)"Data/Models/tree_palm.ase", TREE);
	}

	load_ase_file((char*)"Data/Models/hq.ase", HQ);
	load_ase_file((char*)"Data/Models/bank.ase", BANK);
	load_ase_file((char*)"Data/Models/barracks.ase", BARRACKS);
	load_ase_file((char*)"Data/Models/tank.ase", TANK);
	load_ase_file((char*)"Data/Models/war_factory.ase", WAR_FACTORY);
	load_ase_file((char*)"Data/Models/guard_tower.ase", TOWER);
	load_ase_file((char*)"Data/Models/artillery.ase", ARTILLERY);
	load_ase_file((char*)"Data/Models/trooper.ase", TROOPER);
}

////////////////////////////////////////////////////////////////////
// LOAD_ASE_FILE FUNCTION
// ----------------------
//
// Loads the ASE files into the data structures setup for the models.
// Calculates the bounding box for each model. These are object aligned
// bounding boxes as opposed to axis aligned. this can allow for a more
// realistic collision detection algorithm, but is harder to implement.
// 
////////////////////////////////////////////////////////////////////

void load_ase_file(char *file, int model)
{
	char temp[64];
	int i, j;
	float temp_float;
	int temp_int;

	ifstream ase (file);

	if (! ase.is_open())			// Give an error if it didnt open
	{
		cout << "ERROR - The file failed to open." << endl;
		cout << "Did you remember the .MAP file extension?." << endl;
		return;
	}
	else
	{
		cout << "Loading " << file << endl;
	}

	switch (model)
	{
		case TREE: /********* READ the number of vertices **********/
					for (i=0; i<15; i++)
					{
						ase >> temp[i];
					}

					ase >> tree.num_vertices;

					/********* READ the face data **********/
					for (i=0; i<14; i++)
					{
						ase >> temp[i];
					}

					ase >> tree.num_faces;

					/********* READ the vertex list data **********/
					for (i=0; i<18; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<tree.num_vertices; i++)
					{
						for (j=0; j<12; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_float;

						ase >> temp_float;
						tree.vertices[i].set_x(temp_float);

						ase >> temp_float;
						tree.vertices[i].set_y(temp_float);

						ase >> temp_float;
						tree.vertices[i].set_z(temp_float);
					}

					/********* READ the face list data **********/
					for (i=0; i<17; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<tree.num_faces; i++)
					{
						for (j=0; j<10; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;

						for (j=0; j<3; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						tree.faces[i].set_vertex_one(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						tree.faces[i].set_vertex_two(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						tree.faces[i].set_vertex_three(temp_int);

						for (j=0; j<40; j++)
						{
							ase >> temp[j];
						}
					}

					calculate_bounding_box(TREE);

					cout << "Tree Loaded" << endl;

					break;

		case HQ: /********* READ the number of vertices **********/
					for (i=0; i<15; i++)
					{
						ase >> temp[i];
					}

					ase >> hq.num_vertices;

					/********* READ the face data **********/
					for (i=0; i<14; i++)
					{
						ase >> temp[i];
					}

					ase >> hq.num_faces;

					/********* READ the vertex list data **********/
					for (i=0; i<18; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<hq.num_vertices; i++)
					{
						for (j=0; j<12; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_float;

						ase >> temp_float;
						hq.vertices[i].set_x(temp_float);

						ase >> temp_float;
						hq.vertices[i].set_y(temp_float);

						ase >> temp_float;
						hq.vertices[i].set_z(temp_float);
					}

					/********* READ the face list data **********/
					for (i=0; i<17; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<hq.num_faces; i++)
					{
						for (j=0; j<10; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;

						for (j=0; j<3; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						hq.faces[i].set_vertex_one(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						hq.faces[i].set_vertex_two(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						hq.faces[i].set_vertex_three(temp_int);

						for (j=0; j<40; j++)
						{
							ase >> temp[j];
						}
					}

					calculate_bounding_box(HQ);

					cout << "HQ Loaded" << endl;

					break;

		case BANK: /********* READ the number of vertices **********/
					for (i=0; i<15; i++)
					{
						ase >> temp[i];
					}

					ase >> bank.num_vertices;

					/********* READ the face data **********/
					for (i=0; i<14; i++)
					{
						ase >> temp[i];
					}

					ase >> bank.num_faces;

					/********* READ the vertex list data **********/
					for (i=0; i<18; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<bank.num_vertices; i++)
					{
						for (j=0; j<12; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_float;

						ase >> temp_float;
						bank.vertices[i].set_x(temp_float);

						ase >> temp_float;
						bank.vertices[i].set_y(temp_float);

						ase >> temp_float;
						bank.vertices[i].set_z(temp_float);
					}

					/********* READ the face list data **********/
					for (i=0; i<17; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<bank.num_faces; i++)
					{
						for (j=0; j<10; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;

						for (j=0; j<3; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						bank.faces[i].set_vertex_one(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						bank.faces[i].set_vertex_two(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						bank.faces[i].set_vertex_three(temp_int);

						for (j=0; j<40; j++)
						{
							ase >> temp[j];
						}
					}

					calculate_bounding_box(BANK);

					cout << "Bank Loaded" << endl;

					break;

		case BARRACKS: /********* READ the number of vertices **********/
					for (i=0; i<15; i++)
					{
						ase >> temp[i];
					}

					ase >> barracks.num_vertices;

					/********* READ the face data **********/
					for (i=0; i<14; i++)
					{
						ase >> temp[i];
					}

					ase >> barracks.num_faces;

					/********* READ the vertex list data **********/
					for (i=0; i<18; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<barracks.num_vertices; i++)
					{
						for (j=0; j<12; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_float;

						ase >> temp_float;
						barracks.vertices[i].set_x(temp_float);

						ase >> temp_float;
						barracks.vertices[i].set_y(temp_float);

						ase >> temp_float;
						barracks.vertices[i].set_z(temp_float);
					}

					/********* READ the face list data **********/
					for (i=0; i<17; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<barracks.num_faces; i++)
					{
						for (j=0; j<10; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;

						for (j=0; j<3; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						barracks.faces[i].set_vertex_one(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						barracks.faces[i].set_vertex_two(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						barracks.faces[i].set_vertex_three(temp_int);

						for (j=0; j<40; j++)
						{
							ase >> temp[j];
						}
					}

					calculate_bounding_box(BARRACKS);

					cout << "Barracks Loaded" << endl;

					break;

		case TANK: /********* READ the number of vertices **********/
					for (i=0; i<15; i++)
					{
						ase >> temp[i];
					}

					ase >> tank.num_vertices;

					/********* READ the face data **********/
					for (i=0; i<14; i++)
					{
						ase >> temp[i];
					}

					ase >> tank.num_faces;

					/********* READ the vertex list data **********/
					for (i=0; i<18; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<tank.num_vertices; i++)
					{
						for (j=0; j<12; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_float;

						ase >> temp_float;
						tank.vertices[i].set_x(temp_float);

						ase >> temp_float;
						tank.vertices[i].set_y(temp_float);

						ase >> temp_float;
						tank.vertices[i].set_z(temp_float);
					}

					/********* READ the face list data **********/
					for (i=0; i<17; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<tank.num_faces; i++)
					{
						for (j=0; j<10; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;

						for (j=0; j<3; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						tank.faces[i].set_vertex_one(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						tank.faces[i].set_vertex_two(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						tank.faces[i].set_vertex_three(temp_int);

						for (j=0; j<40; j++)
						{
							ase >> temp[j];
						}
					}

					calculate_bounding_box(TANK);

					cout << "Tank Loaded" << endl;

					break;

		case WAR_FACTORY: 
			
					/********* READ the number of vertices **********/
					for (i=0; i<15; i++)
					{
						ase >> temp[i];
					}

					ase >> war_factory.num_vertices;

					/********* READ the face data **********/
					for (i=0; i<14; i++)
					{
						ase >> temp[i];
					}

					ase >> war_factory.num_faces;

					/********* READ the vertex list data **********/
					for (i=0; i<18; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<war_factory.num_vertices; i++)
					{
						for (j=0; j<12; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_float;

						ase >> temp_float;
						war_factory.vertices[i].set_x(temp_float);

						ase >> temp_float;
						war_factory.vertices[i].set_y(temp_float);

						ase >> temp_float;
						war_factory.vertices[i].set_z(temp_float);
					}

					/********* READ the face list data **********/
					for (i=0; i<17; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<war_factory.num_faces; i++)
					{
						for (j=0; j<10; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;

						for (j=0; j<3; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						war_factory.faces[i].set_vertex_one(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						war_factory.faces[i].set_vertex_two(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						war_factory.faces[i].set_vertex_three(temp_int);

						for (j=0; j<40; j++)
						{
							ase >> temp[j];
						}
					}

					calculate_bounding_box(WAR_FACTORY);

					cout << "War Factory Loaded" << endl;

					break;

		case TOWER: 
			
					/********* READ the number of vertices **********/
					for (i=0; i<15; i++)
					{
						ase >> temp[i];
					}

					ase >> tower.num_vertices;

					/********* READ the face data **********/
					for (i=0; i<14; i++)
					{
						ase >> temp[i];
					}

					ase >> tower.num_faces;

					/********* READ the vertex list data **********/
					for (i=0; i<18; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<tower.num_vertices; i++)
					{
						for (j=0; j<12; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_float;

						ase >> temp_float;
						tower.vertices[i].set_x(temp_float);

						ase >> temp_float;
						tower.vertices[i].set_y(temp_float);

						ase >> temp_float;
						tower.vertices[i].set_z(temp_float);
					}

					/********* READ the face list data **********/
					for (i=0; i<17; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<tower.num_faces; i++)
					{
						for (j=0; j<10; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;

						for (j=0; j<3; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						tower.faces[i].set_vertex_one(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						tower.faces[i].set_vertex_two(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						tower.faces[i].set_vertex_three(temp_int);

						for (j=0; j<40; j++)
						{
							ase >> temp[j];
						}
					}

					calculate_bounding_box(TOWER);

					cout << "Guard Tower Loaded" << endl;

					break;

		case ARTILLERY: 
			
					/********* READ the number of vertices **********/
					for (i=0; i<15; i++)
					{
						ase >> temp[i];
					}

					ase >> artillery.num_vertices;

					/********* READ the face data **********/
					for (i=0; i<14; i++)
					{
						ase >> temp[i];
					}

					ase >> artillery.num_faces;

					/********* READ the vertex list data **********/
					for (i=0; i<18; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<artillery.num_vertices; i++)
					{
						for (j=0; j<12; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_float;

						ase >> temp_float;
						artillery.vertices[i].set_x(temp_float);

						ase >> temp_float;
						artillery.vertices[i].set_y(temp_float);

						ase >> temp_float;
						artillery.vertices[i].set_z(temp_float);
					}

					/********* READ the face list data **********/
					for (i=0; i<17; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<artillery.num_faces; i++)
					{
						for (j=0; j<10; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;

						for (j=0; j<3; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						artillery.faces[i].set_vertex_one(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						artillery.faces[i].set_vertex_two(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						artillery.faces[i].set_vertex_three(temp_int);

						for (j=0; j<40; j++)
						{
							ase >> temp[j];
						}
					}

					calculate_bounding_box(ARTILLERY);

					cout << "Artillery Loaded" << endl;

					break;

		case TROOPER: 
			
					/********* READ the number of vertices **********/
					for (i=0; i<15; i++)
					{
						ase >> temp[i];
					}

					ase >> trooper.num_vertices;

					/********* READ the face data **********/
					for (i=0; i<14; i++)
					{
						ase >> temp[i];
					}

					ase >> trooper.num_faces;

					/********* READ the vertex list data **********/
					for (i=0; i<18; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<trooper.num_vertices; i++)
					{
						for (j=0; j<12; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_float;

						ase >> temp_float;
						trooper.vertices[i].set_x(temp_float);

						ase >> temp_float;
						trooper.vertices[i].set_y(temp_float);

						ase >> temp_float;
						trooper.vertices[i].set_z(temp_float);
					}

					/********* READ the face list data **********/
					for (i=0; i<17; i++)
					{
						ase >> temp[i];
					}

					for (i=0; i<trooper.num_faces; i++)
					{
						for (j=0; j<10; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;

						for (j=0; j<3; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						trooper.faces[i].set_vertex_one(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						trooper.faces[i].set_vertex_two(temp_int);

						for (j=0; j<2; j++)
						{
							ase >> temp[j];
						}

						ase >> temp_int;
						trooper.faces[i].set_vertex_three(temp_int);

						for (j=0; j<40; j++)
						{
							ase >> temp[j];
						}
					}

					calculate_bounding_box(TROOPER);

					cout << "Trooper Loaded" << endl;

					break;

		default:	break;
	}

	cout << endl << endl;

	ase.close();	// Close the file.
}

////////////////////////////////////////////////////////////////////
// CALCULATE_BOUNDING_BOX FUNCTION
// -------------------------------
//
// Basically for each model this function searches through all the points
// and therefore calculates the eight points of the models bounding box.
//
////////////////////////////////////////////////////////////////////

void calculate_bounding_box (int model)
{
	int i;

	float max_x = 0;
	float max_y = 0;
	float max_z = 0;

	float min_x = 999;
	float min_y = 999;
	float min_z = 999;

	switch (model)
	{
		case TREE:	for (i=0; i<tree.num_vertices; i++)
					{
						if (tree.vertices[i].get_x() >= max_x)
						{
							max_x = tree.vertices[i].get_x();
						}
						else if (tree.vertices[i].get_x() <= min_x)
						{
							min_x = tree.vertices[i].get_x();
						}


						if (tree.vertices[i].get_y() >= max_y)
						{
							max_y = tree.vertices[i].get_y();
						}
						else if (tree.vertices[i].get_y() <= min_y)
						{
							min_y = tree.vertices[i].get_y();
						}

						if (tree.vertices[i].get_z() >= max_z)
						{
							max_z = tree.vertices[i].get_z();
						}
						else if (tree.vertices[i].get_z() <= min_z)
						{
							min_z = tree.vertices[i].get_z();
						}
					}

					tree.bounding_box[0].set_x(max_x);
					tree.bounding_box[0].set_y(max_y);
					tree.bounding_box[0].set_z(max_z);

					tree.bounding_box[1].set_x(min_x);
					tree.bounding_box[1].set_y(min_y);
					tree.bounding_box[1].set_z(min_z);

					tree.bounding_box[2].set_x(max_x);
					tree.bounding_box[2].set_y(min_y);
					tree.bounding_box[2].set_z(min_z);

					tree.bounding_box[3].set_x(min_x);
					tree.bounding_box[3].set_y(max_y);
					tree.bounding_box[3].set_z(min_z);

					tree.bounding_box[4].set_x(min_x);
					tree.bounding_box[4].set_y(min_y);
					tree.bounding_box[4].set_z(max_z);

					tree.bounding_box[5].set_x(min_x);
					tree.bounding_box[5].set_y(max_y);
					tree.bounding_box[5].set_z(max_z);

					tree.bounding_box[6].set_x(max_x);
					tree.bounding_box[6].set_y(min_y);
					tree.bounding_box[6].set_z(max_z);

					tree.bounding_box[7].set_x(max_x);
					tree.bounding_box[7].set_y(max_y);
					tree.bounding_box[7].set_z(min_z);

					cout << "Tree bounding box calculated" << endl;

					break;

		case HQ:	for (i=0; i<hq.num_vertices; i++)
					{
						if (hq.vertices[i].get_x() >= max_x)
						{
							max_x = hq.vertices[i].get_x();
						}
						else if (hq.vertices[i].get_x() <= min_x)
						{
							min_x = hq.vertices[i].get_x();
						}

						if (hq.vertices[i].get_y() >= max_y)
						{
							max_y = hq.vertices[i].get_y();
						}
						else if (hq.vertices[i].get_y() <= min_y)
						{
							min_y = hq.vertices[i].get_y();
						}

						if (hq.vertices[i].get_z() >= max_z)
						{
							max_z = hq.vertices[i].get_z();
						}
						else if (hq.vertices[i].get_z() <= min_z)
						{
							min_z = hq.vertices[i].get_z();
						}
					}

					hq.bounding_box[0].set_x(max_x);
					hq.bounding_box[0].set_y(max_y);
					hq.bounding_box[0].set_z(max_z);

					hq.bounding_box[1].set_x(min_x);
					hq.bounding_box[1].set_y(min_y);
					hq.bounding_box[1].set_z(min_z);

					hq.bounding_box[2].set_x(max_x);
					hq.bounding_box[2].set_y(min_y);
					hq.bounding_box[2].set_z(min_z);

					hq.bounding_box[3].set_x(min_x);
					hq.bounding_box[3].set_y(max_y);
					hq.bounding_box[3].set_z(min_z);

					hq.bounding_box[4].set_x(min_x);
					hq.bounding_box[4].set_y(min_y);
					hq.bounding_box[4].set_z(max_z);

					hq.bounding_box[5].set_x(min_x);
					hq.bounding_box[5].set_y(max_y);
					hq.bounding_box[5].set_z(max_z);

					hq.bounding_box[6].set_x(max_x);
					hq.bounding_box[6].set_y(min_y);
					hq.bounding_box[6].set_z(max_z);

					hq.bounding_box[7].set_x(max_x);
					hq.bounding_box[7].set_y(max_y);
					hq.bounding_box[7].set_z(min_z);

					cout << "HQ bounding box calculated" << endl;

					break;

		case BANK:	for (i=0; i<bank.num_vertices; i++)
					{
						if (bank.vertices[i].get_x() >= max_x)
						{
							max_x = bank.vertices[i].get_x();
						}
						else if (bank.vertices[i].get_x() <= min_x)
						{
							min_x = bank.vertices[i].get_x();
						}

						if (bank.vertices[i].get_y() >= max_y)
						{
							max_y = bank.vertices[i].get_y();
						}
						else if (bank.vertices[i].get_y() <= min_y)
						{
							min_y = bank.vertices[i].get_y();
						}

						if (bank.vertices[i].get_z() >= max_z)
						{
							max_z = bank.vertices[i].get_z();
						}
						else if (bank.vertices[i].get_z() <= min_z)
						{
							min_z = bank.vertices[i].get_z();
						}
					}

					bank.bounding_box[0].set_x(max_x);
					bank.bounding_box[0].set_y(max_y);
					bank.bounding_box[0].set_z(max_z);

					bank.bounding_box[1].set_x(min_x);
					bank.bounding_box[1].set_y(min_y);
					bank.bounding_box[1].set_z(min_z);

					bank.bounding_box[2].set_x(max_x);
					bank.bounding_box[2].set_y(min_y);
					bank.bounding_box[2].set_z(min_z);

					bank.bounding_box[3].set_x(min_x);
					bank.bounding_box[3].set_y(max_y);
					bank.bounding_box[3].set_z(min_z);

					bank.bounding_box[4].set_x(min_x);
					bank.bounding_box[4].set_y(min_y);
					bank.bounding_box[4].set_z(max_z);

					bank.bounding_box[5].set_x(min_x);
					bank.bounding_box[5].set_y(max_y);
					bank.bounding_box[5].set_z(max_z);

					bank.bounding_box[6].set_x(max_x);
					bank.bounding_box[6].set_y(min_y);
					bank.bounding_box[6].set_z(max_z);

					bank.bounding_box[7].set_x(max_x);
					bank.bounding_box[7].set_y(max_y);
					bank.bounding_box[7].set_z(min_z);

					cout << "Bank bounding box calculated" << endl;

					break;

		case BARRACKS:	
					for (i=0; i<barracks.num_vertices; i++)
					{
						if (barracks.vertices[i].get_x() >= max_x)
						{
							max_x = barracks.vertices[i].get_x();
						}
						else if (barracks.vertices[i].get_x() <= min_x)
						{
							min_x = barracks.vertices[i].get_x();
						}

						if (barracks.vertices[i].get_y() >= max_y)
						{
							max_y = barracks.vertices[i].get_y();
						}
						else if (barracks.vertices[i].get_y() <= min_y)
						{
							min_y = barracks.vertices[i].get_y();
						}

						if (barracks.vertices[i].get_z() >= max_z)
						{
							max_z = barracks.vertices[i].get_z();
						}
						else if (barracks.vertices[i].get_z() <= min_z)
						{
							min_z = barracks.vertices[i].get_z();
						}
					}

					barracks.bounding_box[0].set_x(max_x);
					barracks.bounding_box[0].set_y(max_y);
					barracks.bounding_box[0].set_z(max_z);

					barracks.bounding_box[1].set_x(min_x);
					barracks.bounding_box[1].set_y(min_y);
					barracks.bounding_box[1].set_z(min_z);

					barracks.bounding_box[2].set_x(max_x);
					barracks.bounding_box[2].set_y(min_y);
					barracks.bounding_box[2].set_z(min_z);

					barracks.bounding_box[3].set_x(min_x);
					barracks.bounding_box[3].set_y(max_y);
					barracks.bounding_box[3].set_z(min_z);

					barracks.bounding_box[4].set_x(min_x);
					barracks.bounding_box[4].set_y(min_y);
					barracks.bounding_box[4].set_z(max_z);

					barracks.bounding_box[5].set_x(min_x);
					barracks.bounding_box[5].set_y(max_y);
					barracks.bounding_box[5].set_z(max_z);

					barracks.bounding_box[6].set_x(max_x);
					barracks.bounding_box[6].set_y(min_y);
					barracks.bounding_box[6].set_z(max_z);

					barracks.bounding_box[7].set_x(max_x);
					barracks.bounding_box[7].set_y(max_y);
					barracks.bounding_box[7].set_z(min_z);

					cout << "Barracks bounding box calculated" << endl;

					break;

		case TANK:	for (i=0; i<tank.num_vertices; i++)
					{
						if (tank.vertices[i].get_x() >= max_x)
						{
							max_x = tank.vertices[i].get_x();
						}
						else if (tank.vertices[i].get_x() <= min_x)
						{
							min_x = tank.vertices[i].get_x();
						}

						if (tank.vertices[i].get_y() >= max_y)
						{
							max_y = tank.vertices[i].get_y();
						}
						else if (tank.vertices[i].get_y() <= min_y)
						{
							min_y = tank.vertices[i].get_y();
						}

						if (tank.vertices[i].get_z() >= max_z)
						{
							max_z = tank.vertices[i].get_z();
						}
						else if (tank.vertices[i].get_z() <= min_z)
						{
							min_z = tank.vertices[i].get_z();
						}
					}

					tank.bounding_box[0].set_x(max_x);
					tank.bounding_box[0].set_y(max_y);
					tank.bounding_box[0].set_z(max_z);

					tank.bounding_box[1].set_x(min_x);
					tank.bounding_box[1].set_y(min_y);
					tank.bounding_box[1].set_z(min_z);

					tank.bounding_box[2].set_x(max_x);
					tank.bounding_box[2].set_y(min_y);
					tank.bounding_box[2].set_z(min_z);

					tank.bounding_box[3].set_x(min_x);
					tank.bounding_box[3].set_y(max_y);
					tank.bounding_box[3].set_z(min_z);

					tank.bounding_box[4].set_x(min_x);
					tank.bounding_box[4].set_y(min_y);
					tank.bounding_box[4].set_z(max_z);

					tank.bounding_box[5].set_x(min_x);
					tank.bounding_box[5].set_y(max_y);
					tank.bounding_box[5].set_z(max_z);

					tank.bounding_box[6].set_x(max_x);
					tank.bounding_box[6].set_y(min_y);
					tank.bounding_box[6].set_z(max_z);

					tank.bounding_box[7].set_x(max_x);
					tank.bounding_box[7].set_y(max_y);
					tank.bounding_box[7].set_z(min_z);

					cout << "Tank bounding box calculated" << endl;

					break;

		case WAR_FACTORY:	for (i=0; i<war_factory.num_vertices; i++)
					{
						if (war_factory.vertices[i].get_x() >= max_x)
						{
							max_x = war_factory.vertices[i].get_x();
						}
						else if (war_factory.vertices[i].get_x() <= min_x)
						{
							min_x = war_factory.vertices[i].get_x();
						}

						if (war_factory.vertices[i].get_y() >= max_y)
						{
							max_y = war_factory.vertices[i].get_y();
						}
						else if (war_factory.vertices[i].get_y() <= min_y)
						{
							min_y = war_factory.vertices[i].get_y();
						}

						if (war_factory.vertices[i].get_z() >= max_z)
						{
							max_z = war_factory.vertices[i].get_z();
						}
						else if (war_factory.vertices[i].get_z() <= min_z)
						{
							min_z = war_factory.vertices[i].get_z();
						}
					}

					war_factory.bounding_box[0].set_x(max_x);
					war_factory.bounding_box[0].set_y(max_y);
					war_factory.bounding_box[0].set_z(max_z);

					war_factory.bounding_box[1].set_x(min_x);
					war_factory.bounding_box[1].set_y(min_y);
					war_factory.bounding_box[1].set_z(min_z);

					war_factory.bounding_box[2].set_x(max_x);
					war_factory.bounding_box[2].set_y(min_y);
					war_factory.bounding_box[2].set_z(min_z);

					war_factory.bounding_box[3].set_x(min_x);
					war_factory.bounding_box[3].set_y(max_y);
					war_factory.bounding_box[3].set_z(min_z);

					war_factory.bounding_box[4].set_x(min_x);
					war_factory.bounding_box[4].set_y(min_y);
					war_factory.bounding_box[4].set_z(max_z);

					war_factory.bounding_box[5].set_x(min_x);
					war_factory.bounding_box[5].set_y(max_y);
					war_factory.bounding_box[5].set_z(max_z);

					war_factory.bounding_box[6].set_x(max_x);
					war_factory.bounding_box[6].set_y(min_y);
					war_factory.bounding_box[6].set_z(max_z);

					war_factory.bounding_box[7].set_x(max_x);
					war_factory.bounding_box[7].set_y(max_y);
					war_factory.bounding_box[7].set_z(min_z);

					cout << "War factory bounding box calculated" << endl;

					break;

		case TOWER:	for (i=0; i<tower.num_vertices; i++)
					{
						if (tower.vertices[i].get_x() >= max_x)
						{
							max_x = tower.vertices[i].get_x();
						}
						else if (tower.vertices[i].get_x() <= min_x)
						{
							min_x = tower.vertices[i].get_x();
						}

						if (tower.vertices[i].get_y() >= max_y)
						{
							max_y = tower.vertices[i].get_y();
						}
						else if (tower.vertices[i].get_y() <= min_y)
						{
							min_y = tower.vertices[i].get_y();
						}

						if (tower.vertices[i].get_z() >= max_z)
						{
							max_z = tower.vertices[i].get_z();
						}
						else if (tower.vertices[i].get_z() <= min_z)
						{
							min_z = tower.vertices[i].get_z();
						}
					}

					tower.bounding_box[0].set_x(max_x);
					tower.bounding_box[0].set_y(max_y);
					tower.bounding_box[0].set_z(max_z);

					tower.bounding_box[1].set_x(min_x);
					tower.bounding_box[1].set_y(min_y);
					tower.bounding_box[1].set_z(min_z);

					tower.bounding_box[2].set_x(max_x);
					tower.bounding_box[2].set_y(min_y);
					tower.bounding_box[2].set_z(min_z);

					tower.bounding_box[3].set_x(min_x);
					tower.bounding_box[3].set_y(max_y);
					tower.bounding_box[3].set_z(min_z);

					tower.bounding_box[4].set_x(min_x);
					tower.bounding_box[4].set_y(min_y);
					tower.bounding_box[4].set_z(max_z);

					tower.bounding_box[5].set_x(min_x);
					tower.bounding_box[5].set_y(max_y);
					tower.bounding_box[5].set_z(max_z);

					tower.bounding_box[6].set_x(max_x);
					tower.bounding_box[6].set_y(min_y);
					tower.bounding_box[6].set_z(max_z);

					tower.bounding_box[7].set_x(max_x);
					tower.bounding_box[7].set_y(max_y);
					tower.bounding_box[7].set_z(min_z);

					cout << "Guard Tower bounding box calculated" << endl;

					break;

		case ARTILLERY:	for (i=0; i<artillery.num_vertices; i++)
					{
						if (artillery.vertices[i].get_x() >= max_x)
						{
							max_x = artillery.vertices[i].get_x();
						}
						else if (artillery.vertices[i].get_x() <= min_x)
						{
							min_x = artillery.vertices[i].get_x();
						}

						if (artillery.vertices[i].get_y() >= max_y)
						{
							max_y = artillery.vertices[i].get_y();
						}
						else if (artillery.vertices[i].get_y() <= min_y)
						{
							min_y = artillery.vertices[i].get_y();
						}

						if (artillery.vertices[i].get_z() >= max_z)
						{
							max_z = artillery.vertices[i].get_z();
						}
						else if (artillery.vertices[i].get_z() <= min_z)
						{
							min_z = artillery.vertices[i].get_z();
						}
					}

					artillery.bounding_box[0].set_x(max_x);
					artillery.bounding_box[0].set_y(max_y);
					artillery.bounding_box[0].set_z(max_z);

					artillery.bounding_box[1].set_x(min_x);
					artillery.bounding_box[1].set_y(min_y);
					artillery.bounding_box[1].set_z(min_z);

					artillery.bounding_box[2].set_x(max_x);
					artillery.bounding_box[2].set_y(min_y);
					artillery.bounding_box[2].set_z(min_z);

					artillery.bounding_box[3].set_x(min_x);
					artillery.bounding_box[3].set_y(max_y);
					artillery.bounding_box[3].set_z(min_z);

					artillery.bounding_box[4].set_x(min_x);
					artillery.bounding_box[4].set_y(min_y);
					artillery.bounding_box[4].set_z(max_z);

					artillery.bounding_box[5].set_x(min_x);
					artillery.bounding_box[5].set_y(max_y);
					artillery.bounding_box[5].set_z(max_z);

					artillery.bounding_box[6].set_x(max_x);
					artillery.bounding_box[6].set_y(min_y);
					artillery.bounding_box[6].set_z(max_z);

					artillery.bounding_box[7].set_x(max_x);
					artillery.bounding_box[7].set_y(max_y);
					artillery.bounding_box[7].set_z(min_z);

					cout << "Artillery bounding box calculated" << endl;

					break;

		case TROOPER:	for (i=0; i<trooper.num_vertices; i++)
					{
						if (trooper.vertices[i].get_x() >= max_x)
						{
							max_x = trooper.vertices[i].get_x();
						}
						else if (trooper.vertices[i].get_x() <= min_x)
						{
							min_x = trooper.vertices[i].get_x();
						}

						if (trooper.vertices[i].get_y() >= max_y)
						{
							max_y = trooper.vertices[i].get_y();
						}
						else if (trooper.vertices[i].get_y() <= min_y)
						{
							min_y = trooper.vertices[i].get_y();
						}

						if (trooper.vertices[i].get_z() >= max_z)
						{
							max_z = trooper.vertices[i].get_z();
						}
						else if (trooper.vertices[i].get_z() <= min_z)
						{
							min_z = trooper.vertices[i].get_z();
						}
					}

					trooper.bounding_box[0].set_x(max_x);
					trooper.bounding_box[0].set_y(max_y);
					trooper.bounding_box[0].set_z(max_z);

					trooper.bounding_box[1].set_x(min_x);
					trooper.bounding_box[1].set_y(min_y);
					trooper.bounding_box[1].set_z(min_z);

					trooper.bounding_box[2].set_x(max_x);
					trooper.bounding_box[2].set_y(min_y);
					trooper.bounding_box[2].set_z(min_z);

					trooper.bounding_box[3].set_x(min_x);
					trooper.bounding_box[3].set_y(max_y);
					trooper.bounding_box[3].set_z(min_z);

					trooper.bounding_box[4].set_x(min_x);
					trooper.bounding_box[4].set_y(min_y);
					trooper.bounding_box[4].set_z(max_z);

					trooper.bounding_box[5].set_x(min_x);
					trooper.bounding_box[5].set_y(max_y);
					trooper.bounding_box[5].set_z(max_z);

					trooper.bounding_box[6].set_x(max_x);
					trooper.bounding_box[6].set_y(min_y);
					trooper.bounding_box[6].set_z(max_z);

					trooper.bounding_box[7].set_x(max_x);
					trooper.bounding_box[7].set_y(max_y);
					trooper.bounding_box[7].set_z(min_z);

					cout << "Trooper bounding box calculated" << endl;

					break;

		default:	break;
	}
}

////////////////////////////////////////////////////////////////////
// LOAD_CAMERA_SETTINGS FUNCTION
// -----------------------------
//
// Loads the initial camera settings from camera.ini
//
////////////////////////////////////////////////////////////////////

void load_camera_settings (void)
{
	ifstream camera ("Data/Settings/camera.ini");

	camera >> open_gl.fp_x;
	camera >> open_gl.fp_y;
	camera >> open_gl.fp_z;
	camera >> open_gl.x_pos;

	camera.close();
}

////////////////////////////////////////////////////////////////////
// LOAD_LIGHT_SETTINGS FUNCTION
// ----------------------------
//
// Loads the intial light settings from light.ini
//
////////////////////////////////////////////////////////////////////

void load_light_settings (void)
{
	ifstream light ("Data/Settings/light.ini");

	light >> ambience;
	light >> white_light;

	light.close();
}

////////////////////////////////////////////////////////////////////
// LOAD_MAP FUNCTION
// -----------------
//
// Loads the map data for the game. A switch loads the correct map.
// The file format .MAP was created for use with the game to distinguish
// the map files from other input files. These are created by Trench Edit!
//
////////////////////////////////////////////////////////////////////

void load_map (void)
{
	char temp[32];
	int i, j;
	ifstream map;

	if (map_name == SAMPLE)
	{
		 map.open("Data/Maps/sample.map");
	}

	else if (map_name == VERDUN)
	{
		map.open("Data/Maps/heightmap.map");
	}

	else if (map_name == PASS)
	{
		map.open("Data/Maps/pass.map");
	}

	else if (map_name == GB)
	{
		map.open("Data/Maps/britain.map");
	}

	else if (map_name == RUSSIA)
	{
		map.open("Data/Maps/russia.map");
	}

	if (! map.is_open())			// Give an error if it didnt open
	{
		cout << "ERROR - The file failed to open." << endl;
		cout << "Did you remember the .MAP file extension?." << endl;
		return;
	}
	else
	{
		cout << "Loading map" << endl;
		cout << "Please be patient - This process can take a while" << endl;
	}

	cout << ".";

	for (i=0; i<15; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		map >> temp[i];
	}

	cout << ".";

	map >> player_one_start;

	for (i=0; i<15; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		map >> temp[i];
	}
	cout << ".";
	map >> player_two_start;
	cout << ".";
	for (i=0; i<8; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		map >> temp[i];
	}
	cout << ".";
	map >> climate;
	cout << ".";
	for (i=0; i<4; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		map >> temp[i];
	}
	cout << ".";
	map >> fog_tag;
	map >> fog_start;
	map >> fog_end;
	cout << ".";
	for (i=0; i<11; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		map >> temp[i];
	}
	cout << ".";
	map >> water_y;
	cout << ".";
	for (i=0; i<9; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		map >> temp[i];
	}
	cout << ".";
	map >> grid_size;
	cout << ".";
	for (i=0; i<9; i++) //cycle through file text. It is there to help anyone using the editor to understand the output.
	{
		map >> temp[i];
	}

	for (i=0; i<grid_size; i++)
	{
		cout << ".";
		for (j=0; j<grid_size; j++)
		{
			map >> terrain[i][j].id_number;
			map >> terrain[i][j].tree;
			map >> terrain[i][j].bank;
			map >> terrain[i][j].x;
			map >> terrain[i][j].y;
			map >> terrain[i][j].z;
		}
	}

	map.close();	// Close the file.

	cout << "Map Loaded" << endl << endl;
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
	open_gl.gl_reshape(width, height);
}

////////////////////////////////////////////////////////////////////
// IDLE FUNCTION
// -------------
//
// This is the function where the game world model is updated.
// This is called when not rendering.
//
////////////////////////////////////////////////////////////////////

void idle (void)
{
	static int search_switch = 0;
	static clock_t old_time;
	clock_t current_time = clock();	
	time_passed = (float)current_time / (float)CLOCKS_PER_SEC;
	int temp;
	int i, j;

	glutSetWindow(window_main);

	if (game_state == GAME_MAIN_LOOP)
	{
		glutSetWindow(window_main);

		srand(time_passed);

		open_gl.gl_idle();

		for (i=0; i<player_one.num_units; i++)	// Orientate the unit towards the target
		{
			
				player_one.player_units[i].set_orientation (calculate_orientation (player_one.player_units[i].get_x(), 
																		  		   player_one.player_units[i].get_y(), 
																				   player_one.player_units[i].get_z(),
																				   player_one.player_units[i].get_target_x(),
																				   player_one.player_units[i].get_target_y(), 
																				   player_one.player_units[i].get_target_z()));
		//	if (i==0)
		//		cout << player_one.player_units[i].get_orientation() << endl;
			
		}

		for (j=0; j<player_two.num_units; j++)	// Orientate the unit towards the target
		{
			player_two.player_units[j].set_orientation (calculate_orientation (player_two.player_units[j].get_x(), 
																		  	   player_two.player_units[j].get_y(), 
																			   player_two.player_units[j].get_z(),
																			   player_two.player_units[j].get_target_x(),
																			   player_two.player_units[j].get_target_y(), 
																			   player_two.player_units[j].get_target_z()));
		//	if (j==0)
		//		cout << player_two.player_units[j].get_orientation() << endl;
		}

		ai.run_ai_model(player_two.money,			// Run the artificial intelligence model
						player_two.num_barracks,
						player_two.num_war_factories,
						player_two.num_guard_towers);

		if (ai.get_state() == GATHER_RESOURCES && search_switch == 0)	// If gather resources then find the nearest bank.
		{
			search_for_best_bank();
			search_switch = 1;
		}
		else if (ai.get_state() == BUILD_BARRACKS && build_switch == FALSE) // otherwise, build things
		{
			for (i=0; i<NUM_BUILDINGS; i++)
			{
				if (player_two.player_buildings.buildings[i].get_id() == ID_FREE && build_switch == FALSE)
				{
					player_two.player_buildings.buildings[i].set_id(ID_BARRACKS);
					cpu_building_number = i;
					build_switch = TRUE;
					cout << "CPU Building Barracks" << endl;
				}
			}
		}
		else if (ai.get_state() == BUILD_WAR_FACTORY && build_switch == FALSE)
		{
			for (i=0; i<NUM_BUILDINGS; i++)
			{
				if (player_two.player_buildings.buildings[i].get_id() == ID_FREE && build_switch == FALSE)
				{
					player_two.player_buildings.buildings[i].set_id(ID_WAR_FACTORY);
					cpu_building_number = i;
					build_switch = TRUE;
					cout << "CPU Building War Factory" << endl;
				}
			}
		}
		else if (ai.get_state() == BUILD_TOWER && build_switch == FALSE)
		{
			for (i=0; i<NUM_BUILDINGS; i++)
			{
				if (player_two.player_buildings.buildings[i].get_id() == ID_FREE && build_switch == FALSE)
				{
					player_two.player_buildings.buildings[i].set_id(ID_TOWER);
					cpu_building_number = i;
					build_switch = TRUE;
					cout << "CPU Building Guard Tower" << endl;
				}
			}
		}

		if (ai.get_unit_to_build() == TANK && unit_build_switch == FALSE)
		{
			cout << "CPU Building Tank" << endl;

			for (i=0; i<NUM_UNITS; i++)
			{
				if (player_two.player_units[i].get_unit_id() == FREE && unit_build_switch == FALSE)
				{
					player_two.player_units[i].set_unit_id(TANK);
					cpu_unit_number = i;

					unit_build_switch = TRUE;
				}
			}
		}
		else if (ai.get_unit_to_build() == ARTILLERY && unit_build_switch == FALSE)
		{
			cout << "CPU Building Artillery" << endl;

			for (i=0; i<NUM_UNITS; i++)
			{
				if (player_two.player_units[i].get_unit_id() == FREE && unit_build_switch == FALSE)
				{
					player_two.player_units[i].set_unit_id(ARTILLERY);
					cpu_unit_number = i;
				
					unit_build_switch = TRUE;
				}
			}

		}
		else if (ai.get_unit_to_build() == TROOPER && unit_build_switch == FALSE)
		{
			cout << "CPU Building Trooper" << endl;

			for (i=0; i<NUM_UNITS; i++)
			{
				if (player_two.player_units[i].get_unit_id() == FREE && unit_build_switch == FALSE)
				{
					player_two.player_units[i].set_unit_id(TROOPER);
					player_two.player_units[i].set_z(99999);
					cpu_unit_number = i;
				
					unit_build_switch = TRUE;
				}
			}

		}

		previous_time = time_passed - old_time;
	
		move_units();		// Moive the units to their targets

		temp = distance_between_two_vectors(player_two.player_units[0].get_x(), player_two.player_units[0].get_y(), player_two.player_units[0].get_z(), 
											player_two.player_units[0].get_target_x(), player_two.player_units[0].get_target_y(),player_two.player_units[0].get_target_z());

		if (temp < 20)
		{
			search_switch = 0;
		}

		update_banks();		// Update banks

		if (previous_time > 3)
		{
			old_time += previous_time;
			previous_time = 0;

			player_one.money += (player_one.num_of_banks_controlled * 10);
			player_two.money += (player_two.num_of_banks_controlled * 10);

			for (i=0; i<player_one.num_units; i++)
			{
				for (j=0; j<player_two.num_units; j++)
				{
					if (player_one.player_units[i].get_unit_id() == TANK)
					{
						if (distance_between_two_vectors(player_one.player_units[i].get_x(), player_one.player_units[i].get_y(), player_one.player_units[i].get_z(),
														player_two.player_units[j].get_x(), player_two.player_units[j].get_y(), player_two.player_units[j].get_z()) < 100
														&& player_two.player_units[j].get_unit_id() != FREE)
						{
							if (player_two.player_units[j].get_hit_points() > 0)
							{
								player_two.player_units[j].damage_unit(player_one.player_units[i].get_attack_points());
								////PlaySound("Data/Sounds/KABOOM.wav",NULL,SND_FILENAME | SND_ASYNC  );
								cout << "Tank attacking" << endl;
							}
							else
							{
								player_two.player_units[j].set_unit_id(FREE);
								player_two.num_units--;
								cout << "Unit destroyed " << j << endl;
							}
						}
					}

					if (player_one.player_units[i].get_unit_id() == TROOPER)
					{
						if (distance_between_two_vectors(player_one.player_units[i].get_x(), player_one.player_units[i].get_y(), player_one.player_units[i].get_z(),
														player_two.player_units[j].get_x(), player_two.player_units[j].get_y(), player_two.player_units[j].get_z()) < 75
														&& player_two.player_units[j].get_unit_id() != FREE)
						{
							if (player_two.player_units[j].get_hit_points() > 0)
							{
								player_two.player_units[j].damage_unit(player_one.player_units[i].get_attack_points());
								//PlaySound("Data/Sounds/KABOOM.wav",NULL,SND_FILENAME | SND_ASYNC  );
								cout << "Trooper attacking" << endl;
							}
							else
							{
								player_two.player_units[j].set_unit_id(FREE);
								player_two.num_units--;
								cout << "Unit destroyed " << j << endl;
							}
						}
					}

					if (player_one.player_units[i].get_unit_id() == ARTILLERY)
					{
						if (distance_between_two_vectors(player_one.player_units[i].get_x(), player_one.player_units[i].get_y(), player_one.player_units[i].get_z(),
														player_two.player_units[j].get_x(), player_two.player_units[j].get_y(), player_two.player_units[j].get_z()) < 300
														&& player_two.player_units[j].get_unit_id() != FREE)
						{
							if (player_two.player_units[j].get_hit_points() > 0)
							{
								player_two.player_units[j].damage_unit(player_one.player_units[i].get_attack_points());
								//PlaySound("Data/Sounds/KABOOM.wav",NULL,SND_FILENAME | SND_ASYNC  );
								cout << "Artillery attacking" << endl;
							}
							else
							{
								player_two.player_units[j].set_unit_id(FREE);
								player_two.num_units--;
								cout << "Unit destroyed " << j << endl;
							}
						}
					}
				}
			}

			//////// PLAYER TWO ATTACK //////////////////////

			for (i=0; i<player_two.num_units; i++)
			{
				for (j=0; j<player_one.num_units; j++)
				{
					if (player_two.player_units[i].get_unit_id() == TANK)
					{
						if (distance_between_two_vectors(player_two.player_units[i].get_x(), player_two.player_units[i].get_y(), player_two.player_units[i].get_z(),
														player_one.player_units[j].get_x(), player_one.player_units[j].get_y(), player_one.player_units[j].get_z()) < 100
														&& player_one.player_units[j].get_unit_id() != FREE)
						{
							if (player_one.player_units[j].get_hit_points() > 0)
							{
								player_one.player_units[j].damage_unit(player_two.player_units[i].get_attack_points());
								//PlaySound("Data/Sounds/KABOOM.wav",NULL,SND_FILENAME | SND_ASYNC  );
								cout << "Tank attacking" << endl;
							}
							else
							{
								player_one.player_units[j].set_unit_id(FREE);
								player_one.num_units--;
								cout << "Unit destroyed " << j << endl;
							}
						}
					}

					if (player_two.player_units[i].get_unit_id() == TROOPER)
					{
						if (distance_between_two_vectors(player_two.player_units[i].get_x(), player_two.player_units[i].get_y(), player_two.player_units[i].get_z(),
														player_one.player_units[j].get_x(), player_one.player_units[j].get_y(), player_one.player_units[j].get_z()) < 75
														&& player_one.player_units[j].get_unit_id() != FREE)
						{
							if (player_one.player_units[j].get_hit_points() > 0)
							{
								player_one.player_units[j].damage_unit(player_two.player_units[i].get_attack_points());
								//PlaySound("Data/Sounds/KABOOM.wav",NULL,SND_FILENAME | SND_ASYNC  );
								cout << "Trooper attacking" << endl;
							}
							else
							{
								player_one.player_units[j].set_unit_id(FREE);
								player_one.num_units--;
								cout << "Unit destroyed " << j << endl;
							}
						}
					}

					if (player_two.player_units[i].get_unit_id() == ARTILLERY)
					{
						if (distance_between_two_vectors(player_two.player_units[i].get_x(), player_two.player_units[i].get_y(), player_two.player_units[i].get_z(),
														player_one.player_units[j].get_x(), player_one.player_units[j].get_y(), player_one.player_units[j].get_z()) < 300
														&& player_one.player_units[j].get_unit_id() != FREE)
						{
							if (player_one.player_units[j].get_hit_points() > 0)
							{
								player_one.player_units[j].damage_unit(player_two.player_units[i].get_attack_points());
								//PlaySound("Data/Sounds/KABOOM.wav",NULL,SND_FILENAME | SND_ASYNC  );
								cout << "Artillery attacking" << endl;
							}
							else
							{
								player_one.player_units[j].set_unit_id(FREE);
								player_one.num_units--;
								cout << "Unit destroyed " << j << endl;
							}
						}
					}
				}
			}

			for (i=0; i<player_one.player_buildings.num_of_buildings; i++)
			{
				if (player_one.player_buildings.buildings[i].get_id() == ID_TOWER)
				{
					for (j=0; j<player_two.num_units; j++)
					{
						if (distance_between_two_vectors(player_one.player_buildings.buildings[i].get_x(), player_one.player_buildings.buildings[i].get_y(), player_one.player_buildings.buildings[i].get_z(),
													player_two.player_units[j].get_x(), player_two.player_units[j].get_y(), player_two.player_units[j].get_z()) < 100
													&& player_two.player_units[j].get_unit_id() != FREE)
						{
							if (player_two.player_units[j].get_hit_points() > 0)
							{
								player_two.player_units[j].damage_unit(100);
								//PlaySound("Data/Sounds/MINIGUN.wav",NULL,SND_FILENAME | SND_ASYNC  );
							}
							else
							{
								player_two.player_units[j].set_unit_id(FREE);
								player_two.num_units--;
								cout << "Unit destroyed " << j << endl;
							}
						}
					}
				}
			}

			for (i=0; i<player_two.player_buildings.num_of_buildings; i++)
			{
				if (player_two.player_buildings.buildings[i].get_id() == ID_TOWER)
				{
					for (j=0; j<player_one.num_units; j++)
					{
						if (distance_between_two_vectors(player_two.player_buildings.buildings[i].get_x(), player_two.player_buildings.buildings[i].get_y(), player_two.player_buildings.buildings[i].get_z(),
													player_one.player_units[j].get_x(), player_one.player_units[j].get_y(), player_one.player_units[j].get_z()) < 100
													&& player_one.player_units[j].get_unit_id() != FREE)
						{
							if (player_one.player_units[j].get_hit_points() > 0)
							{
								player_one.player_units[j].damage_unit(100);
								//PlaySound("Data/Sounds/MINIGUN.wav",NULL,SND_FILENAME | SND_ASYNC  );
							}
							else
							{
								player_one.player_units[j].set_unit_id(FREE);
								player_one.num_units--;
								cout << "Unit destroyed " << j << endl;
							}
						}
					}
				}
			}
	
			if (player_two.num_units > 10 && ai.stance == AGGRESIVE)
			{
				for (i=0; i<player_two.num_units; i++)
				{
					player_two.player_units[i].set_target_x(player_one.hq_info.get_x());
					player_two.player_units[i].set_target_y(player_one.hq_info.get_y());
					player_two.player_units[i].set_target_z(player_one.hq_info.get_z());
				}
			}
		}
/*
		for (i=0; i<NUM_PARTICLE; i++)
		{
			particle_y[i] += sin(i);

			if(particle_y[i] > 100)
			{
				particle_y[i] = 0;
			}
		}
*/
		glutPostRedisplay();

		if (create_building == TRUE)	// If creating a building, run the algorithm
		{
			build();
		}

		if (create_unit == TRUE)		// If creating a unit, run the algorithm	
		{
			build_units();
		}

		if (build_switch == TRUE)		// If creating a cpu building, run the algorithm
		{	
			build_cpu();
		}

		if (unit_build_switch == TRUE)	// If creating a cpu unit, run the algorithm
		{
			build_cpu_units();
		}
	}

	glui_subwin->sync_live();

	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////
// BUILD CPU UNITS FUNCTION
// ------------------------
//
// Builds the cpus units.
//
////////////////////////////////////////////////////////////////////

void build_cpu_units (void)
{
	int i, temp;

	temp = 0;

	if (player_two.player_units[cpu_unit_number].get_unit_id() == TANK)
	{
		if (player_two.money > 0 && player_two.player_units[cpu_unit_number].get_hit_points() < 1750)
		{
			player_two.player_units[cpu_unit_number].set_hit_points(player_two.player_units[cpu_unit_number].get_hit_points() +1);
			player_two.money -= 1;
		}
		else if (player_two.player_units[cpu_unit_number].get_hit_points() == 1750)
		{
			player_two.player_units[cpu_unit_number].set_unit_id(TANK);
			player_two.player_units[cpu_unit_number].set_speed(TANK_SPEED);
			player_two.player_units[cpu_unit_number].set_attack_points(TANK_ATTACK);
			player_two.score += 10;
			player_two.num_units++;

			for (i=0; i<player_two.player_buildings.num_of_buildings; i++)
			{
				if (player_two.player_buildings.buildings[i].get_id() == WAR_FACTORY)
				{
					player_two.player_units[cpu_unit_number].set_x(player_two.player_buildings.buildings[i].get_x());
					player_two.player_units[cpu_unit_number].set_y(player_two.player_buildings.buildings[i].get_y());
					player_two.player_units[cpu_unit_number].set_z(player_two.player_buildings.buildings[i].get_z());

					player_two.player_units[cpu_unit_number].set_target_x(player_two.player_buildings.buildings[i].get_x() + (rand()%150 - 75));
					player_two.player_units[cpu_unit_number].set_target_y(player_two.player_buildings.buildings[i].get_y());
					player_two.player_units[cpu_unit_number].set_target_z(player_two.player_buildings.buildings[i].get_z() + (rand()%150 - 75));
					
					temp = 1;
				}
			}

			//PlaySound("Data/Sounds/APPEAR1.wav",NULL,SND_FILENAME | SND_ASYNC );
			unit_build_switch = FALSE;
		}
	}

	temp = 0;

	if (player_two.player_units[cpu_unit_number].get_unit_id() == ARTILLERY)
	{
		if (player_two.money > 0 && player_two.player_units[cpu_unit_number].get_hit_points() < 600)
		{
			player_two.player_units[cpu_unit_number].set_hit_points(player_two.player_units[cpu_unit_number].get_hit_points() +1);
			player_two.money -= 1;
		}
		else if (player_two.player_units[cpu_unit_number].get_hit_points() == 600)
		{
			player_two.player_units[cpu_unit_number].set_unit_id(ARTILLERY);
			player_two.player_units[cpu_unit_number].set_speed(ARTILLERY_SPEED);
			player_two.player_units[cpu_unit_number].set_attack_points(ARTILLERY_ATTACK);
			player_two.score += 10;
			player_two.num_units++;

			for (i=0; i<player_two.player_buildings.num_of_buildings; i++)
			{
				if (player_two.player_buildings.buildings[i].get_id() == WAR_FACTORY)
				{
					player_two.player_units[cpu_unit_number].set_x(player_two.player_buildings.buildings[i].get_x());
					player_two.player_units[cpu_unit_number].set_y(player_two.player_buildings.buildings[i].get_y());
					player_two.player_units[cpu_unit_number].set_z(player_two.player_buildings.buildings[i].get_z());

					player_two.player_units[cpu_unit_number].set_target_x(player_two.player_buildings.buildings[i].get_x() + (rand()%150 - 75));
					player_two.player_units[cpu_unit_number].set_target_y(player_two.player_buildings.buildings[i].get_y());
					player_two.player_units[cpu_unit_number].set_target_z(player_two.player_buildings.buildings[i].get_z() + (rand()%150 - 75));
					
					temp = 1;
				}
			}

			//PlaySound("Data/Sounds/APPEAR1.wav",NULL,SND_FILENAME | SND_ASYNC );
			unit_build_switch = FALSE;
		}
	}

	temp = 0;

	if (player_two.player_units[cpu_unit_number].get_unit_id() == TROOPER)
	{
		if (player_two.money > 0 && player_two.player_units[cpu_unit_number].get_hit_points() < 200)
		{
			player_two.player_units[cpu_unit_number].set_hit_points(player_two.player_units[cpu_unit_number].get_hit_points() +1);
			player_two.money -= 1;
		}
		else if (player_two.player_units[cpu_unit_number].get_hit_points() == 200)
		{
			player_two.player_units[cpu_unit_number].set_unit_id(TROOPER);
			player_two.player_units[cpu_unit_number].set_speed(TROOPER_SPEED);
			player_two.player_units[cpu_unit_number].set_attack_points(TROOPER_ATTACK);
			player_two.score += 10;
			player_two.num_units++;

			for (i=0; i<player_two.player_buildings.num_of_buildings; i++)
			{
				if (player_two.player_buildings.buildings[i].get_id() == ID_BARRACKS && temp == 0)
				{
					player_two.player_units[cpu_unit_number].set_x(player_two.player_buildings.buildings[i].get_x());
					player_two.player_units[cpu_unit_number].set_y(player_two.player_buildings.buildings[i].get_y());
					player_two.player_units[cpu_unit_number].set_z(player_two.player_buildings.buildings[i].get_z());

					player_two.player_units[cpu_unit_number].set_target_x(player_two.player_buildings.buildings[i].get_x() + (rand()%150 - 75));
					player_two.player_units[cpu_unit_number].set_target_y(player_two.player_buildings.buildings[i].get_y());
					player_two.player_units[cpu_unit_number].set_target_z(player_two.player_buildings.buildings[i].get_z() + (rand()%150 - 75));
					
					temp = 1;
				}
			}

			cout << player_two.player_units[cpu_unit_number].get_x() << " " << player_two.player_units[cpu_unit_number].get_y() << " " << player_two.player_units[cpu_unit_number].get_z() << endl;

			//PlaySound("Data/Sounds/APPEAR1.wav",NULL,SND_FILENAME | SND_ASYNC );
			unit_build_switch = FALSE;
		}
	}
}

////////////////////////////////////////////////////////////////////
// BUILD CPU FUNCTION
// ------------------
//
// Builds the cpu's buildings
//
////////////////////////////////////////////////////////////////////

void build_cpu (void)
{
	static int temp = 0;
	int random;

	if (player_two.player_buildings.buildings[cpu_building_number].get_id() == ID_BARRACKS)
	{
		if (player_two.money > 0 && player_two.player_buildings.buildings[cpu_building_number].get_hit_points() < 500)
		{
			player_two.player_buildings.buildings[cpu_building_number].set_hit_points(player_two.player_buildings.buildings[cpu_building_number].get_hit_points() +1);
			player_two.money -= 1;
		}
		else if (player_two.player_buildings.buildings[cpu_building_number].get_hit_points() == 500)
		{
			player_two.num_barracks++;
			player_two.player_buildings.num_of_buildings++;
			player_two.score += 10;
			player_two.player_buildings.buildings[cpu_building_number].set_x(player_two.hq_info.get_x() + 100);
			player_two.player_buildings.buildings[cpu_building_number].set_y(player_two.hq_info.get_y());
			player_two.player_buildings.buildings[cpu_building_number].set_z(player_two.hq_info.get_z() + (rand()%100 - 50));
			//PlaySound("Data/Sounds/RADARON2.wav",NULL,SND_FILENAME | SND_ASYNC );
			build_switch = FALSE;
		}
	}

	if (player_two.player_buildings.buildings[cpu_building_number].get_id() == ID_WAR_FACTORY)
	{
		if (player_two.money > 0 && player_two.player_buildings.buildings[cpu_building_number].get_hit_points() < 2000)
		{
			player_two.player_buildings.buildings[cpu_building_number].set_hit_points(player_two.player_buildings.buildings[cpu_building_number].get_hit_points() +1);
			player_two.money -= 1;
		}
		else if (player_two.player_buildings.buildings[cpu_building_number].get_hit_points() == 2000)
		{
			player_two.num_war_factories++;
			player_two.player_buildings.num_of_buildings++;
			player_two.score += 10;
			player_two.player_buildings.buildings[cpu_building_number].set_x(player_two.hq_info.get_x() - 100);
			player_two.player_buildings.buildings[cpu_building_number].set_y(player_two.hq_info.get_y());
			player_two.player_buildings.buildings[cpu_building_number].set_z(player_two.hq_info.get_z() + (rand()%100 - 50));
			//PlaySound("Data/Sounds/RADARON2.wav",NULL,SND_FILENAME | SND_ASYNC );
			build_switch = FALSE;
		}
	}

	if (player_two.player_buildings.buildings[cpu_building_number].get_id() == ID_TOWER)
	{
		if (player_two.money > 0 && player_two.player_buildings.buildings[cpu_building_number].get_hit_points() < 750)
		{
			player_two.player_buildings.buildings[cpu_building_number].set_hit_points(player_two.player_buildings.buildings[cpu_building_number].get_hit_points() +1);
			player_two.money -= 1;
		}
		else if (player_two.player_buildings.buildings[cpu_building_number].get_hit_points() == 750)
		{
			player_two.num_guard_towers++;
			player_two.player_buildings.num_of_buildings++;
			player_two.score += 10;

			random = rand()%4;

			// Basically, switch between the north, south, east and west sides of the cpu base and build guard towers
			switch (random)						
			{
				case 0:		player_two.player_buildings.buildings[cpu_building_number].set_x(player_two.hq_info.get_x() + 150);
							player_two.player_buildings.buildings[cpu_building_number].set_y(player_two.hq_info.get_y());
							player_two.player_buildings.buildings[cpu_building_number].set_z(player_two.hq_info.get_z() + (rand()%300 - 150));

							break;

				case 1:		player_two.player_buildings.buildings[cpu_building_number].set_x(player_two.hq_info.get_x() - 150);
							player_two.player_buildings.buildings[cpu_building_number].set_y(player_two.hq_info.get_y());
							player_two.player_buildings.buildings[cpu_building_number].set_z(player_two.hq_info.get_z() + (rand()%300 - 150));

							break;

				case 2:		player_two.player_buildings.buildings[cpu_building_number].set_x(player_two.hq_info.get_x() + (rand()%300 - 150));
							player_two.player_buildings.buildings[cpu_building_number].set_y(player_two.hq_info.get_y());
							player_two.player_buildings.buildings[cpu_building_number].set_z(player_two.hq_info.get_z() + 150);

							break;

				case 3:		player_two.player_buildings.buildings[cpu_building_number].set_x(player_two.hq_info.get_x() + (rand()%300 - 150));
							player_two.player_buildings.buildings[cpu_building_number].set_y(player_two.hq_info.get_y());
							player_two.player_buildings.buildings[cpu_building_number].set_z(player_two.hq_info.get_z() - 150);

							break;

				default:	break;
			}
/*
			if (random == 0)
			{
				player_two.player_buildings.buildings[cpu_building_number].set_x(player_two.hq_info.get_x() + 150);
				player_two.player_buildings.buildings[cpu_building_number].set_y(player_two.hq_info.get_y());
				player_two.player_buildings.buildings[cpu_building_number].set_z(player_two.hq_info.get_z() + (rand()%300 - 150));
			}
			if (random == 0)
			{
				player_two.player_buildings.buildings[cpu_building_number].set_x(player_two.hq_info.get_x() + 150);
				player_two.player_buildings.buildings[cpu_building_number].set_y(player_two.hq_info.get_y());
				player_two.player_buildings.buildings[cpu_building_number].set_z(player_two.hq_info.get_z() + (rand()%300 - 150));
			}
*/
			//PlaySound("Data/Sounds/RADARON2.wav",NULL,SND_FILENAME | SND_ASYNC );
			build_switch = FALSE;
		}
	}
}

////////////////////////////////////////////////////////////////////
// BUILD UNITS FUNCTION
// --------------------
//
// Builds units for the player.
//
////////////////////////////////////////////////////////////////////

void build_units (void)
{
	static int temp = 0;
	int i;

	if (current_unit_to_build == TANK)
	{
		if (player_one.money > 0 && player_one.player_units[current_unit_number].get_hit_points() < 1750)
		{
			player_one.player_units[current_unit_number].set_hit_points(player_one.player_units[current_unit_number].get_hit_points() +1);
			player_one.money -= 1;
			if (temp == 0)
			{
				//PlaySound("Data/Sounds/CASHDN1.wav",NULL,SND_FILENAME | SND_ASYNC  );
				temp = 1;
			}
			else if (temp == 1)
			{
				temp = 0;
			}
		}
		else if (player_one.player_units[current_unit_number].get_hit_points() == 1750)
		{
			create_unit = FALSE;
			current_unit_to_build = ID_FREE;
			player_one.num_units++;
			//PlaySound("Data/Sounds/BUZZY1.wav",NULL,SND_FILENAME | SND_ASYNC );

			for (i=0; i<player_one.player_buildings.num_of_buildings; i++)
			{
				if (player_one.player_buildings.buildings[i].get_id() == ID_WAR_FACTORY)
				{
					player_one.player_units[current_unit_number].set_x(player_one.player_buildings.buildings[i].get_x());
					player_one.player_units[current_unit_number].set_y(player_one.player_buildings.buildings[i].get_y());
					player_one.player_units[current_unit_number].set_z(player_one.player_buildings.buildings[i].get_z());
				}
			}

			player_one.score += 10;
		}
	}

	if (current_unit_to_build == TROOPER)
	{
		if (player_one.money > 0 && player_one.player_units[current_unit_number].get_hit_points() < 200)
		{
			player_one.player_units[current_unit_number].set_hit_points(player_one.player_units[current_unit_number].get_hit_points() +1);
			player_one.money -= 1;
			if (temp == 0)
			{
				//PlaySound("Data/Sounds/CASHDN1.wav",NULL,SND_FILENAME | SND_ASYNC  );
				temp = 1;
			}
			else if (temp == 1)
			{
				temp = 0;
			}
		}
		else if (player_one.player_units[current_unit_number].get_hit_points() == 200)
		{
			create_unit = FALSE;
			current_unit_to_build = ID_TROOPER;
			player_one.num_units++;
			//PlaySound("Data/Sounds/BUZZY1.wav",NULL,SND_FILENAME | SND_ASYNC );

			for (i=0; i<player_one.player_buildings.num_of_buildings; i++)
			{
				if (player_one.player_buildings.buildings[i].get_id() == ID_BARRACKS)
				{
					player_one.player_units[current_unit_number].set_x(player_one.player_buildings.buildings[i].get_x());
					player_one.player_units[current_unit_number].set_y(player_one.player_buildings.buildings[i].get_y());
					player_one.player_units[current_unit_number].set_z(player_one.player_buildings.buildings[i].get_z());
				}
			}

			player_one.score += 10;
		}
	}

	if (current_unit_to_build == ARTILLERY)
	{
		if (player_one.money > 0 && player_one.player_units[current_unit_number].get_hit_points() < 600)
		{
			player_one.player_units[current_unit_number].set_hit_points(player_one.player_units[current_unit_number].get_hit_points() +1);
			player_one.money -= 1;
			if (temp == 0)
			{
				//PlaySound("Data/Sounds/CASHDN1.wav",NULL,SND_FILENAME | SND_ASYNC  );
				temp = 1;
			}
			else if (temp == 1)
			{
				temp = 0;
			}
		}
		else if (player_one.player_units[current_unit_number].get_hit_points() == 600)
		{
			create_unit = FALSE;
			current_unit_to_build = ID_ARTILLERY;
			player_one.num_units++;
			//PlaySound("Data/Sounds/BUZZY1.wav",NULL,SND_FILENAME | SND_ASYNC );

			for (i=0; i<player_one.player_buildings.num_of_buildings; i++)
			{
				if (player_one.player_buildings.buildings[i].get_id() == ID_WAR_FACTORY)
				{
					player_one.player_units[current_unit_number].set_x(player_one.player_buildings.buildings[i].get_x());
					player_one.player_units[current_unit_number].set_y(player_one.player_buildings.buildings[i].get_y());
					player_one.player_units[current_unit_number].set_z(player_one.player_buildings.buildings[i].get_z());

					player_one.player_units[current_unit_number].set_target_x(player_one.player_buildings.buildings[i].get_x() + rand()%100 - 50);
					player_one.player_units[current_unit_number].set_target_y(player_one.player_buildings.buildings[i].get_y());
					player_one.player_units[current_unit_number].set_target_z(player_one.player_buildings.buildings[i].get_z() + 50);
				}
			}

			player_one.score += 10;
		}
	}
}

////////////////////////////////////////////////////////////////////
// BUILD FUNCTION
// --------------
//
// Builds the players buildings.
//
////////////////////////////////////////////////////////////////////

void build (void)
{
	static int temp = 0;
//	int i;

	if (create_building == TRUE)
	{
		if (current_building_to_build == ID_BARRACKS)
		{
			if (player_one.money > 0 && player_one.player_buildings.buildings[current_building_number].get_hit_points() < 500)
			{
				player_one.player_buildings.buildings[current_building_number].set_hit_points(player_one.player_buildings.buildings[current_building_number].get_hit_points() +1);
				player_one.money -= 1;
				if (temp == 0)
				{
					//PlaySound("Data/Sounds/CASHDN1.wav",NULL,SND_FILENAME | SND_ASYNC  );
					temp = 1;
				}
				else if (temp == 1)
				{
					temp = 0;
				}
			}
			else if (player_one.player_buildings.buildings[current_building_number].get_hit_points() == 500)
			{
				create_building = FALSE;
				current_building_to_build = ID_FREE;
				player_one.num_barracks++;
				player_one.player_buildings.num_of_buildings++;
				//PlaySound("Data/Sounds/BUZZY1.wav",NULL,SND_FILENAME | SND_ASYNC );
				place_building.ready_to_place_building = YES;
				place_building.building_id = ID_BARRACKS;
				player_one.score += 10;
			}
		}

		if (current_building_to_build == ID_WAR_FACTORY)
		{
			if (player_one.money > 0 && player_one.player_buildings.buildings[current_building_number].get_hit_points() < 2000)
			{
				player_one.player_buildings.buildings[current_building_number].set_hit_points(player_one.player_buildings.buildings[current_building_number].get_hit_points() +1);
				player_one.money -= 1;
				if (temp == 0)
				{
					//PlaySound("Data/Sounds/CASHDN1.wav",NULL,SND_FILENAME | SND_ASYNC  );
					temp = 1;
				}
				else if (temp == 1)
				{
					temp = 0;
				}
			}
			else if (player_one.player_buildings.buildings[current_building_number].get_hit_points() == 2000)
			{
				create_building = FALSE;
				current_building_to_build = ID_FREE;
				player_one.num_war_factories++;
				player_one.player_buildings.num_of_buildings++;
				//PlaySound("Data/Sounds/BUZZY1.wav",NULL,SND_FILENAME | SND_ASYNC );
				place_building.ready_to_place_building = YES;
				place_building.building_id = ID_WAR_FACTORY;
				player_one.score += 30;
			}
		}

		if (current_building_to_build == ID_TOWER)
		{
			if (player_one.money > 0 && player_one.player_buildings.buildings[current_building_number].get_hit_points() < 750)
			{
				player_one.player_buildings.buildings[current_building_number].set_hit_points(player_one.player_buildings.buildings[current_building_number].get_hit_points() +1);
				player_one.money -= 1;
				if (temp == 0)
				{
					//PlaySound("Data/Sounds/CASHDN1.wav",NULL,SND_FILENAME | SND_ASYNC  );
					temp = 1;
				}
				else if (temp == 1)
				{
					temp = 0;
				}
			}
			else if (player_one.player_buildings.buildings[current_building_number].get_hit_points() == 750)
			{
				create_building = FALSE;
				current_building_to_build = ID_FREE;
				player_one.num_guard_towers++;
				player_one.player_buildings.num_of_buildings++;
				//PlaySound("Data/Sounds/BUZZY1.wav",NULL,SND_FILENAME | SND_ASYNC );
				place_building.ready_to_place_building = YES;
				place_building.building_id = ID_TOWER;
				player_one.score += 15;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
// MOVE_UNIT FUNCTION
// ------------------
//
// Moves the units. One of the biggest improvements to this would be
// to include A* pathfinding.
//
////////////////////////////////////////////////////////////////////

void move_units (void)
{
	int i, j, k, l, m;
	float float_temp1, temp;

	for (i=0; i<player_one.num_units; i++)
	{
		if (player_one.player_units[i].get_target_x() < player_one.player_units[i].get_x())
		{
			player_one.player_units[i].set_x(player_one.player_units[i].get_x() - player_one.player_units[i].get_speed());
		}

		if (player_one.player_units[i].get_target_z() < player_one.player_units[i].get_z())
		{
			player_one.player_units[i].set_z(player_one.player_units[i].get_z() - player_one.player_units[i].get_speed());	
		}

		if (player_one.player_units[i].get_target_x() > player_one.player_units[i].get_x())
		{
			player_one.player_units[i].set_x(player_one.player_units[i].get_x() + player_one.player_units[i].get_speed());
		}

		if (player_one.player_units[i].get_target_z() > player_one.player_units[i].get_z())
		{
			player_one.player_units[i].set_z(player_one.player_units[i].get_z() + player_one.player_units[i].get_speed());	
		}

		temp = 999999999;

		for (j=0; j<TERRAIN_SIZE; j++)
		{
			for (k=0; k<TERRAIN_SIZE; k++)
			{
				float_temp1 = distance_between_two_vectors(player_one.player_units[i].get_x(), player_one.player_units[i].get_y(), player_one.player_units[i].get_z(), 
				terrain[j][k].x, terrain[j][k].y, terrain[j][k].z);	

				if (float_temp1 < temp)
				{
					temp = float_temp1;
					player_one.player_units[i].terrain_number = terrain[j][k].id_number;
					player_one.player_units[i].set_y(terrain[j][k].y);
				}
			}
		}

		for (l=0; l<TERRAIN_SIZE; l++)
		{
			for (m=0; m<TERRAIN_SIZE; m++)
			{
				if (terrain[l][m].id_number == player_one.player_units[i].terrain_number)
				{
					player_one.player_units[i].set_y(terrain[l][m].y);
				}
			}
		}
	}

	for (i=0; i<player_two.num_units; i++)
	{
		if (player_two.player_units[i].get_target_x() < player_two.player_units[i].get_x())
		{
			player_two.player_units[i].set_x(player_two.player_units[i].get_x() - player_two.player_units[i].get_speed());
		}

		if (player_two.player_units[i].get_target_y() < player_two.player_units[i].get_y())
		{
			player_two.player_units[i].set_y(player_two.player_units[i].get_y() - player_two.player_units[i].get_speed());
		}

		if (player_two.player_units[i].get_target_z() < player_two.player_units[i].get_z())
		{
			player_two.player_units[i].set_z(player_two.player_units[i].get_z() - player_two.player_units[i].get_speed());	
		}

		if (player_two.player_units[i].get_target_x() > player_two.player_units[i].get_x())
		{
			player_two.player_units[i].set_x(player_two.player_units[i].get_x() + player_two.player_units[i].get_speed());
		}

		if (player_two.player_units[i].get_target_y() > player_two.player_units[i].get_y())
		{
			player_two.player_units[i].set_y(player_two.player_units[i].get_y() + player_two.player_units[i].get_speed());
		}

		if (player_two.player_units[i].get_target_z() > player_two.player_units[i].get_z())
		{
			player_two.player_units[i].set_z(player_two.player_units[i].get_z() + player_two.player_units[i].get_speed());	
		}

		temp = 999999999;

		for (j=0; j<TERRAIN_SIZE; j++)
		{
			for (k=0; k<TERRAIN_SIZE; k++)
			{
				float_temp1 = distance_between_two_vectors(player_two.player_units[i].get_x(), player_two.player_units[i].get_y(), player_two.player_units[i].get_z(), 
				terrain[j][k].x, terrain[j][k].y, terrain[j][k].z);	

				if (float_temp1 < temp)
				{
					temp = float_temp1;
					player_two.player_units[i].terrain_number = terrain[j][k].id_number;
					player_two.player_units[i].set_y(terrain[j][k].y);
				}
			}
		}

		for (l=0; l<TERRAIN_SIZE; l++)
		{
			for (m=0; m<TERRAIN_SIZE; m++)
			{
				if (terrain[l][m].id_number == player_two.player_units[i].terrain_number)
				{
					player_two.player_units[i].set_y(terrain[l][m].y);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
// SEARCH_FOR_BEST_BANK FUNCTION
// -----------------------------
//
// Searches for the best bank by looking at the distace between the 
// the first unit  and each bank, returning the closest bank number.
//
////////////////////////////////////////////////////////////////////

void search_for_best_bank (void)
{
	float closest_distance, distance;
	int i;

	closest_distance = 99999;

	if (search_for_best_bank_switch == 0)
	{
		for (i=0; i<num_banks; i++)
		{
			if (banks[i].get_controller() == NEUTRAL)
			{
				distance = distance_between_two_vectors(banks[i].get_x(), banks[i].get_y(), banks[i].get_z(), 
												 player_two.hq_info.get_x(), player_two.hq_info.get_y(), player_two.hq_info.get_z());
				if (distance < closest_distance)
				{
					best_bank_num = i;
					closest_distance = distance;
					player_two.player_units[0].set_target_x(banks[i].get_x());
					player_two.player_units[0].set_target_y(banks[i].get_y());
					player_two.player_units[0].set_target_z(banks[i].get_z());
				}
			}
		}
	
		search_for_best_bank_switch = 1;
	}
}

////////////////////////////////////////////////////////////////////
// UPDATE_BANKS FUNCTION
// ---------------------
//
// Updates the control of each bank depending on whether a player,
// or computer unit is within its radius. Else the bank is neutral.
//
////////////////////////////////////////////////////////////////////

void update_banks (void)
{
	int i, j, bank_taken;

	for (i=0; i<10; i++)
	{
		for (j=0; j<num_banks; j++)
		{
			if (distance_between_two_vectors(player_one.player_units[i].get_x(), player_one.player_units[i].get_y(), player_one.player_units[i].get_z(), 
											 banks[j].get_x(), banks[j].get_y(), banks[j].get_z()) < 20)
			{
				banks[j].set_controller(PLAYER_ONE_HUMAN);
			}

			if (distance_between_two_vectors(player_two.player_units[i].get_x(), player_two.player_units[i].get_y(), player_two.player_units[i].get_z(), 
											 banks[j].get_x(), banks[j].get_y(), banks[j].get_z()) < 20)
			{
				banks[j].set_controller(PLAYER_TWO_CPU);
				bank_taken = 1;
			}
		}
	}

	if (bank_taken = 1)
	{
		search_for_best_bank_switch = 0;
	}

	player_one.num_of_banks_controlled = 0;
	player_two.num_of_banks_controlled = 0;

	for (i=0; i<num_banks; i++)
	{
		if (banks[i].get_controller() == PLAYER_ONE_HUMAN)
		{
			player_one.num_of_banks_controlled++;
		}
		else if (banks[i].get_controller() == PLAYER_TWO_CPU)
		{
			player_two.num_of_banks_controlled++;
		}
	}
}

////////////////////////////////////////////////////////////////////
// CREATE_GLUI_SUBWINDOW FUNCTION
// ------------------------------
//
// Creates the glui toolbar in the main game window.
//
////////////////////////////////////////////////////////////////////

void create_glui_subwindow (void)
{
	glui_subwin = GLUI_Master.create_glui_subwindow ( window_main, GLUI_SUBWINDOW_RIGHT ); // Master glui object

	GLUI_EditText *fps_edittext =	// Add an uneditable box to put the FPS count in.
		glui_subwin->add_edittext( (char*)"FPS:", GLUI_EDITTEXT_FLOAT, &frames_per_second );
	fps_edittext->disable();

	GLUI_Panel *buildings_panel = glui_subwin->add_panel((char*)"Buildings" );	// Adds a panel
	GLUI_Panel *units_panel = glui_subwin->add_panel((char*) "Units" );

	glutSetWindow(window_main);

	GLUI_Spinner *height_spinner =	// Adds a spinner for camera tilt
		glui_subwin->add_spinner((char*)"Camera Tilt",GLUI_SPINNER_FLOAT, &open_gl.x_pos);
		height_spinner->set_int_limits( -90, 0);

	glui_subwin->add_checkbox((char*)"Terrain Grid", &terrain_grid_tag);	// Terrain grid on?

	glui_subwin->add_button_to_panel(buildings_panel, (char*)"Barracks", ID_BARRACKS, glui_callback);
	glui_subwin->add_button_to_panel(buildings_panel, (char*)"War Factory", ID_WAR_FACTORY, glui_callback);
	glui_subwin->add_button_to_panel(buildings_panel, (char*)"Guard Tower", ID_TOWER, glui_callback);

	glui_subwin->add_button_to_panel(units_panel, (char*) "Tank", ID_TANK, glui_callback);
	glui_subwin->add_button_to_panel(units_panel, (char*)"Trooper", ID_TROOPER, glui_callback);
	glui_subwin->add_button_to_panel(units_panel, (char*)"Artillery", ID_ARTILLERY, glui_callback);

	glui_subwin->add_button((char*)"Quit Trench War!", 0,(GLUI_Update_CB)exit ); // Quit button

//	glui_subwin->sync_live();
	glui_subwin->set_main_gfx_window( window_main );	// Register callbacks 
	GLUI_Master.set_glutIdleFunc( idle );
	GLUI_Master.set_glutReshapeFunc( reshape );
}

////////////////////////////////////////////////////////////////////
// GLUI CALLBACK FUNCTION
// ----------------------
//
// Defines the action to take when the user interacts with the toolbar.
//
////////////////////////////////////////////////////////////////////

void glui_callback (int id)
{
	int i;

	if (id == ID_BARRACKS && current_building_to_build == ID_FREE) 
	{
		for (i=0; i<NUM_BUILDINGS; i++)
		{
			if(player_one.player_buildings.buildings[i].get_id() == ID_FREE)
			{
			//	cout << i << endl;

				player_one.player_buildings.buildings[i].set_id(ID_BARRACKS);

				create_building = TRUE;
				current_building_to_build = ID_BARRACKS;
				current_building_number = i;

				break;
			}
		}
	}
	else if(current_building_to_build == ID_BARRACKS && create_building == TRUE)
	{
		create_building = FALSE;
		current_building_to_build = ID_FREE;
		player_one.player_buildings.buildings[current_building_number].set_id(ID_FREE);
		// Refund players money
		player_one.money += player_one.player_buildings.buildings[current_building_number].get_hit_points();
		player_one.player_buildings.buildings[current_building_number].set_hit_points(0);
		//PlaySound("Data/Sounds/CANCLD1.wav",NULL,SND_FILENAME | SND_ASYNC );
	}

	if (id == ID_WAR_FACTORY && current_building_to_build == ID_FREE) 
	{
		for (i=0; i<NUM_BUILDINGS; i++)
		{
			if(player_one.player_buildings.buildings[i].get_id() == ID_FREE)
			{
			//	cout << i << endl;

				player_one.player_buildings.buildings[i].set_id(ID_WAR_FACTORY);

				create_building = TRUE;
				current_building_to_build = ID_WAR_FACTORY;
				current_building_number = i;

				break;
			}
		}
	}
	else if(current_building_to_build == ID_WAR_FACTORY && create_building == TRUE)
	{
		create_building = FALSE;
		current_building_to_build = ID_FREE;
		player_one.player_buildings.buildings[current_building_number].set_id(ID_FREE);
		// Refund players money
		player_one.money += player_one.player_buildings.buildings[current_building_number].get_hit_points();
		player_one.player_buildings.buildings[current_building_number].set_hit_points(0);
		//PlaySound("Data/Sounds/CANCLD1.wav",NULL,SND_FILENAME | SND_ASYNC );
	}

	if (id == ID_TOWER && current_building_to_build == ID_FREE) 
	{
		for (i=0; i<NUM_BUILDINGS; i++)
		{
			if(player_one.player_buildings.buildings[i].get_id() == ID_FREE)
			{
			//	cout << i << endl;

				player_one.player_buildings.buildings[i].set_id(ID_TOWER);

				create_building = TRUE;
				current_building_to_build = ID_TOWER;
				current_building_number = i;

				break;
			}
		}
	}
	else if(current_building_to_build == ID_TOWER && create_building == TRUE)
	{
		create_building = FALSE;
		current_building_to_build = ID_FREE;
		player_one.player_buildings.buildings[current_building_number].set_id(ID_FREE);
		// Refund players money
		player_one.money += player_one.player_buildings.buildings[current_building_number].get_hit_points();
		player_one.player_buildings.buildings[current_building_number].set_hit_points(0);
		//PlaySound("Data/Sounds/CANCLD1.wav",NULL,SND_FILENAME | SND_ASYNC );
	}

	if (id == ID_TANK) 
	{
		for (i=0; i<NUM_UNITS; i++)
		{
			if(player_one.player_units[i].get_unit_id() == FREE)
			{
			//	cout << i << endl;

				player_one.player_units[i].set_unit_id(TANK);

				create_unit = TRUE;
				current_unit_to_build = TANK;
				current_unit_number = i;

				break;
			}
		}
	}


	if (id == ID_TROOPER) 
	{
		for (i=0; i<NUM_UNITS; i++)
		{
			if(player_one.player_units[i].get_unit_id() == FREE)
			{
			//	cout << i << endl;

				player_one.player_units[i].set_unit_id(TROOPER);

				create_unit = TRUE;
				current_unit_to_build = TROOPER;
				current_unit_number = i;

				break;
			}
		}
	}

	if (id == ID_ARTILLERY) 
	{
		for (i=0; i<NUM_UNITS; i++)
		{
			if(player_one.player_units[i].get_unit_id() == FREE)
			{
			//	cout << i << endl;

				player_one.player_units[i].set_unit_id(ARTILLERY);

				create_unit = TRUE;
				current_unit_to_build = ARTILLERY;
				current_unit_number = i;

				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
// CREATE STARTUP WINDOW FUNCTION
// ------------------------------
//
// Creates the startup window.
//
////////////////////////////////////////////////////////////////////

void create_startup_window (void)
{
	glui_startup_window = GLUI_Master.create_glui ((char*) "Options", 0, 0, 0);

	music = 1;

	GLUI_RadioGroup *map_radio;		// Different radio groups to use.
	GLUI_RadioGroup *music_radio;
	GLUI_RadioGroup *ai_radio;

	GLUI_Panel *maps_panel = glui_startup_window->add_panel((char*)"Maps" );
	GLUI_Panel *music_panel = glui_startup_window->add_panel((char*)"Music" );
	GLUI_Panel *money_panel = glui_startup_window->add_panel((char*) "Money" );
	GLUI_Panel *ai_panel = glui_startup_window->add_panel((char*) "Enemy AI" );

	map_radio = glui_startup_window->add_radiogroup_to_panel(maps_panel, &map_name);	// Different maps
	glui_startup_window->add_radiobutton_to_group( map_radio, (char*)"Sample" );
	glui_startup_window->add_radiobutton_to_group( map_radio, (char*) "Verdun" );
	glui_startup_window->add_radiobutton_to_group( map_radio, (char*)"Passchendaele" );
	glui_startup_window->add_radiobutton_to_group( map_radio, (char*)"Great Britain" );
	glui_startup_window->add_radiobutton_to_group( map_radio, (char*)"Russian Front" );

	glui_startup_window->add_checkbox_to_panel(music_panel, (char*)"On / Off", &music);		// Music on/off

	music_radio = glui_startup_window->add_radiogroup_to_panel(music_panel, &track_name);	// Different music tracks to play
	glui_startup_window->add_radiobutton_to_group( music_radio, (char*)"[0] Lawrence of Arabia" );
	glui_startup_window->add_radiobutton_to_group( music_radio, (char*)"[1] The Orc Horde" );
	glui_startup_window->add_radiobutton_to_group( music_radio, (char*)"[2] The Ride of The Valkyries" );
	glui_startup_window->add_radiobutton_to_group( music_radio, (char*)"[3] Hell March" );
	glui_startup_window->add_radiobutton_to_group( music_radio, (char*)"[4] Radio 2" );

	ai_radio = glui_startup_window->add_radiogroup_to_panel(ai_panel, &ai.stance);	// AI stance
	glui_startup_window->add_radiobutton_to_group( ai_radio, (char*)"Aggresive" );
	glui_startup_window->add_radiobutton_to_group( ai_radio, (char*)"Passive" );

	glui_startup_window->add_column(false);

	GLUI_Spinner *money_player_one_spinner =		// Players money at startup
		glui_startup_window->add_spinner_to_panel(money_panel, (char*)"Player One",GLUI_SPINNER_INT, &player_one.money);
		money_player_one_spinner->set_int_limits( 0, 9999);

	GLUI_Spinner *money_player_two_spinner =
		glui_startup_window->add_spinner_to_panel(money_panel, (char*)"Player Two",GLUI_SPINNER_INT, &player_two.money);
		money_player_two_spinner->set_int_limits( 0, 9999);

	glui_startup_window->add_button((char*)"PLAY!", START_GAME, startup_window_callback);		// Play button

	glui_startup_window->add_button((char*)"Quit Trench War!", 0,(GLUI_Update_CB)exit );		// Quit button

	glui_startup_window->set_main_gfx_window( window_main );
	GLUI_Master.set_glutIdleFunc( idle );
	GLUI_Master.set_glutReshapeFunc( reshape );
}

////////////////////////////////////////////////////////////////////
// SUBWINDOW_CALLBACK FUNCTION
// ---------------------------
//
// Callback function for the toolbar
//
////////////////////////////////////////////////////////////////////

void subwindow_callback (int mode)
{
	if (mode == KILL_SUBWINDOW)
	{
		glui_subwin->close();
	}
}

////////////////////////////////////////////////////////////////////
// STARTUP WINDOW CALLBACK FUNCTION
// --------------------------------
//
// Called when the play button is pressed. Basically, closes the start
// window, initialises the main game loop and initialises the whole 
// program.
//
////////////////////////////////////////////////////////////////////

void startup_window_callback (int mode)
{
	if (mode == START_GAME)
	{
		//PlaySound("Data/Sounds/hellguy.wav",NULL,SND_FILENAME | SND_ASYNC );
	
		glui_startup_window->close();

		glutSetWindow(window_main);
		glutShowWindow();

		glutSetWindow(radar_window);
		glutShowWindow();

		glui_subwin->show();

		glutSetWindow(window_main);

		glutFullScreen ();
		fullscreen_x = glutGet(GLUT_SCREEN_WIDTH);
		fullscreen_y = glutGet(GLUT_SCREEN_HEIGHT);

		initialise();

		game_state = GAME_MAIN_LOOP;

		if (music == 1)
		{
			play_music(FALSE);
		}
	}
}

////////////////////////////////////////////////////////////////////
// CALCULATE_ORIENTATION FUNCTION
// ------------------------------
//
// Calculates the way that the model should face. Most of the models
// are orientated towards their target, however this function seems
// to slowly rotate towards the starting orientation.
//
////////////////////////////////////////////////////////////////////

float calculate_orientation (float x1, float y1, float z1, float x2, float y2, float z2)
{
	float dot, normal_x, normal_y, normal_z;
	float angle = 0;
	float vectors_magnitude;

	dot = (x1 * x2) + (y1 * y2) + (z1 * z2);

	normal_x = (y1 * z2) - (z1 * y2);
	normal_y = (z1 * x2) - (x1 * z2);
	normal_z = (x1 * y2) - (y1 * x2);

	vectors_magnitude = magnitude(x1, y1, z1) * magnitude(x2, y2, z2) ;

	angle = acos( dot / vectors_magnitude );

	if(_isnan(angle))
		return 0;

	angle = angle * (180 / PI);	
//	angle = angle * (PI / 180);	
//	angle = PI + (PI - angle);
/*
	if (angle > 180)
	{
		angle = 360 - angle;
	}
	else
	{
		angle = angle - 180;
	}
*/
	return angle;
}


////////////////////////////////////////////////////////////////////
// CALCULATE_ORIENTATION FUNCTION
// ------------------------------
//
// This returns the magnitude of a vector.
//
////////////////////////////////////////////////////////////////////

float magnitude(float normal_x, float normal_y, float normal_z)
{
	// Here is the equation:  magnitude = sqrt(V.x^2 + V.y^2 + V.z^2)  Where V is the vector
	return (float)sqrt( (normal_x * normal_x) + (normal_y * normal_y) + (normal_z * normal_z) );
}

////////////////////////////////////////////////////////////////////
// CALCULATE FRAMET RATE FUNCTION
// ------------------------------
//
// Returns the FPS count.
//
////////////////////////////////////////////////////////////////////

void calculate_frame_rate (void)
{
	static float framesPerSecond    = 0.0f;		// This will store our fps
    static float lastTime			= 0.0f;		// This will hold the time from the last frame
	static float frameTime = 0.0f;				// This stores the last frame's time

	// Get the current time in seconds
    float currentTime = timeGetTime() * 0.001f;				

	// Here we store the elapsed time between the current and last frame,
	// then keep the current frame in our static variable for the next frame.
 	frame_interval = currentTime - frameTime;
	frameTime = currentTime;

	// Increase the frame counter
    ++framesPerSecond;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
    if( currentTime - lastTime > 1.0f )
    {
		// Here we set the lastTime to the currentTime
	    lastTime = currentTime;
	
		frames_per_second = framesPerSecond;

		// Reset the frames per second
        framesPerSecond = 0;
    }
}

////////////////////////////////////////////////////////////////////
// PLAY MUSIC FUNCTION
// -------------------
//
// Plays MP3 files.
//
////////////////////////////////////////////////////////////////////

void play_music (int cleanup)
{
	/*
	IGraphBuilder *pGraph;
	IMediaControl *pMediaControl;
	IMediaEvent *pEvent;

	HRESULT hr;

	// make filtergraph manager and query for interfaces.

	CoInitialize(NULL);

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
	IID_IGraphBuilder, (void **)&pGraph);

	if (FAILED(hr)) {

	//DEBUGMESSAGE("DirectShow: CoCreateInstanceFailed");

	exit( 0 ); 
	}


	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pMediaControl);

	if (FAILED(hr)) {

	// Here we should release any created devices
	// DEBUGMESSAGE("DirectShow: Failed to Query Interface for MediaControl");

	exit( 0 );

	}


	pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);


	if (FAILED(hr)) {

	// Here we should release any created devices
	//DEBUGMESSAGE("DirectShow: Failed to Query Interface for MediaEvent");

	exit( 0 );

	}

	if (track_name == 0)
	{
		// The "L" prefix converts an ASCII string to a wide character string.
		hr = pGraph->RenderFile( L"Data/Music/desert.mp3", NULL);
	}

	if (track_name == 1)
	{
		// The "L" prefix converts an ASCII string to a wide character string.
		hr = pGraph->RenderFile( L"Data/Music/ice.mp3", NULL);
	}

	if (track_name == 2)
	{
		// The "L" prefix converts an ASCII string to a wide character string.
		hr = pGraph->RenderFile( L"Data/Music/ride_of_the_valkyries.mp3", NULL);
	}

	if (track_name == 3)
	{
		// The "L" prefix converts an ASCII string to a wide character string.
		hr = pGraph->RenderFile( L"Data/Music/hell_march.mp3", NULL);
	}

	if (track_name == 4)
	{
		// The "L" prefix converts an ASCII string to a wide character string.
		hr = pGraph->RenderFile( L"Data/Music/radio_2.mp3", NULL);
	}

	if (FAILED(hr)) {

	// Here we should release any created devices
	//DEBUGMESSAGE("DirectShow: Failed to Render File");

	exit( 0 );

	}

	// Run the MP3

	pMediaControl->Run();

	// You can also use
	// pMediaControl->Stop();

	// Wait for completion.
//	long evCode;
//	pEvent->WaitForCompletion(INFINITE, &evCode);

	if(cleanup == TRUE)
	{
		// Clean up.
		pMediaControl->Release();
		pEvent->Release();
		pGraph->Release();
		//CoUninitialize();
	}
	*/
}