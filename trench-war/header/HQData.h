// HQData.h: interface for the CHQData class.
//
//////////////////////////////////////////////////////////////////////

// Headquarters data

class CHQData  
{
private:
	float x;
	float y;
	float z;

	float hit_points;

public:
	CHQData();
	~CHQData();

	void set_x(float);
	void set_y(float);
	void set_z(float);

	void set_hit_points(float);
	void damage_hq(float);

	float get_x(void);
	float get_y(void);
	float get_z(void);

	float get_hit_points(void);
};

