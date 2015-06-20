#ifndef HEADER_H
#define HEADER_H

enum genre {
	OTHER, DADAISM, SURREALISM, IMPRESSIONISM, POST_IMPRESSIONISM,
	PORTRAIT, ABSTRACT, EXPRESSIONISM, CONTEMPORARY, SCENE, MODERNISM,
	POST_MODERNISM, POINTISM, SKETCH, POP_ART, HISTORIC, LANDSCAPE, 
	FOLK_ART, FAUVISM, CUBISM, PHOTOGRAPH, UNKNOWN_GENRE
};
enum rarity { COMMON, UNCOMMON, RARE, LEGENDARY, MASTERPIECE, UNKNOWN_RARITY };
enum sort_option { ARTIST_NAME, GENRE, STYLE, MEDIUM, DATE, BASE_VALUE, VALUE, TITLE, AREA, HEIGHT, WIDTH, RARITY, PAINTING_ID, IN_GALLERY, NO_SORT };
enum hud_element_type { ELEMENT_ARRAY, THUMBNAIL, FINISH_THUMBNAIL, FURNITURE_THUMBNAIL, TEXT_AREA, TEXT_FIELD, BUTTON_TOGGLE, BUTTON_PRESS, NO_TYPE };
enum vertical_justification { V_TOP, V_MIDDLE, V_BOTTOM, V_STRETCH };
enum horizontal_justification { H_LEFT, H_CENTER, H_RIGHT, H_STRETCH };
enum finish_type { FLOOR_FINISH, WALL_FINISH };

enum npc_type { AUCTIONEER, DEALER, COLLECTOR, DONOR, BENEFACTOR, ENTHUSIAST, DESIGNER, FORGER, ART_EXPERT, HISTORIAN, PRESERVATIONIST, MARKET_EXPERT, NULL_NPC_TYPE };

enum primary_attribute { 
	ENTRY_FEE_REDUCITON, 
	NPC_SPAWN_CHANCE_INCREASE, 
	INCREASE_XP_FROM_SET_PAINTINGS_FOR_VISITORS, 
	INCREASE_XP_FROM_GALLERY_PAINTINGS_FOR_VISITORS,
	EARNING_DURATION_REDUCTION_FROM_SET_PAINTINGS_FOR_VISITORS,
	EARNING_DURATION_REDUCTION_FROM_GALLERY_PAINTINGS_FOR_VISITORS,
	NPC_BOOST_AUCTIONEER, NPC_BOOST_DEALER, NPC_BOOST_COLLECTOR, NPC_BOOST_DONOR, NPC_BOOST_BENEFACTOR, NPC_BOOST_ENTHUSIAST, 
	NPC_BOOST_DESIGNER, NPC_BOOST_FORGER, NPC_BOOST_ART_EXPERT, NPC_BOOST_HISTORIAN, NPC_BOOST_PRESERVATIONIST, NPC_BOOST_MARKET_EXPERT,
	NULL_PRIMARY_ATTRIBUTE
};

enum secondary_attribute {
	INCREASE_XP_FROM_SET_PAINTINGS_FOR_FRIENDS,
	INCREASE_XP_FROM_GALLERY_PAINTINGS_FOR_FRIENDS,
	EARNING_DURATION_REDUCTION_FROM_SET_PAINTINGS_FOR_FRIENDS,
	EARNING_DURATION_REDUCTION_FROM_GALLERY_PAINTINGS_FOR_FRIENDS,
	XP_PER_VISITOR_INTERACTION,
	REP_BOOST,
	MONEY_PER_VISITOR_INTERACTION,
	NULL_SECONDARY_ATTRIBUTE
};

enum artwork_attribute {
	BASE_INCOME_PER_SEC,
	BASE_XP_EARNED,
	BASE_XP_DURATION,

	NPC_AUCTIONEER_BASE,
	NPC_DEALER_BASE,
	NPC_COLLECTOR_BASE,
	NPC_DONOR_BASE,
	NPC_BENEFACTOR_BASE,
	NPC_ENTHUSIAST_BASE,
	NPC_DESIGNER_BASE,
	NPC_FORGER_BASE,
	NPC_ART_EXPERT_BASE,
	NPC_HISTORIAN_BASE,
	NPC_PRESERVATIONIST_BASE,
	NPC_MARKET_EXPERT_BASE,

	ENTRY_FEE_REDUCTION_VISITORS,
	XP_FROM_SET_WORKS_INCREASE_VISITORS,
	XP_FROM_WORKS_INCREASE_VISITORS,
	XP_DURATION_FOR_SET_WORKS_DECREASE_VISITORS,
	XP_DURATION_FOR_WORKS_DECREASE_VISITORS,

	NPC_AUCTIONEER_BOOST,
	NPC_DEALER_BOOST,
	NPC_COLLECTOR_BOOST,
	NPC_DONOR_BOOST,
	NPC_BENEFACTOR_BOOST,
	NPC_ENTHUSIAST_BOOST,
	NPC_DESIGNER_BOOST,
	NPC_FORGER_BOOST,
	NPC_ART_EXPERT_BOOST,
	NPC_HISTORIAN_BOOST,
	NPC_PRESERVATIONIST_BOOST,
	NPC_MARKET_EXPERT_BOOST,

	ENTRY_FEE_REDUCTION_MEMBERS,
	XP_FROM_SET_WORKS_INCREASE_MEMBERS,
	XP_FROM_WORKS_INCREASE_MEMBERS,
	XP_DURATION_FOR_SET_WORKS_DECREASE_MEMBERS,
	XP_DURATION_FOR_WORKS_DECREASE_MEMBERS,

	XP_GAIN_FOR_PLAYER_INTERACTION,
	MONEY_GAIN_FOR_PLAYER_INTERACTION
};

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <list>
#include <iostream>
#include <time.h>
#include <boost/smart_ptr/shared_ptr.hpp>
//#include <boost/filesystem.hpp>
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
class image;
class hud_element;
class hud;
class artwork_thumbnail;
class gallery_stub;

using std::vector;
using std::list;
using std::string;
using std::cout;
using std::endl;
using std::map;
using std::multimap;
using std::pair;

using boost::shared_ptr;
//using namespace boost::filesystem;

using jep::ogl_context;
using jep::ogl_camera;
using jep::ogl_camera_free;
using jep::static_text;
using jep::text_handler;
using jep::key_handler;
using jep::texture_handler;
using jep::text_character;
using jep::date;
using jep::bignum;
using jep::line;
using jep::rectangle;
using jep::render_type;
using jep::NORMAL;
using jep::TEXT;
using jep::ABSOLUTE;
using jep::text_justification;
using jep::LL;
using jep::UL;
using jep::UR;
using jep::LR;

using glm::vec4;
using glm::vec3;
using glm::vec2;
using glm::mat4;

typedef pair<horizontal_justification, vertical_justification> justpair;

string getDateString(const date &d, bool include_day);
string stringFromRarity(rarity r);
string stringFromGenre(genre g);

#endif