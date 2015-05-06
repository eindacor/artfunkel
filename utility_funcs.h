#ifndef UTILITY_FUNCS_H
#define UTILITY_FUNCS_H
#include "header.h"

string getDateString(const date &d, bool include_day);
bignum lookupValue(rarity work_rarity, bignum work_value_scale);
void printDate(date d);
genre genreFromString(string s);
rarity rarityFromString(string s);
//this function takes a vector of instances and modifies their model matrices for proper display
void offsetArtworks(vector<pair<int, shared_ptr<artwork> > > &art_vec, float space_between = 2.0f, float eye_level = 1.65f, float starting_z = 0.0f, bool x_only = false);
void addFrames(vector< shared_ptr<artwork> > &art_vec, shared_ptr<ogl_context> context, shared_ptr<ogl_camera> camera, string data_path);
vector<pair<int, shared_ptr<artwork> > >::iterator sortArtVec(vector<pair<int, shared_ptr<artwork> > > &art_vec, sort_options sort, bool ascending = true);
void printArtwork(const shared_ptr<artwork> &target);
mat4 calcImageScale(const shared_ptr<artwork> &target, float width_max, float height_max);
void makeThumbnails(vector<pair<int, shared_ptr<artwork> > > &art_vec, float margin_size, float cell_size);
void makeHighlight(shared_ptr<artwork> target, float top_margin, float bottom_margin, float cell_width);

//TODO for the love of god, enumerate something
vector<pair<int, shared_ptr<artwork> > >::const_iterator findChunkFirst(
	vector<pair<int, shared_ptr<artwork> > >::const_iterator first,
	const vector<pair<int, shared_ptr<artwork> > > &art_vec, int chunk_size, bool forward);

vector<pair<int, shared_ptr<artwork> > >::const_iterator findChunkLast(
	vector<pair<int, shared_ptr<artwork> > >::const_iterator first,
	const vector<pair<int, shared_ptr<artwork> > > &art_vec, int chunk_size);

vector<pair<int, shared_ptr<artwork> > >::const_iterator findChunkEnd(
	vector<pair<int, shared_ptr<artwork> > >::const_iterator first,
	const vector<pair<int, shared_ptr<artwork> > > &art_vec, int chunk_size);

#endif