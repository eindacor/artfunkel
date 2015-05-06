#ifndef UTILITY_FUNCS_H
#define UTILITY_FUNCS_H
#include "header.h"

string getDateString(const date &d, bool include_day);
bignum lookupValue(rarity work_rarity, bignum work_value_scale);
void printDate(date d);
genre genreFromString(string s);
rarity rarityFromString(string s);
//this function takes a vector of instances and modifies their model matrices for proper display
void offsetArtworks(vector<pair<int, shared_ptr<artwork_instance> > > &art_vec, float space_between = 2.0f, float eye_level = 1.65f, float starting_z = 0.0f, bool x_only = false);
void addFrames(vector< shared_ptr<artwork_instance> > &art_vec, shared_ptr<ogl_context> context, shared_ptr<ogl_camera> camera, string data_path);
vector<pair<int, shared_ptr<artwork_instance> > >::iterator sortArtVec(vector<pair<int, shared_ptr<artwork_instance> > > &art_vec, sort_options sort, bool ascending = true);
void printArtworkInstance(const shared_ptr<artwork_instance> &target);
mat4 calcThumbnailScale(const shared_ptr<artwork_instance> &target, float width_max, float height_max);
vector<pair<int, shared_ptr<artwork_instance> > >::iterator makeThumbnails(vector<pair<int, shared_ptr<artwork_instance> > > &art_vec,
	const shared_ptr<ogl_context> &context, float margin_size, int items_to_display, vector<pair<int, shared_ptr<artwork_instance> > >::iterator first_element);

#endif