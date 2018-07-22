class open_gl  
{
private:

public:
	float	x_pos;
	float	y_pos;
	float	heading;
	float	old_x;
	float	old_y;
	double	update;
	int		selected_point_one;
	int		selected_point_two;
	int		gl_grid_copy[20][20][3];
	int		start_point_copy[2];
	int		finish_point_copy[2];

	open_gl();
	~open_gl();
	
	void gl_display_setup(void);
	void gl_display_setup_2d(int argc, char** argv);
	void gl_initialise_perspective(void);
	void gl_perspective_viewing_system(void);

	void gl_keyboard(unsigned char);
	void gl_mouse(int, int);
	void gl_motion(int, int);
	void gl_idle(void);

	void gl_increment_xpos(float);
	void gl_decrement_xpos(float);
	void gl_increment_ypos(float);
	void gl_decrement_ypos(float);

	void gl_draw_sphere(int, int, int, int);
	void gl_light(float, float, float);
	void gl_text_renderer(float x, float y, void *font,char *string);
};

