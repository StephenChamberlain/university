#include "../header/texture_algorithm.h"
#include <gl\glut.h>

////////////////////////////////////////////////

texture_algorithm::texture_algorithm()
{
	int i, j;
	float seed, greyscale, red, green, blue;

	for (i=0; i<512; i++)
	{
		for (j=1; j<512; j++)
		{
			texture[i][j].red = 0;
			texture[i][j].green = 0;
			texture[i][j].blue = 0;
		}
	}

	for (i=0; i<512; i++)
	{
		seed = tex_random_number_generator();	
		srand(seed);
		greyscale = rand()%100;
		red = rand()%100;
		green = rand()%100;
		blue = rand()%100;
		greyscale = greyscale / (greyscale + 100);
		red = red / (red + 100);
		green = green / (green + 100);
		blue = blue / (blue + 100);

		texture[i][0].red = red;
		texture[i][0].green = 0;
		texture[i][0].blue = 0;
	}
}

////////////////////////////////////////////////

texture_algorithm::~texture_algorithm()
{

}

////////////////////////////////////////////////

void texture_algorithm::tex_draw_pixels(void)
{
	int i, j;

	glPointSize(1);

	for(i=0; i<512; i++)
	{
		for(j=0; j<512; j++)
		{
			glColor3f(texture[i][j].red, texture[i][j].green, texture[i][j].blue);

			glBegin(GL_POINTS);
				glVertex2f(i-256, j-256);
			glEnd();
		}
	}
}

////////////////////////////////////////////////

float texture_algorithm::tex_random_number_generator(void)
{
	next = next * 1103515245 + 12345;
	return (unsigned int)(next/65536)%32768 / 100;
}

////////////////////////////////////////////////

void texture_algorithm::tex_perform_texture_algorithm(int scanline_number)
{
	int i, seed, random_offset, rand2;

/*	seed = tex_random_number_generator();	
	srand(seed);
	random_offset = rand()%100;
	rand2 = rand()%100;
	random_offset = random_offset / (random_offset / 2);
	random_offset = random_offset / (random_offset + 1);
*/
	for (i=0; i<512; i++)
	{
		if (i-1 == -1)
		{
			texture[i][scanline_number].red = (texture[512][scanline_number-1].red + texture[i][scanline_number-1].red + texture[i+1][scanline_number-1].red) / 3 ;
			texture[i][scanline_number].green = (texture[512][scanline_number-1].green + texture[i][scanline_number-1].green + texture[i+1][scanline_number-1].green) / 3 ;
			texture[i][scanline_number].blue = (texture[512][scanline_number-1].blue + texture[i][scanline_number-1].blue + texture[i+1][scanline_number-1].blue) / 3 ;
		}
		else if (i+1 == 512)
		{
			texture[i][scanline_number].red = (texture[0][scanline_number-1].red + texture[i][scanline_number-1].red + texture[i+1][scanline_number-1].red) / 3 ;
			texture[i][scanline_number].green = (texture[0][scanline_number-1].green + texture[i][scanline_number-1].green + texture[i+1][scanline_number-1].green) / 3 ;
			texture[i][scanline_number].blue = (texture[0][scanline_number-1].blue + texture[i][scanline_number-1].blue + texture[i+1][scanline_number-1].blue) / 3 ;
		}
		else
		{
			texture[i][scanline_number].red = (texture[i-1][scanline_number-1].red + texture[i][scanline_number-1].red + texture[i+1][scanline_number-1].red) / 3 ;
			texture[i][scanline_number].green = (texture[i-1][scanline_number-1].green + texture[i][scanline_number-1].green + texture[i+1][scanline_number-1].green) / 3 ;
			texture[i][scanline_number].blue = (texture[i-1][scanline_number-1].blue + texture[i][scanline_number-1].blue + texture[i+1][scanline_number-1].blue) / 3 ;
		}
	}
}