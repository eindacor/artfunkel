#ifndef UTILITY_FUNCS_H
#define UTILITY_FUNCS_H
#include "header.h"

string getDateString(const date &d, bool include_day);
double lookupValue(rarity work_rarity);
void printDate(date d);
genre genreFromString(string s);
rarity rarityFromString(string s);
//this function takes a vector of instances and modifies their model matrices for proper display
void offsetArtworks(vector< shared_ptr<artwork_instance> > &art_vec, float eye_level, float starting_z = 0.0f, bool x_only = false);
void addFrames(vector< shared_ptr<artwork_instance> > &art_vec, shared_ptr<ogl_context> context, shared_ptr<ogl_camera> camera, string data_path);

#endif