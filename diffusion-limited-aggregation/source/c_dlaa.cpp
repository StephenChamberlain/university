// c_dlaa.cpp: implementation of the c_dlaa class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/c_dlaa.h"
#include <gl\glut.h>
#include <time.h>
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

c_dlaa::c_dlaa()
{

}

//////////////////////////////////////////////////////////////////////

c_dlaa::~c_dlaa()
{

}

//////////////////////////////////////////////////////////////////////

void c_dlaa::initialise_class(void)
{
	int i, j;

	for(i=0; i<401; i++)
	{
		for(j=0; j<401; j++)
		{
		//	memory[i][j][0] = rand()%100;
			memory[i][j][0] = 0;
		}
	}

	for(i=0; i<5; i++)
	{
			for(j=0; j<5; j++)
			{
				memory[i + 199][j + 199][0] = 1;
			}
	}

	switch_tag = 0;

	next = 1;

	colour[0] = 1;
	colour[1] = 0;
	colour[2] = 0;
}

//////////////////////////////////////////////////////////////////////

void c_dlaa::draw_pixels(void)
{
	int i, j;
	GLubyte ubImage[65536];

	for(i=0; i<401; i++)
	{
		for(j=0; j<401; j++)
		{
			if (memory[i][j][0] == 1)
			{
				glRasterPos2f((i*1.0)-199.5, (j*1.0)-199.5);
				glColor3f(0.0, 1.0, 0.0);

				if (colour[0] == 1)
				{
					glDrawPixels(1, 1, GL_RED, GL_UNSIGNED_BYTE, ubImage);
				}
				if (colour[1] == 1)
				{
					glDrawPixels(1, 1, GL_GREEN, GL_UNSIGNED_BYTE, ubImage);
				}
				if (colour[2] == 1)
				{
					glDrawPixels(1, 1, GL_BLUE, GL_UNSIGNED_BYTE, ubImage);
				}
			}
		}
	}

//	glRasterPos2f(current_pixel[0]-199.5, current_pixel[1]-199.5);
//	glDrawPixels(5, 5, GL_RGBA, GL_UNSIGNED_BYTE, ubImage);

//	cout << "Down  " << " " << random_tracker[0] ;
//	cout << " Right " << " " << random_tracker[1] ;
//	cout << " Up    " << " " << random_tracker[2] ;
//	cout << " Left  " << " " << random_tracker[3] << endl;
}

//////////////////////////////////////////////////////////////////////

void c_dlaa::peform_diffusion_limited_aggregation(int seed)
{
	int i, j, random_direction, x, y;

	if (switch_tag == 0)
	{
		i = rand()%400;
		j = rand()%400; //choose a location at random

		if (i == 0)
		{
			i++;
		}
		if (j == 0)
		{
			j++;
		}

		if (i == 400)
		{
			i--;
		}
		if (j == 400)
		{
			j--;
		}

		current_pixel[0] = i;
		current_pixel[1] = j;

	//	cout << "Pixel chosen: " << i << " " << j << endl;
		
		switch_tag = 1;
	}

	if (switch_tag == 1)
	{
		x = current_pixel[0];
		y = current_pixel[1];

		if (memory[x - 1][y][0] == 1 || memory[x][y + 1][0] == 1 || memory[x + 1][y][0] == 1 || memory[x][y - 1][0] == 1)
		{
			memory[x][y][0] = 1; //if the pixel is next to a true pixel, set it to true
			switch_tag = 0;
		}
		else
		{
		//	srand(seed);
		//	random_direction = rand()%99999; //otherwise, move the current pixel
		//	srand(random_direction);
		//	random_direction = rand()%40;

			random_direction = random();
			srand(random_direction);
			random_direction = rand()%40;

			//////////////////////////

			if (random_direction > 0 && random_direction < 10 && random_direction != last_direction)
			{
				current_pixel[0] += 1;
				random_tracker[0] ++;
				last_direction = random_direction;
			}

			//////////////////////////

			if (random_direction > 10 && random_direction < 20 &&  random_direction != last_direction)
			{
				current_pixel[1] += 1;
				random_tracker[1] ++;
				last_direction = random_direction;
			}

			//////////////////////////

			if (random_direction > 20 && random_direction < 30 &&  random_direction != last_direction)
			{
				current_pixel[0] -= 1;
				random_tracker[2] ++;
				last_direction = random_direction;
			}

			//////////////////////////

			if (random_direction > 30 && random_direction < 40 &&  random_direction != last_direction)
			{
				current_pixel[1] -= 1;
				random_tracker[3] ++;
				last_direction = random_direction;
			}

			if (current_pixel[0] >= 400)
			{
				current_pixel[0] -= 11;
			}
			if (current_pixel[0] <= 0)
			{
				current_pixel[0] += 11;
			}
			if (current_pixel[1] >= 400)
			{
				current_pixel[1] -= 11;
			}
			if (current_pixel[1] <= 0)
			{
				current_pixel[1] += 11;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////

int c_dlaa::random(void)
{
	next = next * 1103515245 + 12345;
//	cout << next << endl;
	return (unsigned int)(next/65536)%32768;
}