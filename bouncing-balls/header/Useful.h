// Useful.h: interface for the CUseful class.
//
//////////////////////////////////////////////////////////////////////

class CUseful  
{
private:
	float frameInterval;	// Used to calculate the frame rate
	float overallFramesPerSecond;	// Frames per second count

public:
	CUseful();
	~CUseful();

	void CalculateFrameRate();

	float GetFramesPerSecond();
	float DistanceBetweenTwoVectors(float, float, float, 
									float, float, float);
};
