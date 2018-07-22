// OpenGL.h: interface for the COpenGL class.
//
//////////////////////////////////////////////////////////////////////



class COpenGL  
{
private:
	/******* Integers *******/
	int fullscreen_x;
	int fullscreen_y;
	int	font;	// GLUT font to use on the HUD

	/******* Chars *******/
	char text_buffer[512];		// Text buffer for displaying HUD text

public:
	COpenGL();
	~COpenGL();

	void gl_keyboard (unsigned char, int, int);
	void gl_reshape (int, int);
	void gl_text_renderer(float x, float y, void *font, char *string, float r, float g, float b);
	void gl_fp_viewing_system (float, float, float, float, float);

	void set_fullscreen_x (int);
	void set_fullscreen_y (int);
	void set_perspective (void);
	void set_2d_viewing_system (void);
	void set_light_and_properties (float, float, float, float, float, float);

	int get_fullscreen_x (void);
	int get_fullscreen_y (void);

	void draw_point(float, float, float);
	void draw_line(float, float, float, float, float, float);
	void draw_sphere(float, float, float, int);
	void draw_textured_polygon(float, float, float,
		  					   float, float, float,
							   float, float, float,
							   float, float, float);

	void draw_hud (float);

	void text_renderer(float x, float y, void *font,char *string, float, float, float);
};
