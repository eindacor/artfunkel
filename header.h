#ifndef HEADER_H
#define HEADER_H

#include <string>
#include <vector>
#include <iostream>
#include <boost/smart_ptr/shared_ptr.hpp>
#include "jep_util.h"
#include "jeploot.h"
#include "ogl_tools.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class artist;
class artwork;

using std::vector;
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

enum genre { OTHER, DADAISM, SURREALISM, IMPRESSIONISM, POST_IMPRESSIONISM, 
			PORTRAIT, ABSTRACT, EXPRESSIONISM, CONTEMPORARY, SCENE, MODERNISM, 
			POST_MODERNISM, POINTISM, SKETCH };
enum rarity { UNKNOWN_RARITY, COMMON, UNCOMMON, RARE, ULTRA, LEGENDARY, MASTERPIECE};

string getDateString(const jep::date &d, bool include_day);
double lookupValue(rarity work_rarity);

#endif