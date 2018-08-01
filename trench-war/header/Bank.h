// Bank.h: interface for the CBank class.
//
//////////////////////////////////////////////////////////////////////

// Stores all bank data

class CBank  
{
private:
	float x;
	float y;
	float z;

	int controlled_by;

public:
	enum entities {PLAYER_ONE_HUMAN, PLAYER_TWO_CPU, NEUTRAL};

	CBank();
	~CBank();

	void set_x(float);
	void set_y(float);
	void set_z(float);

	void set_controller (int);
	int get_controller (void);

	float get_x(void);
	float get_y(void);
	float get_z(void);
};

