#include "fileman.h"

void save_file(const string &data_path, const string &player_name, const shared_ptr<player> &current_player)
{
	string file_path = data_path + "\\" + player_name + ".sav";
	FILE *save_file;
	save_file = fopen(file_path.c_str(), "wb");

	fprintf(save_file, "%ul", current_player->getXP());
	fprintf(save_file, "%uh", current_player->getLevel());

}