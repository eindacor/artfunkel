#ifndef UTILITY_FUNCS_HPP
#define UTILITY_FUNCS_HPP

#include "utility_funcs.h"

string getDateString(const jep::date &d, bool include_day)
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

void printDate(jep::date d, bool include_day)
{
	cout << getDateString(d, include_day) << endl;
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

	if (s == "SKETCH")
		return SKETCH;

	if (s == "POP ART")
		return POP_ART;

	if (s == "HISTORIC")
		return HISTORIC;

	if (s == "LANDSCAPE")
		return LANDSCAPE;

	return OTHER;
}

string stringFromGenre(genre g)
{
	if (g == ABSTRACT)
		return "ABSTRACT";

	if (g == POST_IMPRESSIONISM)
		return "POST_IMPRESSIONISM";

	if (g == OTHER)
		return "OTHER";

	if (g == IMPRESSIONISM)
		return "IMPRESSIONISM";

	if (g == SURREALISM)
		return "SURREALISM";

	if (g == DADAISM)
		return "DADAISM";

	if (g == PORTRAIT)
		return "PORTRAIT";

	if (g == SCENE)
		return "SCENE";

	if (g == MODERNISM)
		return "MODERNISM";

	if (g == POINTISM)
		return "POINTISM";

	if (g == SKETCH)
		return "SKETCH";

	if (g == POP_ART)
		return "POP ART";

	if (g == HISTORIC)
		return "HISTORIC";

	if (g == LANDSCAPE)
		return "LANDSCAPE";

	return "Other";
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

string stringFromRarity(rarity r)
{
	if (r == COMMON)
		return "Common";

	if (r == UNCOMMON)
		return "Uncommon";

	if (r == RARE)
		return "Rare";

	if (r == ULTRA)
		return "Ultra";

	if (r == LEGENDARY)
		return "Legendary";

	if (r == MASTERPIECE)
		return "Masterpiece";

	return "Unknown";
}

#endif