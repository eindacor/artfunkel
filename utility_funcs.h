#ifndef UTILITY_FUNCS_H
#define UTILITY_FUNCS_H

#include "header.h"

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

void printDate(jep::date d)
{
	cout << getDateString(d) << endl;
}

void printArtist(const artist &a)
{
	cout << a.getID() << ": " << a.getName() << " (" << a.getBirthDateString(false) << " - " << a.getDeathDateString(false) << ")" << endl;
}

void printArtwork(const artwork &a)
{
	cout << a.getTitle() << " by " << a.getArtistName() << "($" << a.getValue() << "k)" << endl;
}

genre genreFromString(string s)
{
	if (s == "ABSTRACT")
		return ABSTRACT;

	if (s == "POST-IMPRESSIONISM")
		return POST_IMPRESSIONISM;

	if (s == "OTHER")
		return OTHER;

	if (s == "IMPRESSIONISM")
		return IMPRESSIONISM;

	if (s == "SURREALISM")
		return SURREALISM;

	if (s == "DADAISM")
		return DADAISM;

	if (s == "PORTRAIT")
		return PORTRAIT;

	if (s == "SCENE")
		return SCENE;

	if (s == "MODERNISM")
		return MODERNISM;

	if (s == "POINTISM")
		return POINTISM;

	return OTHER;
}

rarity rarityFromString(string s)
{
	if (s == "COMMON")
		return COMMON;

	if (s == "UNCOMMON")
		return UNCOMMON;

	if (s == "RARE")
		return RARE;

	if (s == "ULTRA")
		return ULTRA;

	if (s == "LEGENDARY")
		return LEGENDARY;

	if (s == "MASTERPIECE")
		return MASTERPIECE;

	return UNKNOWN_RARITY;
}

#endif