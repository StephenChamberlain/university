// HQData.cpp: implementation of the CHQData class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/HQData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHQData::CHQData()
{

}

CHQData::~CHQData()
{

}

float CHQData::get_x(void)
{
	return x;
}

float CHQData::get_y(void)
{
	return y;
}

float CHQData::get_z(void)
{
	return z;
}

float CHQData::get_hit_points(void)
{
	return hit_points;
}

void CHQData::set_x(float new_x)
{
	x = new_x;
}

void CHQData::set_y(float new_y)
{
	y = new_y;
}

void CHQData::set_z(float new_z)
{
	z = new_z;
}

void CHQData::set_hit_points (float new_hit_points)
{
	hit_points = new_hit_points;
}

void CHQData::damage_hq (float damage)
{
	hit_points -= damage;
}
