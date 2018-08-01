// BuildingData.cpp: implementation of the CBuildingData class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/BuildingData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuildingData::CBuildingData()
{
	id = ID_FREE;
}

CBuildingData::~CBuildingData()
{

}

void CBuildingData::damage_building(int damage)
{
	hit_points -= damage;
}

float CBuildingData::get_x(void)
{
	return x;
}

float CBuildingData::get_y(void)
{
	return y;
}

float CBuildingData::get_z(void)
{
	return z;
}

int CBuildingData::get_id(void)
{
	return id;
}

int CBuildingData::get_hit_points(void)
{
	return hit_points;
}

void CBuildingData::set_hit_points(int new_hit_points)
{
	hit_points = new_hit_points;
}

void CBuildingData::set_x(float new_x)
{
	x = new_x;
}

void CBuildingData::set_y(float new_y)
{
	y = new_y;
}

void CBuildingData::set_z(float new_z)
{
	z = new_z;
}

void CBuildingData::set_id(int new_id)
{
	id = new_id;
}