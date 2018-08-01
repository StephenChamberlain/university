// c_terrain.cpp: implementation of the c_terrain class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/c_terrain.h"
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

c_terrain::c_terrain()
{
	y = 0;
	bank = 0;
	tree_orientation = 0;
}

c_terrain::~c_terrain()
{
	
}

int c_terrain::random(void)
{
	next = rand();
	next = next * 1103515245 + 12345;
	next /= 1000;
	return (unsigned int)(next/65536)%32768;
}