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

enum genre { OTHER, DADAISM, SURREALISM, IMPRESSIONISM, POST_IMPRESSIONISM, PORTRAIT, ABSTRACT, EXPRESSIONISM, CONTEMPORARY, SCENE, MODERNISM, POST_MODERNISM, POINTISM };
enum rarity { UNKNOWN_RARITY, COMMON, UNCOMMON, RARE, ULTRA, LEGENDARY, MASTERPIECE};

string getDateString(const jep::date &d, bool include_day);
double lookupValue(rarity work_rarity);

#endif