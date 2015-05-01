#ifndef HEADER_H
#define HEADER_H

enum genre {
	OTHER, DADAISM, SURREALISM, IMPRESSIONISM, POST_IMPRESSIONISM,
	PORTRAIT, ABSTRACT, EXPRESSIONISM, CONTEMPORARY, SCENE, MODERNISM,
	POST_MODERNISM, POINTISM, SKETCH, POP_ART, HISTORIC, LANDSCAPE, 
	FOLK_ART, FAUVISM, CUBISM, PHOTOGRAPH, UNKNOWN_GENRE
};
enum rarity { COMMON, UNCOMMON, RARE, LEGENDARY, MASTERPIECE, UNKNOWN_RARITY };

#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <iostream>
#include <boost/smart_ptr/shared_ptr.hpp>
#include "jep_util.h"
#include "jeploot.h"
#include "ogl_tools.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class artist;
class artwork_data;
class artwork_instance;

using std::vector;
using std::list;
using std::string;
using std::cout;
using std::endl;
using boost::shared_ptr;
using jep::ogl_context;
using jep::ogl_camera;
using jep::ogl_camera_free;
using jep::key_handler;

using glm::vec4;
using glm::vec3;
using glm::mat4;

string getDateString(const jep::date &d, bool include_day);
double lookupValue(rarity work_rarity);
string stringFromRarity(rarity r);
string stringFromGenre(genre g);

#endif