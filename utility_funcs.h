#ifndef UTILITY_FUNCS_H
#define UTILITY_FUNCS_H
#include "header.h"

string getDateString(const date &d, bool include_day);
bignum lookupValue(rarity work_rarity, bignum work_value_scale);
void printDate(date d);
genre genreFromString(string s);
rarity rarityFromString(string s);
//this function takes a vector of instances and modifies their model matrices for proper display
void offsetArtworks(vector< shared_ptr<artwork_instance> > &art_vec, float space_between = 2.0f, float eye_level = 1.65f, float starting_z = 0.0f, bool x_only = false);
void offsetArtworks(map<int, shared_ptr<artwork_instance> > &art_vec, float space_between = 2.0f, float eye_level = 1.65f, float starting_z = 0.0f, bool x_only = false);
void addFrames(vector< shared_ptr<artwork_instance> > &art_vec, shared_ptr<ogl_context> context, shared_ptr<ogl_camera> camera, string data_path);

#endif