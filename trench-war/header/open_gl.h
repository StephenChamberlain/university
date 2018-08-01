#include <gl/glut.h>

// My own personal opengl class which wraps some GL functions up.
// Including texturing, perspective stuff, reshape functions
// and drawing functions.

class open_gl  
{
private:

public:
	enum	draw_object {SPHERE, CUBE};	

	int		selected_point_one;
	int		selected_point_two;
	int		gl_grid_copy[20][20][3];
	int		start_point_copy[2];
	int		finish_point_copy[2];
	int		yrot;
	int		wireframe;
	int		button;
	int		state;

	float	water_animation;
	float	top_camera[2], left_camera[2], front_camera[2];		// Camera values for the different viewing systems.
	float	top_zoom, left_zoom, front_zoom, perspective_zoom;
	float	fp_x, fp_y, fp_z;
	float	x_pos;
	float	y_pos;
	float	heading;
	float	old_x;
	float	old_y;

	double	update;

	GLuint  tex_grass_grid;
	GLuint	tex_grass;
	GLuint	tex_sand;
	GLuint	tex_ice;
	GLuint  tex_water;
	GLuint	tex_tree;
	GLuint	tex_tree_mask;
	GLuint	tex_player_one;
	GLuint  tex_player_two;
	GLuint  tex_bank;
	GLuint  tex_river_bank;
    GLuint	tex_hq;
	GLuint	tex_neutral;
	GLuint	tex_hud;
	GLuint	tex_smoke;
	GLuint	tex_smoke_one;
	GLuint	tex_build_barracks;
	GLuint	tex_build_gtower;
	GLuint	tex_build_war;
	GLuint	tex_sky_box;
	GLuint	tex_tank_build;
	GLuint	tex_trooper_build;
	GLuint	tex_artillery_build;
	GLuint	tex_player_one_hq;
	GLuint	tex_player_two_hq;
	GLuint	tex_player_one_barracks;
	GLuint	tex_player_two_barracks;
	GLuint	tex_player_one_trooper;
	GLuint	tex_player_two_trooper;
	GLuint	tex_player_one_guard_tower;
	GLuint	tex_player_two_guard_tower;
	GLuint	tex_player_one_tank;
	GLuint	tex_player_two_tank;

	open_gl();
	~open_gl();

	void gl_display_setup(void);
	void gl_initialise_perspective(void);
	void gl_perspective_viewing_system(void);
	void gl_initialise_2d(void);
	void gl_2d_viewing_system(float, int, int, float, float);
	void gl_keyboard(unsigned char);
	void gl_mouse(int, int, int, int);
	void gl_motion(int, int);
	void gl_idle(void);
	void gl_reshape(int, int);

	void gl_increment_xpos(float);
	void gl_decrement_xpos(float);
	void gl_increment_ypos(float);
	void gl_decrement_ypos(float);

	bool gl_load_bitmap(char*);
	void gl_setup_texture_parameters(void);
	void gl_setup_textures(int);

	void gl_setup_fog(float, float);

	void gl_light(float, float, float, float, float, float);
	void gl_text_renderer(float x, float y, void *font,char *string, float, float, float);

	void gl_draw_sphere(float, float, float, float, float, float, float);
	void gl_draw_textured_sphere(float, int);
	void gl_draw_textured_skybox(void);
	void gl_draw_textured_cube(float);
	void gl_draw_textured_2d_square(float, float, double, double, double);
	void gl_draw_textured_2d_plane(void);
	void gl_draw_textured_polygon(float, float, float,
			 				      float, float, float,
								  float, float, float,
								  float, float, float, int);
	void gl_draw_point(float, float, float);
	void gl_draw_line(float, float, float, float, float, float);
	void gl_draw_polygon(float, float, float, float, float, float, float, float, float);
	void gl_draw_plane(float, float);
	void gl_draw_object(float, float, float, int, int, float, float, float);
	void gl_draw_cursor(float, float, float);
};

