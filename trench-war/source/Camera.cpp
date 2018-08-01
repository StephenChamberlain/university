// Camera.cpp: implementation of the CCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/Camera.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCamera::CCamera()
{

}

CCamera::~CCamera()
{

}

void CCamera::set_x(float new_x)
{
	x = new_x;
}

void CCamera::set_y(float new_y)
{
	y = new_y;
}

void CCamera::set_z(float new_z)
{
	z = new_z;
}

float CCamera::get_x(void)
{
	return x;
}

float CCamera::get_y(void)
{
	return y;
}

float CCamera::get_z(void)
{
	return z;
}