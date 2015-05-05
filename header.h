#ifndef HEADER_H
#define HEADER_H

enum genre {
	OTHER, DADAISM, SURREALISM, IMPRESSIONISM, POST_IMPRESSIONISM,
	PORTRAIT, ABSTRACT, EXPRESSIONISM, CONTEMPORARY, SCENE, MODERNISM,
	POST_MODERNISM, POINTISM, SKETCH, POP_ART, HISTORIC, LANDSCAPE, 
	FOLK_ART, FAUVISM, CUBISM, PHOTOGRAPH, UNKNOWN_GENRE
};
enum rarity { COMMON, UNCOMMON, RARE, LEGENDARY, MASTERPIECE, UNKNOWN_RARITY };

enum sort_options { ARTIST_NAME, GENRE, STYLE, MEDIUM, DATE, BASE_VALUE, VALUE, TITLE, AREA, HEIGHT, WIDTH, RARITY, PAINTING_ID, NO_SORT };

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <list>
#include <iostream>
#include <boost/smart_ptr/shared_ptr.hpp>
#include "jep_util.h"
#include "jeploot.h"
#include "ogl_tools.h"
#include "JEPbignum.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class artist;
class artwork_data;
class artwork_instance;
class art_db;
class painting_surface;
class frame_model;
class player;
class gallery;
class loot_generator;

using std::vector;
using std::list;
using std::string;
using std::cout;
using std::endl;
using std::map;
using std::pair;

using boost::shared_ptr;

using jep::ogl_context;
using jep::ogl_camera;
using jep::ogl_camera_free;
using jep::key_handler;
using jep::date;
using jep::bignum;

using glm::vec4;
using glm::vec3;
using glm::vec2;
using glm::mat4;

string getDateString(const date &d, bool include_day);
string stringFromRarity(rarity r);
string stringFromGenre(genre g);
vector<float> generateInterleavedVertices(vec3 bottom_left, vec3 top_left, vec3 top_right, vec3 bottom_right,
	float uv_map_dimension, char u_axis, char v_axis);

#endif