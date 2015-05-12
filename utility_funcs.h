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
map<int, mat4> getThumbnailMatrixMap(const shared_ptr<ogl_context> &context, const vector<pair<int, shared_ptr<artwork> > > &art_vec,
	float margin_size, float cell_size, float cell_padding_factor);
map<int, mat4> getHighlightMatrixMap(const shared_ptr<ogl_context> &context, const vector<pair<int, shared_ptr<artwork> > > &art_vec,
	const vec2 &screen_position, float cell_size);

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

pair<vec3, vec3> getRayFromCursorPosition(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera);
bool paintingSelected(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera, const shared_ptr<artwork> &art, float &scale);
vector< vector<vec3> > getTriangles(const vector<float> &vec_vertices, int offset, int stride);
vector< pair<vec3, vec3> > getOuterEdges(const vector< vector<vec3> > &triangles);
float getNormalRotation(const vector<float> &vec_vertices, int normal_offset, int stride);

#endif