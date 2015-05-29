#ifndef FILEMAN_H
#define FILEMAN_H

#include "header.h"
#include "player.h"
#include "gallery.h"
#include "artwork.h"

void save_file(const string &data_path, const string &player_name, const shared_ptr<player> &current_player);

#endif