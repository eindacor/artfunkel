#ifndef UTILITY_FUNCS_H
#define UTILITY_FUNCS_H
#include "header.h"

vector<float> generateFrameVertices(vec3 bottom_left, vec3 top_left, vec3 top_right, vec3 bottom_right,
	float uv_map_dimension, char u_axis, char v_axis, vector<unsigned short> &frame_indices);
string getDateString(const date &d, bool include_day);
bignum lookupValue(rarity work_rarity, bignum work_value_scale);
void printDate(date d);
genre genreFromString(string s);
rarity rarityFromString(string s);

string getDateString(const date &d, bool include_day);
string stringFromRarity(rarity r);
string stringFromAttribute(artwork_attribute aa);
string stringFromGenre(genre g);

//this function takes a vector of instances and modifies their model matrices for proper display
//void offsetArtworks(vector<pair<int, shared_ptr<artwork> > > &art_vec, float space_between = 2.0f, float eye_level = 1.65f, float starting_z = 0.0f, bool x_only = false);
vector<shared_ptr<artwork> >::iterator sortArtVec(vector<shared_ptr<artwork> > &art_vec, sort_option sort, 
	const shared_ptr<player> &current_player, bool ascending = true);
void printArtwork(const shared_ptr<artwork> &target);
mat4 calcImageScale(const shared_ptr<artwork> &target, float width_max, float height_max);
vector<vec2> getThumbnailLocations(const shared_ptr<ogl_context> &context, int thumbnail_count, float bottom_margin, float thumbnail_height);
map<int, mat4> getThumbnailMatrixMap(const shared_ptr<ogl_context> &context, const vector<shared_ptr<artwork> > &art_vec,
	float margin_size, float cell_size, float cell_padding_factor);
map<int, mat4> getHighlightMatrixMap(const shared_ptr<ogl_context> &context, const vector<shared_ptr<artwork> > &art_vec,
	const vec2 &screen_position, float cell_size);

//TODO for the love of god, enumerate something
vector< shared_ptr<artwork> >::const_iterator findChunkFirst(
	vector< shared_ptr<artwork> >::const_iterator first,
	vector< shared_ptr<artwork> > &art_vec, int chunk_size, bool forward);

vector< shared_ptr<artwork> >::const_iterator findChunkLast(
	vector< shared_ptr<artwork> >::const_iterator first,
	const vector< shared_ptr<artwork> > &art_vec, int chunk_size);

vector< shared_ptr<artwork> >::const_iterator findChunkEnd(
	vector< shared_ptr<artwork> >::const_iterator first,
	const vector< shared_ptr<artwork> > &art_vec, int chunk_size);

pair<vec3, vec3> getRayFromCursorPosition(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera);
bool paintingSelected(shared_ptr<key_handler> &keys, const shared_ptr<ogl_camera> &camera, const shared_ptr<artwork> &art, float &scale);
vector< vector<vec3> > getTriangles(const vector<float> &vec_vertices, int offset, int stride);
vector< vector<vec3> > getTriangles(const vector<float> &vec_vertices, const vector<unsigned short> &vertex_indices, int offset, int stride);
vector< pair<vec3, vec3> > getOuterEdges(const vector< vector<vec3> > &triangles);
float getNormalRotation(const vector<float> &vec_vertices, int normal_offset, int stride);

//pre and post render effects
void highlight(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera);
void highlightSubtle(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera);
void fullBrightness(const shared_ptr<ogl_context> &context, const shared_ptr<ogl_camera> &camera);

void loadTextures(shared_ptr<texture_handler> &textures);
void readConfigFile(string &data_path, string &username);
bool fileExists(string filename);

pair<float, float> getAttributeMinMax(artwork_attribute aa);
float getAttributeRating(artwork_attribute aa, float actual_rating);
bool attributeIsPrimary(artwork_attribute aa);
bool attributeIsSecondary(artwork_attribute aa);
bool attributeIsDefault(artwork_attribute aa);

#endif