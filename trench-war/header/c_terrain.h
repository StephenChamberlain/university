// c_terrain.h: interface for the c_terrain class.
//
//////////////////////////////////////////////////////////////////////

// Class that holds all the terrain data

class c_terrain  
{
public:
	float x;
	float y;
	float z;
	float tree_orientation;
	int	  id_number;
	int	  tree;
	int	  bank;
	unsigned long int	next; //used for random generator

	c_terrain();
	~c_terrain();

	int random(void);
};

