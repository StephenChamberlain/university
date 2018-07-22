// Quaternion.h: interface for the CQuaternion class.
//
//////////////////////////////////////////////////////////////////////

class CQuaternion  
{
private:
	float x;
	float y;
	float z;
	float w;

public:
	CQuaternion();
	~CQuaternion();

	void set_x(float);
	void set_y(float);
	void set_z(float);
	void set_w(float);

	float get_x (void);
	float get_y (void);
	float get_z (void);
	float get_w (void);

	void create_from_axis_angle(float, float, float, float);
	void create_matrix(float *pMatrix);
};

