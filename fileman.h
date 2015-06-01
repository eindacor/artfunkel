#ifndef FILEMAN_H
#define FILEMAN_H

#include "header.h"
#include "player.h"
#include "gallery.h"
#include "artwork.h"

template <typename T>
T getFromFile(std::ifstream &file_stream);

string getStringFromFile(std::ifstream & file_stream);

template <typename T>
void writeToFile(std::ifstream &file_stream, T toWrite);

void writeStringToFile(std::ifstream &file_stream, string toWrite);

void savePlayer(const string &data_path, const string player_name, const shared_ptr<player> &current_player);
shared_ptr<player> loadPlayer(const string &data_path, string player_name, const shared_ptr<art_db> &database, const shared_ptr<loot_generator> &loot,
	const shared_ptr<ogl_context> &context, shared_ptr<texture_handler> &textures);


#endif