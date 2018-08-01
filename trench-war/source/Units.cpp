// Units.cpp: implementation of the CUnits class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/Units.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnits::CUnits()
{
	unit_id = FREE;
	terrain_number = 0;
}

///////////////////////////////////

CUnits::~CUnits()
{

}

///////////////////////////////////

void CUnits::set_x(float new_x)
{
	x = new_x;
}

///////////////////////////////////

void CUnits::set_y(float new_y)
{
	y = new_y;
}

///////////////////////////////////

void CUnits::set_z(float new_z)
{
	z = new_z;
}

///////////////////////////////////

void CUnits::set_target_x(float new_x)
{
	target_x = new_x;
}

///////////////////////////////////

void CUnits::set_target_y(float new_y)
{
	target_y = new_y;
}

///////////////////////////////////

void CUnits::set_target_z(float new_z)
{
	target_z = new_z;
}

///////////////////////////////////

void CUnits::set_unit_id(int new_id)
{
	unit_id = new_id;
}

///////////////////////////////////

void CUnits::set_hit_points(int new_hit_points)
{
	hit_points = new_hit_points;
}

///////////////////////////////////

void CUnits::set_orientation(float new_orientation)
{
	orientation = new_orientation;
}

///////////////////////////////////

void CUnits::set_speed(float new_speed)
{
	speed = new_speed;
}

///////////////////////////////////

void CUnits::set_attack_points(int new_attack_points)
{
	attack_points = new_attack_points;
}

///////////////////////////////////

void CUnits::damage_unit(int damage)
{
	hit_points -= damage;
}

///////////////////////////////////

float CUnits::get_x(void)
{
	return x;
}

///////////////////////////////////

float CUnits::get_y(void)
{
	return y;
}

///////////////////////////////////

float CUnits::get_z(void)
{
	return z;
}

///////////////////////////////////

float CUnits::get_target_x(void)
{
	return target_x;
}

///////////////////////////////////

float CUnits::get_target_y(void)
{
	return target_y;
}

///////////////////////////////////

float CUnits::get_target_z(void)
{
	return target_z;
}

///////////////////////////////////

int CUnits::get_unit_id(void)
{
	return unit_id;
}

///////////////////////////////////

int CUnits::get_hit_points(void)
{
	return hit_points;
}

///////////////////////////////////

int CUnits::get_attack_points(void)
{
	return attack_points;	 
}

///////////////////////////////////

float CUnits::get_orientation(void)
{
	return orientation;
}

///////////////////////////////////

float CUnits::get_speed(void)
{
	return speed;
}