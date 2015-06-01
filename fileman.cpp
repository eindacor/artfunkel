#include "fileman.h"
#include "art_db.h"

template <typename T>
T getFromFile(std::ifstream &file_stream)
{
	T from_file;
	file_stream.read((char*)&from_file, sizeof(T));
	cout << "from file: " << from_file << endl;
	return from_file;
}

string getStringFromFile(std::ifstream &file_stream)
{
	unsigned int string_size = getFromFile<unsigned int>(file_stream);
	char* buffer = new char[string_size];
	file_stream.read(buffer, string_size);

	string from_file(buffer, string_size);
	delete[] buffer;

	return from_file;

	//from_file.reserve(string_size);
	//file_stream.read((char*)&from_file, sizeof(from_file));
	//return from_file;
}

template <typename T>
void writeToFile(std::ofstream &file_stream, T toWrite)
{
	file_stream.write((char*)&toWrite, sizeof(T));
}

void writeStringToFile(std::ofstream &file_stream, string toWrite)
{
	unsigned int string_length = toWrite.length();
	writeToFile(file_stream, string_length);
	file_stream.write(toWrite.c_str(), string_length);
}

void savePlayer(const string &data_path, const string player_name, const shared_ptr<player> &current_player)
{
	cout << "SAVING PLAYER" << endl;
	string file_path = data_path + player_name + ".sav";
	const char* c_path = file_path.c_str();

	std::ofstream save_file;
	save_file.open(c_path, std::ios::binary);

	writeStringToFile(save_file, player_name);
	writeToFile(save_file, current_player->getXP());
	writeToFile(save_file, current_player->getLevel());
	writeStringToFile(save_file, current_player->getBankBalanceString(false));

	const map<unsigned int, shared_ptr<artwork> > inventory(current_player->getInventory());

	writeToFile(save_file, (unsigned short)inventory.size());

	for (const auto &work : inventory)
	{
		unsigned id = (unsigned)work.first;
		writeToFile(save_file, id);
	}

	map<int, shared_ptr<gallery> > galleries = (current_player->getGalleries());

	writeToFile(save_file, (unsigned short)galleries.size());
	cout << "gallery count: " << galleries.size() << endl;

	for (const auto &gallery_pair : galleries)
	{
		//map index of gallery in player class
		writeToFile(save_file, gallery_pair.first);
		writeToFile(save_file, gallery_pair.second->getTemplateID());

		//work id, wall position, wall index
		const map< unsigned, pair<vec2, unsigned short> > work_map(gallery_pair.second->getWorkMap());
		writeToFile(save_file, (unsigned short)work_map.size());

		cout << "paintings in work map: " << work_map.size() << endl;

		for (const auto &map_data : work_map)
		{
			unsigned work_id = map_data.first;
			vec2 wall_position = map_data.second.first;
			unsigned short wall_index = map_data.second.second;

			writeToFile(save_file, wall_index);
			writeToFile(save_file, wall_position.x);
			writeToFile(save_file, wall_position.y);
			writeToFile(save_file, work_id);
		}
	}

	save_file.close();
}

shared_ptr<player> loadPlayer(const string &data_path, string player_name, const shared_ptr<art_db> &database, const shared_ptr<loot_generator> &loot,
	const shared_ptr<ogl_context> &context, shared_ptr<texture_handler> &textures)
{
	cout << "LOADING PLAYER" << endl;
	string file_path = data_path + "\\" + player_name + ".sav";
	
	if (fileExists(file_path))
	{
		std::ifstream load_file;
		load_file.open(file_path.c_str(), std::ios::in | std::ios::binary);

		//unsigned short name_size;
		//load_file.read((char*)&name_size, sizeof(name_size));	

		//string loaded_player_name;
		//loaded_player_name.reserve(name_size);
		//load_file.read((char*)&loaded_player_name, sizeof(loaded_player_name));

		string loaded_player_name = getStringFromFile(load_file);

		unsigned long xp = getFromFile<unsigned long>(load_file);
		unsigned short level = getFromFile<unsigned short>(load_file);

		string bank_balance_string = getStringFromFile(load_file);

		unsigned short inventory_size = getFromFile<unsigned short>(load_file);

		vector<unsigned> inventory;

		for (int i = 0; i < inventory_size; i++)
		{
			inventory.push_back(getFromFile<unsigned>(load_file));
			cout << "work id: " << inventory.at(i) << endl;
		}

		unsigned short num_galleries = getFromFile<unsigned short>(load_file);
		cout << "gallery count: " << num_galleries << endl;

		for (int i = 0; i < num_galleries; i++)
		{
			int gallery_index = getFromFile<int>(load_file);
			cout << "gallery index: " << gallery_index << endl;
			unsigned gallery_template_id = getFromFile<unsigned>(load_file);
			cout << "gallery template id: " << gallery_template_id << endl;

			unsigned short paintings_in_gallery = getFromFile<unsigned short>(load_file);
			cout << "paintings in gallery: " << paintings_in_gallery << endl;

			for (int j = 0; j < paintings_in_gallery; j++)
			{
				unsigned work_id = getFromFile<unsigned short>(load_file);
				cout << "work id: " << work_id << endl;
				float position_x = getFromFile<float>(load_file);
				float position_y = getFromFile<float>(load_file);
				cout << "wall position: " << position_x << ", " << position_y << endl;
				unsigned short wall_index = getFromFile<unsigned>(load_file);
				cout << "wall index: " << wall_index << endl;
			}
		}

		load_file.close();

		//TODO make new constructor using the loaded parameters
		shared_ptr<player> generated_player(new player(loaded_player_name, context, textures, xp, level, bank_balance_string));

		for (const auto &i : inventory)
		{
			shared_ptr<artwork_data> work_data = database->getArtwork(i);
			shared_ptr<artwork> work(new artwork(work_data, false, 1.0f));
			generated_player->addWorkToInventory(work);
		}

		return generated_player;
	}

	else
	{
		shared_ptr<player> generated_player(new player(player_name, loot, context, textures, data_path));
		savePlayer(data_path, player_name, generated_player);
		return generated_player;
	}
}