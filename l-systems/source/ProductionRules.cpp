// ProductionRules.cpp: implementation of the CProductionRules class.
//
//////////////////////////////////////////////////////////////////////

#include <iostream>			// Input, output 
#include <fstream>			// File management 
#include <windows.h>		// Windows. Some windows functions are used

#include "../header/ProductionRules.h"

#define RULE_MAX_SIZE	8192

////////////////////////////////////////////////////////////////////
// STANDARD NAMESPACE
////////////////////////////////////////////////////////////////////

using namespace std;		// Standard namespace

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProductionRules::CProductionRules()
{
	int i;

	for (i=0; i<RULE_MAX_SIZE; i++)
	{
		drawing_rules[i] = '0';
	//	cout << drawing_rules[i] << endl;
	}

	position_in_drawing_array = 0;
}

//////////////////////////////////////////////////////////////////////

CProductionRules::~CProductionRules()
{

}

//////////////////////////////////////////////////////////////////////

void CProductionRules::add_rule(int position, char rule_to_add)
{
	rules[position] = rule_to_add;
}

//////////////////////////////////////////////////////////////////////

char CProductionRules::get_rule(int position)
{
	return rules[position];
}

//////////////////////////////////////////////////////////////////////

char CProductionRules::get_drawing_rule(int position)
{
	return drawing_rules[position];
}

//////////////////////////////////////////////////////////////////////

void CProductionRules::set_number_of_rules(int new_number_of_rules)
{
	number_of_rules = new_number_of_rules;
}

//////////////////////////////////////////////////////////////////////

int CProductionRules::get_number_of_rules(void)
{
	return number_of_rules;
}

//////////////////////////////////////////////////////////////////////

void CProductionRules::create_drawing_rules(int number_of_iterations)
{
//	use_koch_curve_rules(number_of_iterations);
	use_other_rules(number_of_iterations);
//	use_plant_rules(number_of_iterations);
}

//////////////////////////////////////////////////////////////////////

void CProductionRules::use_koch_curve_rules (int number_of_iterations)
{
	int i, j, k;

	for (i=0; i<number_of_rules; i++)
	{
		if (rules[i] == 'F')
		{
			drawing_rules[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '+';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '+';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = 'F';
			position_in_drawing_array++;
		}

		if (rules[i] == '+')
		{
			drawing_rules[position_in_drawing_array] = '+';
			position_in_drawing_array++;
		}

		if (rules[i] == '-')
		{
			drawing_rules[position_in_drawing_array] = '-';
			position_in_drawing_array++;
		}
	}

	position_in_drawing_array = 0;

	for (j=0; j<number_of_iterations; j++)
	{
		if (drawing_rules[j] == 'F')
		{
			temp[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '+';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '+';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = 'F';
			position_in_drawing_array++;
		}

		if (drawing_rules[j] == '+')
		{
			temp[position_in_drawing_array] = '+';
			position_in_drawing_array++;
		}

		if (drawing_rules[j] == '-')
		{
			temp[position_in_drawing_array] = '-';
			position_in_drawing_array++;
		}

		for (k=0; k<position_in_drawing_array; k++)
		{
			drawing_rules[k] = temp[k];
		}

	//	position_in_drawing_array = 0;
	}
}


//////////////////////////////////////////////////////////////////////

void CProductionRules::use_plant_rules (int number_of_iterations)
{
//X = F[+X]F[-X]+X

	int i, j, k;

	for (i=0; i<number_of_rules; i++)
	{
		if (rules[i] == 'X')
		{
			drawing_rules[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '[';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '+';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = 'X';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = ']';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '[';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = 'X';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = ']';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '+';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = 'X';
			position_in_drawing_array++;
		}

		if (rules[i] == '+')
		{
			drawing_rules[position_in_drawing_array] = '+';
			position_in_drawing_array++;
		}

		if (rules[i] == '-')
		{
			drawing_rules[position_in_drawing_array] = '-';
			position_in_drawing_array++;
		}

		if (rules[i] == '[')
		{
			drawing_rules[position_in_drawing_array] = '[';
			position_in_drawing_array++;
		}

		if (rules[i] == ']')
		{
			drawing_rules[position_in_drawing_array] = ']';
			position_in_drawing_array++;
		}

		if (rules[i] == 'X')
		{
			drawing_rules[position_in_drawing_array] = 'X';
			position_in_drawing_array++;
		}
	}

	position_in_drawing_array = 0;

	for (j=0; j<number_of_iterations; j++)
	{
		if (drawing_rules[j] == 'X')
		{
			temp[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '[';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '+';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = 'X';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = ']';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '[';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = 'X';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = ']';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '+';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = 'X';
			position_in_drawing_array++;
		}

		if (drawing_rules[j] == '+')
		{
			temp[position_in_drawing_array] = '+';
			position_in_drawing_array++;
		}

		if (drawing_rules[j] == '-')
		{
			temp[position_in_drawing_array] = '-';
			position_in_drawing_array++;
		}

		if (drawing_rules[j] == '[')
		{
			temp[position_in_drawing_array] = '[';
			position_in_drawing_array++;
		}

		if (drawing_rules[j] == ']')
		{
			temp[position_in_drawing_array] = ']';
			position_in_drawing_array++;
		}

		if (drawing_rules[j] == 'X')
		{
			temp[position_in_drawing_array] = 'X';
			position_in_drawing_array++;
		}

		for (k=0; k<position_in_drawing_array; k++)
		{
			drawing_rules[k] = temp[k];
		}

	//	position_in_drawing_array = 0;
	}
}

//////////////////////////////////////////////////////////////////////

void CProductionRules::use_other_rules (int number_of_iterations)
{
	//	F+F&F+F

	int i, j, k;

	for (i=0; i<number_of_rules; i++)
	{
		if (rules[i] == 'F')
		{
			drawing_rules[position_in_drawing_array] = '[';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '+';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '&';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			drawing_rules[position_in_drawing_array] = ']';
			position_in_drawing_array++;
		}

		if (rules[i] == '+')
		{
			drawing_rules[position_in_drawing_array] = '+';
			position_in_drawing_array++;
		}

		if (rules[i] == '-')
		{
			drawing_rules[position_in_drawing_array] = '-';
			position_in_drawing_array++;
		}

		if (rules[i] == '&')
		{
			drawing_rules[position_in_drawing_array] = '&';
			position_in_drawing_array++;
		}

		if (rules[i] == '[')
		{
			drawing_rules[position_in_drawing_array] = '[';
			position_in_drawing_array++;
		}

		if (rules[i] == ']')
		{
			drawing_rules[position_in_drawing_array] = ']';
			position_in_drawing_array++;
		}
	}

	position_in_drawing_array = 0;

	for (j=0; j<number_of_iterations; j++)
	{
		if (drawing_rules[j] == 'F')
		{
			temp[position_in_drawing_array] = '[';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '+';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '&';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = '-';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = 'F';
			position_in_drawing_array++;

			temp[position_in_drawing_array] = ']';
			position_in_drawing_array++;
		}

		if (drawing_rules[j] == '+')
		{
			temp[position_in_drawing_array] = '+';
			position_in_drawing_array++;
		}
	
		if (drawing_rules[j] == '-')
		{
			temp[position_in_drawing_array] = '-';
			position_in_drawing_array++;
		}

		if (drawing_rules[j] == '&')
		{
			temp[position_in_drawing_array] = '&';
			position_in_drawing_array++;
		}

		if (drawing_rules[j] == '[')
		{
			temp[position_in_drawing_array] = '[';
			position_in_drawing_array++;
		}

		if (drawing_rules[j] == ']')
		{
			temp[position_in_drawing_array] = ']';
			position_in_drawing_array++;
		}

		for (k=0; k<position_in_drawing_array; k++)
		{
			drawing_rules[k] = temp[k];
		}

	//	position_in_drawing_array = 0;
	}
}