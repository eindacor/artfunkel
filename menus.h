#ifndef MENUS_H
#define MENUS_H

#include "header.h"

int mainMenu(string data_path, const shared_ptr<ogl_context> &context, const shared_ptr<key_handler> &keys, const shared_ptr<text_handler> &text);

int viewInventory(string data_path, const shared_ptr<ogl_context> &context,
	const shared_ptr<key_handler> &keys, const shared_ptr<player> &current_player, const shared_ptr<text_handler> &text);

int openCrate(string data_path, const shared_ptr<ogl_context> &context, const shared_ptr<key_handler> &keys, 
	const shared_ptr<player> &current_player, const shared_ptr<loot_generator> &lg, const shared_ptr<text_handler> &text);

int viewGallery(string data_path, const shared_ptr<ogl_context> &context, shared_ptr<key_handler> keys, 
	const shared_ptr<player> &current_player, const shared_ptr<text_handler> &text);

#endif