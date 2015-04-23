#ifndef HEADER_H
#define HEADER_H

#include <string>
#include <vector>
#include <iostream>
#include "jep_util.h"
#include "jeploot.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;

enum genre {DADA, SURREAL, IMPRESSIONIST, PORTRAIT, ABSTRACT, EXPRESSIONIST};
enum rarity {COMMON, UNCOMMON, RARE, ULTRA, LEGENDARY, MASTERPIECE};

string getDateString(const jep::date &d, bool include_day = true)
{
	string date_string;

	bool day_known = d.getDayOfWeek() != UNKNOWN_DAY;

	if (day_known && include_day)
		date_string += d.getDayString() + ", ";

	if (d.getMonth() != UNKNOWN_MONTH)
		date_string += d.getMonthString() + " ";

	if (day_known)
		date_string += std::to_string(d.getDayOfMonth()) + ", ";

	date_string += std::to_string(d.getYear());

	return date_string;
}

double lookupValue(rarity work_rarity)
{
	//placeholder function
	double min, max;
	switch (work_rarity)
	{
	case COMMON: 
		min = 5.0f;
		max = 15.0f;
		break;
	case UNCOMMON:
		min = 15.0f;
		max = 45.0f;
		break;
	case RARE:
		min = 45.0f;
		max = 165.0f;
		break;
	case ULTRA:
		min = 165.0f;
		max = 645.0f;
		break;
	case LEGENDARY:
		min = 645.0f;
		max = 2565.0f;
		break;
	case MASTERPIECE:
		min = 2565.0f;
		max = 10245.0f;
		break;
	}

	return jep::floatRoll(min, max, 2);
}

#endif