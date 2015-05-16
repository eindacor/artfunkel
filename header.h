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
enum hud_element_type { ELEMENT_ARRAY, THUMBNAIL, TEXT_BOX, TEXT_FIELD, BUTTON_TOGGLE, BUTTON_PRESS, NO_TYPE };
enum vertical_justification { V_TOP, V_MIDDLE, V_BOTTOM, V_STRETCH };
enum horizontal_justification { H_LEFT, H_CENTER, H_RIGHT, H_STRETCH };

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
#include "obj_parser.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/string_cast.hpp>
#include <gtx/intersect.hpp>

class artist;
class artwork_data;
class artwork;
class art_db;
class painting_surface;
class frame_model;
class player;
class gallery;
class display_wall;
class loot_generator;
class line;
class rectangle;
class image;
class hud_element;
class hud;
class artwork_thumbnail;

using std::vector;
using std::list;
using std::string;
using std::cout;
using std::endl;
using std::map;
using std::multimap;
using std::pair;

using boost::shared_ptr;

using jep::ogl_context;
using jep::ogl_camera;
using jep::ogl_camera_free;
using jep::static_text;
using jep::text_handler;
using jep::key_handler;
using jep::date;
using jep::bignum;
using jep::render_type;

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