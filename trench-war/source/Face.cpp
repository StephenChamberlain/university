// Face.cpp: implementation of the CFace class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/Face.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFace::CFace()
{

}

CFace::~CFace()
{

}

void CFace::set_vertex_one(int new_vertex_one)
{
	vertex_one = new_vertex_one;
}

void CFace::set_vertex_two(int new_vertex_two)
{
	vertex_two = new_vertex_two;
}

void CFace::set_vertex_three(int new_vertex_three)
{
	vertex_three = new_vertex_three;
}

int CFace::get_vertex_one(void)
{
	return vertex_one;
}

int CFace::get_vertex_two(void)
{
	return vertex_two;
}

int CFace::get_vertex_three(void)
{
	return vertex_three;
}