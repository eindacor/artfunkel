#include "fileman.h"

void save_file(const string &data_path, const string &player_name, const shared_ptr<player> &current_player)
{
	string file_path = data_path + "\\" + player_name + ".sav";
	std::ofstream file_out(file_path.c_str(), std::ios::binary);
	file_out.open(file_path.c_str(), std::ios::binary | std::ios::out);

	
}