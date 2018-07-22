// Quaternion.cpp: implementation of the CQuaternion class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <math.h>

#include "../header/Quaternion.h"

#define PI 3.14159

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuaternion::CQuaternion()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;   
	w = 1.0f; 
}

//////////////////////////////////////////////////////////////////////

CQuaternion::~CQuaternion()
{

}

//////////////////////////////////////////////////////////////////////

void CQuaternion::set_x(float new_x)
{
	x = new_x;
}

//////////////////////////////////////////////////////////////////////

void CQuaternion::set_y(float new_y)
{
	y = new_y;
}

//////////////////////////////////////////////////////////////////////

void CQuaternion::set_z(float new_z)
{
	z = new_z;
}

//////////////////////////////////////////////////////////////////////

void CQuaternion::set_w(float new_w)
{
	w = new_w;
}

//////////////////////////////////////////////////////////////////////

float CQuaternion::get_x(void)
{
	return x;
}

//////////////////////////////////////////////////////////////////////

float CQuaternion::get_y(void)
{
	return y;
}

//////////////////////////////////////////////////////////////////////

float CQuaternion::get_z(void)
{
	return z;
}

//////////////////////////////////////////////////////////////////////

float CQuaternion::get_w(void)
{
	return w;
}

//////////////////////////////////////////////////////////////////////

void CQuaternion::create_from_axis_angle(float in_x, float in_y, float in_z, float degree)
{ 
	// This function takes an angle and an axis of rotation, then converts
	// it to a quaternion.  An example of an axis and angle is what we pass into
	// glRotatef().  That is an axis angle rotation.  It is assumed an angle in 
	// degrees is being passed in.  Instead of using glRotatef(), we can now handle
	// the rotations our self.

	// The equations for axis angle to quaternions are such:

	// w = cos( theta / 2 )
	// x = X * sin( theta / 2 )
	// y = Y * sin( theta / 2 )
	// z = Z * sin( theta / 2 )

	// First we want to convert the degrees to radians 
	// since the angle is assumed to be in radians
	float angle = float((degree / 180.0f) * PI);

	// Here we calculate the sin( theta / 2) once for optimization
	float result = (float)sin( angle / 2.0f );
		
	// Calcualte the w value by cos( theta / 2 )
	w = (float)cos( angle / 2.0f );

	// Calculate the x, y and z of the quaternion
	x = float(in_x * result);
	y = float(in_y * result);
	z = float(in_z * result);
}

//////////////////////////////////////////////////////////////////////

void CQuaternion::create_matrix(float *pMatrix)
{
	// Make sure the matrix has allocated memory to store the rotation data
	if(!pMatrix) return;

	// This function is a necessity when it comes to doing almost anything
	// with quaternions.  Since we are working with OpenGL, which uses a 4x4
	// homogeneous matrix, we need to have a way to take our quaternion and
	// convert it to a rotation matrix to modify the current model view matrix.
	// We pass in a 4x4 matrix, which is a 1D array of 16 floats.  This is how OpenGL
	// allows us to pass in a matrix to glMultMatrixf(), so we use a single dimensioned array.
	//
	//     ¦        2     2												 ¦
    //     ¦ 1 - (2y  + 2z )   2xy + 2zw         2xz - 2yw			0	 ¦
    //     ¦															 ¦
    //     ¦                          2     2							 ¦
    // M = ¦ 2xy - 2zw         1 - (2x  + 2z )   2zy + 2xw			0	 ¦
    //     ¦															 ¦
    //     ¦                                            2     2			 ¦
    //     ¦ 2xz + 2yw         2yz - 2xw         1 - (2x  + 2y )	0	 ¦
    //     ¦															 ¦
	//     ¦															 ¦
	//     ¦ 0				   0				 0					1	 |													 ¦
	//     ¦															 ¦
	// 
	// This is of course a 4x4 matrix.  Since OpenGL uses a 4x4 matrix, we need to conform to this.
	// Remember that the identity matrix of a 4x4 matrix has a diagonal of 1's, where
	// the rest of the indices are 0.  That is where we get the 0's lining the sides, and
	// the 1 at the bottom-right corner.  Since OpenGL matrices are row by column, we fill
	// in our matrix accordingly below.
	
	// First row
	pMatrix[ 0] = 1.0f - 2.0f * ( y * y + z * z ); 
	pMatrix[ 1] = 2.0f * (x * y + z * w);
	pMatrix[ 2] = 2.0f * (x * z - y * w);
	pMatrix[ 3] = 0.0f;  

	// Second row
	pMatrix[ 4] = 2.0f * ( x * y - z * w );  
	pMatrix[ 5] = 1.0f - 2.0f * ( x * x + z * z ); 
	pMatrix[ 6] = 2.0f * (z * y + x * w );  
	pMatrix[ 7] = 0.0f;  

	// Third row
	pMatrix[ 8] = 2.0f * ( x * z + y * w );
	pMatrix[ 9] = 2.0f * ( y * z - x * w );
	pMatrix[10] = 1.0f - 2.0f * ( x * x + y * y );  
	pMatrix[11] = 0.0f;  

	// Fourth row
	pMatrix[12] = 0.0f;  
	pMatrix[13] = 0.0f;  
	pMatrix[14] = 0.0f;  
	pMatrix[15] = 1.0f;

	// Now pMatrix[] is a 4x4 homogeneous matrix that can be applied to an OpenGL Matrix
}