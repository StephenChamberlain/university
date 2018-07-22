// Vertex.h: interface for the CVertex class.
//
//////////////////////////////////////////////////////////////////////

// Vertex data, x,y,z. Used by the model structs to store model data.

class CVertex  
{
private:
	float x;
	float y;
	float z;

public:
	void set_x(float);
	void set_y(float);
	void set_z(float);

	float get_x(void);
	float get_y(void);
	float get_z(void);

	CVertex();
	~CVertex();

};

