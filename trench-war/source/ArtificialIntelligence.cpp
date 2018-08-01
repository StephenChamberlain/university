// ArtificialIntelligence.cpp: implementation of the CArtificialIntelligence class.
//
//////////////////////////////////////////////////////////////////////

#include "../header/ArtificialIntelligence.h"
#include <math.h>
#include <windows.h>
#include <time.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArtificialIntelligence::CArtificialIntelligence()
{

}

//////////////////////////////////////////////////////////////////////

CArtificialIntelligence::~CArtificialIntelligence()
{

}

//////////////////////////////////////////////////////////////////////

void CArtificialIntelligence::set_ai_state(int new_state)
{
	state = new_state;
}

//////////////////////////////////////////////////////////////////////

int CArtificialIntelligence::get_state(void)
{
	return state;
}

//////////////////////////////////////////////////////////////////////
 
void CArtificialIntelligence::run_ai_model(int cpu_money, int num_barracks, int num_war_factories, int num_towers)
{
	int random;

	if (cpu_money < 1000)
	{
		state = GATHER_RESOURCES;
	}
	else if (num_barracks < 1)
	{
		state = BUILD_BARRACKS;
	}
	else if (num_war_factories < 1)
	{
		state = BUILD_WAR_FACTORY;
	}
	else if (num_towers < 4)
	{
		state = BUILD_TOWER;
	}

	if (num_war_factories == 1)
	{
		random = rand()%2;

		if (random == 0)
		{
			unit_to_build = TANK;
		}
		else if (random == 1)
		{
			unit_to_build = ARTILLERY;
		}
	}
	else if (num_barracks == 1)
	{
		unit_to_build = TROOPER;
	}
}

//////////////////////////////////////////////////////////////////////

void CArtificialIntelligence::set_unit_to_build(int new_unit_to_build)
{
	unit_to_build = new_unit_to_build;
}

//////////////////////////////////////////////////////////////////////

int CArtificialIntelligence::get_unit_to_build(void)
{
	return unit_to_build;
}

//////////////////////////////////////////////////////////////////////

void CArtificialIntelligence::set_stance(int new_stance)
{
	stance = new_stance;
}

//////////////////////////////////////////////////////////////////////

int CArtificialIntelligence::get_stance(void)
{
	return stance;
}