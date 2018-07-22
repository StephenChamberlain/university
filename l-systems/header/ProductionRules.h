// ProductionRules.h: interface for the CProductionRules class.
//
//////////////////////////////////////////////////////////////////////

#define RULE_MAX_SIZE	8192

class CProductionRules  
{
private:
	int number_of_rules;
	char rules[RULE_MAX_SIZE];
	char drawing_rules[RULE_MAX_SIZE];
	char temp[RULE_MAX_SIZE];

public:
	int position_in_drawing_array;

	void add_rule(int, char);
	char get_rule(int);
	char get_drawing_rule(int);

	void set_number_of_rules(int);
	int get_number_of_rules(void);

	void create_drawing_rules(int);
	void use_koch_curve_rules(int);
	void use_plant_rules(int);
	void use_other_rules(int);

	CProductionRules();
	~CProductionRules();

};

