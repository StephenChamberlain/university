class c_dlaa  
{
private:

public:
	int		memory[800][800][1];
	int		switch_tag;
	int		current_pixel[2];
	int		random_tracker[4];
	int		last_direction;
	int		colour[3];
	unsigned long int next;

	void initialise_class(void);

	void draw_pixels(void);
	int random(void);
	void peform_diffusion_limited_aggregation(int);

	c_dlaa();
	~c_dlaa();

};