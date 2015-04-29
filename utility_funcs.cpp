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

	if (s == "FAUVISM")
		return FAUVISM;

	if (s == "FOLK ART")
		return FOLK_ART;

	return OTHER;
}

string stringFromGenre(genre g)
{
	switch (g)
	{
	case ABSTRACT: return "Abstract";
	case EXPRESSIONISM: return "Expressionism";
	case CONTEMPORARY: return "Contemporary";
	case POST_IMPRESSIONISM: return "Post-Impressionism";
	case OTHER: return "Other";
	case IMPRESSIONISM: return "Impressionism";
	case SURREALISM: return "Surrealism";
	case DADAISM: return "Dadaism";
	case PORTRAIT: return "Portrait";
	case SCENE: return "Scene";
	case MODERNISM: return "Modernism";
	case POST_MODERNISM: return "Post-Modernism";
	case POINTISM: return "Pointism";
	case SKETCH: return "Sketch";
	case POP_ART: return "Pop Art";
	case HISTORIC: return "Historic";
	case LANDSCAPE: return "Landscape";
	case FAUVISM: return "Fauvism";
	case FOLK_ART: return "Folk Art";
	default: cout << "genre not found: " << g << endl;
		return "Unknown Genre";
	}
	
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
	switch (r)
	{
	case COMMON: return "Common";
	case UNCOMMON: return "Uncommon";
	case RARE: return "Rare";
	case ULTRA: return "Ultra";
	case LEGENDARY: return "Legendary";
	case MASTERPIECE: return "Masterpiece";
	default: return "Unknown Rarity";
	}
}

#endif