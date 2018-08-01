// ArtificialIntelligence.h: interface for the CArtificialIntelligence class.
//
//////////////////////////////////////////////////////////////////////

// AI model class

class CArtificialIntelligence  
{
private:
	int state;
	int unit_to_build;

public:
	enum	intel_state {AGGRESIVE, PASSIVE, GATHER_RESOURCES, BUILD_BARRACKS, BUILD_WAR_FACTORY, BUILD_TOWER};
	enum	units {TREE, TANK, BANK, HQ, BARRACKS, WAR_FACTORY, TOWER, ARTILLERY, TROOPER, FREE};

	int stance;

	CArtificialIntelligence();
	~CArtificialIntelligence();

	void set_ai_state (int);
	void set_unit_to_build (int);
	void set_stance (int);

	void run_ai_model(int, int, int, int);

	int get_state (void);
	int get_stance (void);
	int get_unit_to_build (void);
};
