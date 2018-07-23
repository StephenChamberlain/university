class open_gl
{
private:
	double x_pos;
	double y_pos;
	double old_x;
	double old_y;
	int selected_point_one;
	int selected_point_two;
	int heading;
	double update;

public:

	void gl_display_setup(void);
	void gl_display_setup_2d(int argc, char** argv);
	void gl_draw_sphere(int x, int y, int z, int size);
	void gl_initialise_perspective(void);
	void gl_perspective_viewing_system(void);
	void gl_keyboard(unsigned char key_press);
	void gl_increment_xpos(float amount);
	void gl_decrement_xpos(float amount);
	void gl_increment_ypos(float amount);
	void gl_decrement_ypos(float amount);
	void gl_light(float x, float y, float z);
	void gl_mouse(int x, int y);
	void gl_motion(int x, int y);
	void gl_text_renderer(float x, float y, void *font, char *string);
	void gl_idle(void);

	open_gl();
	~open_gl();
};