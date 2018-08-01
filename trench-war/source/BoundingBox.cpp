// BoundingBox.cpp: implementation of the CBoundingBox class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/BoundingBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBoundingBox::CBoundingBox()
{

}

///////////////////////////////////

CBoundingBox::~CBoundingBox()
{

}

///////////////////////////////////

float CBoundingBox::get_x(void)
{
	return x;
}

///////////////////////////////////

float CBoundingBox::get_y(void)
{
	return y;
}

///////////////////////////////////

float CBoundingBox::get_z(void)
{
	return z;
}

///////////////////////////////////

void CBoundingBox::set_x(float new_x)
{
	x = new_x;
}

///////////////////////////////////

void CBoundingBox::set_y(float new_y)
{
	y = new_y;
}

///////////////////////////////////

void CBoundingBox::set_z(float new_z)
{
	z = new_z;
}