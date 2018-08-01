// Bank.cpp: implementation of the CBank class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/Bank.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBank::CBank()
{

}

CBank::~CBank()
{

}

float CBank::get_x(void)
{
	return x;
}

float CBank::get_y(void)
{
	return y;
}

float CBank::get_z(void)
{
	return z;
}

void CBank::set_x(float new_x)
{
	x = new_x;
}

void CBank::set_y(float new_y)
{
	y = new_y;
}

void CBank::set_z(float new_z)
{
	z = new_z;
}

void CBank::set_controller(int new_controller)
{
	controlled_by = new_controller;
}

int CBank::get_controller (void)
{
	return controlled_by;
}