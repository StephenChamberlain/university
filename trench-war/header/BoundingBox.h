// BoundingBox.h: interface for the CBoundingBox class.
//
//////////////////////////////////////////////////////////////////////

// Contains eight points of the bounding box. 

class CBoundingBox  
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

	CBoundingBox();
	~CBoundingBox();

};

