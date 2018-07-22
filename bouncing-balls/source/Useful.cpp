// Useful.cpp: implementation of the CUseful class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/Useful.h"
#include <time.h>			// Used for timing
#include <math.h>			// Used for maths
#include <windows.h>		// Windows. Some windows functions are used
#include <iostream>			// Input, output 
#include <fstream>			// File management 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUseful::CUseful()
{

}

//////////////////////////////////////////////////////////////////////

CUseful::~CUseful()
{

}

//////////////////////////////////////////////////////////////////////

void CUseful::CalculateFrameRate (void)
{
/*	static float framesPerSecond    = 0.0f;		// This will store our fps
    static float lastTime			= 0.0f;		// This will hold the time from the last frame
	static float frameTime			= 0.0f;		// This stores the last frame's time

	// Get the current time in seconds
    float currentTime = timeGetTime() * 0.001f;				

	// Here we store the elapsed time between the current and last frame,
	// then keep the current frame in our static variable for the next frame.
 	frameInterval = currentTime - frameTime;
	frameTime = currentTime;

	// Increase the frame counter
    ++framesPerSecond;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
    if( currentTime - lastTime > 1.0f )
    {
		// Here we set the lastTime to the currentTime
	    lastTime = currentTime;
	
		overallFramesPerSecond = framesPerSecond;

		// Reset the frames per second
        framesPerSecond = 0;
    }*/
}

////////////////////////////////////////////////////////////////////

float CUseful::GetFramesPerSecond(void)
{
	return overallFramesPerSecond;
}

//////////////////////////////////////////////////////////////////////

float CUseful::DistanceBetweenTwoVectors(float x1, float y1, float z1,
								       float x2, float y2, float z2)
{
	float distance;
	float dx = x1 - x2;
	float dz = z1 - z2;

	distance = sqrt((dx*dx) + (dz*dz));

	return distance;
}