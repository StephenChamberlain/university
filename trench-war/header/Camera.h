// Camera.h: interface for the CCamera class.
//
//////////////////////////////////////////////////////////////////////

// Stores data on the camera, position etc

class CCamera  
{
private:
	float x;
	float y;
	float z;

public:
	CCamera();
	~CCamera();
	
	void set_x (float);
	void set_y (float);
	void set_z (float);

	float get_x (void);
	float get_y (void);
	float get_z (void);
};

