// Units.h: interface for the CUnits class.
//
//////////////////////////////////////////////////////////////////////

// Unit data. Most of the variables are self explanitary.

class CUnits  
{
private:
	float x;
	float y;
	float z;

	float target_x;
	float target_y;
	float target_z;

	float orientation;
	float speed;

	int unit_id;
	int hit_points;
	int attack_points;

public:
	enum units {TREE, TANK, BANK, HQ, BARRACKS, WAR_FACTORY, TOWER, ARTILLERY, TROOPER, FREE};

	int terrain_number;
		
	CUnits();
	~CUnits();

	void set_x(float);
	void set_y(float);
	void set_z(float);

	void set_target_x(float);
	void set_target_y(float);
	void set_target_z(float);

	void set_unit_id(int);
	void set_hit_points(int);

	void set_orientation(float);
	void set_speed(float);
	void set_attack_points(int);

	void damage_unit(int);

	float get_x(void);
	float get_y(void);
	float get_z(void);

	float get_target_x(void);
	float get_target_y(void);
	float get_target_z(void);

	int get_unit_id(void);
	int get_hit_points(void);
	int get_attack_points(void);

	float get_orientation(void);
	float get_speed(void);
};

