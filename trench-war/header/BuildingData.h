// BuildingData.h: interface for the CBuildingData class.
//
//////////////////////////////////////////////////////////////////////

// Class about the buildings data

class CBuildingData  
{
private:
	float x;
	float y;
	float z;

	int id;
	int hit_points;

public:
	enum building_id {ID_BARRACKS, 
					  ID_FREE, 
				  	  ID_PLACE_BUILDING, 
					  ID_QUIT, 
					  ID_WAR_FACTORY, 
					  ID_TOWER, 
					  ID_TANK, 
					  ID_TROOPER, 
					  ID_ARTILLERY};

	int terrain_number;

	void set_x(float);
	void set_y(float);
	void set_z(float);

	void set_id(int);
	void set_hit_points(int);

	void damage_building(int);

	float get_x(void);
	float get_y(void);
	float get_z(void);

	int get_id(void);
	int get_hit_points(void);

	CBuildingData();
	~CBuildingData();
};
