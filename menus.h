#ifndef MENUS_H
#define MENUS_H

#include "header.h"

enum select_mode {NOT_SET, PLACE_PAINTING};

int mainMenu(string data_path, const shared_ptr<ogl_context> &context, const shared_ptr<key_handler> &keys, 
	const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures);

int viewInventory_HUD(string data_path, const shared_ptr<ogl_context> &context,
	shared_ptr<key_handler> &keys, const shared_ptr<player> &current_player, 
	const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures);

int openCrate_HUD(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> &keys,
	const shared_ptr<player> &current_player, const shared_ptr<loot_generator> &lg, 
	const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures);

int viewGallery_HUD(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> keys,
	const shared_ptr<player> &current_player, const shared_ptr<text_handler> &text, shared_ptr<texture_handler> &textures);

#endif