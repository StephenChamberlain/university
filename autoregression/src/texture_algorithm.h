#include "memory.h"

class texture_algorithm  
{
public:
	memory texture[512][512];
	unsigned long int next;

	texture_algorithm();
	~texture_algorithm();

	void tex_perform_texture_algorithm(int);

	void tex_draw_pixels(void);
	float tex_random_number_generator(void);
};


