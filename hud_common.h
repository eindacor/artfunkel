#include "header.h"
#include "player.h"
#include "artwork.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"
#include "utility_funcs.h"

void refreshThumbnails(const shared_ptr<ogl_context> &context, const shared_ptr<texture_handler> &textures, const shared_ptr<player> &current_player,
	const vector<shared_ptr<artwork> > &source, shared_ptr<dynamic_hud_array> &container, const vec2 thumbnail_size, float thumbnail_padding, bool addframe = true);

void setWorkInfoFields(const shared_ptr<ogl_context> &context, const shared_ptr<text_handler> &text, shared_ptr<dynamic_hud_array> &container, float scale_modifier = 1.0f);

void setWorkInfoDescription(shared_ptr<dynamic_hud_array> &work_description, const shared_ptr<artwork> &work);

shared_ptr<dynamic_hud_array> generatePlayerInfo(const shared_ptr<ogl_context> &context, const shared_ptr<text_handler> &text, const shared_ptr<player> &current_player);

void refreshPlayerInfo(const shared_ptr<dynamic_hud_array> &player_summary, const shared_ptr<player> &current_player);


