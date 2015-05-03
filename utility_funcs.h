#ifndef UTILITY_FUNCS_H
#define UTILITY_FUNCS_H
#include "header.h"

string getDateString(const date &d, bool include_day);
double lookupValue(rarity work_rarity);
void printDate(date d);
genre genreFromString(string s);
rarity rarityFromString(string s);

#endif