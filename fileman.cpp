#include "fileman.h"
#include "art_db.h"

template <typename T>
T getFromFile(std::ifstream &file_stream)
{
	T from_file;
	file_stream.read((char*)&from_file, sizeof(T));
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

void savePlayerOld(const string &data_path, const string player_name, const shared_ptr<player> &current_player)
{
	/*
	string loaded_player_name
	unsigned long xp
	unsigned short level
	string bank_balance_string

	unsigned short inventory_size
		unsigned work_id = getFromFile<unsigned>(load_file);

	unsigned short num_galleries
		int gallery_index
		string gallery_template_name

		unsigned short paintings_in_gallery
			unsigned work_id
			float position_x
			float position_y
			unsigned short wall_index
	*/

	cout << "SAVING PLAYER" << endl;
	string file_path = data_path + "gamesave_data\\" + player_name + ".sav";
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

		bool profited = work.second->getProfited();
		writeToFile(save_file, profited);
	}

	map<int, shared_ptr<gallery> > galleries = (current_player->getGalleries());

	writeToFile(save_file, (unsigned short)galleries.size());

	for (const auto &gallery_pair : galleries)
	{
		//map index of gallery in player class
		writeToFile(save_file, gallery_pair.first);
		writeStringToFile(save_file, gallery_pair.second->getTemplateName());

		//work id, wall position, wall index
		const map< unsigned, pair<vec2, unsigned short> > work_map(gallery_pair.second->getWorkMap());
		writeToFile(save_file, (unsigned short)work_map.size());

		for (const auto &map_data : work_map)
		{
			unsigned work_id = map_data.first;
			vec2 wall_position = map_data.second.first;
			unsigned short wall_index = map_data.second.second;

			writeToFile(save_file, work_id);
			writeToFile(save_file, wall_position.x);
			writeToFile(save_file, wall_position.y);
			writeToFile(save_file, wall_index);
		}
	}

	save_file.close();
}

void savePlayer(const string &data_path, const string player_name, const shared_ptr<player> &current_player)
{
	/*
	string loaded_player_name
	unsigned long xp
	unsigned short level
	string bank_balance_string
	time_t last_balance_check

	unsigned short inventory_size
		unsigned work_id

	unsigned short num_galleries
		int gallery_index
		string gallery_template_name
		string gallery_name

		unsigned num_walls
			unsigned short wall index
			string texture name

		unsigned short paintings_in_gallery
			unsigned work_id
			float position_x
			float position_y
			unsigned short wall_index
	*/

	cout << "SAVING PLAYER" << endl;
	string file_path = data_path + "gamesave_data\\" + player_name + ".sav";
	const char* c_path = file_path.c_str();

	std::ofstream save_file;
	save_file.open(c_path, std::ios::binary);

	writeStringToFile(save_file, player_name);
	writeToFile(save_file, current_player->getXP());
	writeToFile(save_file, current_player->getLevel());
	writeStringToFile(save_file, current_player->getBankBalanceString(false));
	
	current_player->updateBank();
	writeToFile(save_file, current_player->getLastBalanceCheck());

	const map<unsigned int, shared_ptr<artwork> > inventory(current_player->getInventory());

	writeToFile(save_file, (unsigned short)inventory.size());

	for (const auto &work : inventory)
	{
		unsigned id = (unsigned)work.first;
		writeToFile(save_file, id);

		bool profited = work.second->getProfited();
		writeToFile(save_file, profited);
	}

	map<int, shared_ptr<gallery> > galleries = (current_player->getGalleries());

	writeToFile(save_file, (unsigned short)galleries.size());

	for (const auto &gallery_pair : galleries)
	{
		//map index of gallery in player class
		writeToFile(save_file, gallery_pair.first);
		writeStringToFile(save_file, gallery_pair.second->getTemplateName());
		writeStringToFile(save_file, gallery_pair.second->getName());

		map<unsigned short, shared_ptr<display_wall> > gallery_walls = gallery_pair.second->getWalls();
		writeToFile(save_file, (unsigned short)gallery_walls.size());

		for (const auto &wall : gallery_walls)
		{
			writeToFile(save_file, wall.second->getIndex());
			writeStringToFile(save_file, wall.second->getTextureFilename());
		}

		//work id, wall position, wall index
		const map< unsigned, pair<vec2, unsigned short> > work_map(gallery_pair.second->getWorkMap());
		writeToFile(save_file, (unsigned short)work_map.size());

		for (const auto &map_data : work_map)
		{
			unsigned work_id = map_data.first;
			vec2 wall_position = map_data.second.first;
			unsigned short wall_index = map_data.second.second;

			writeToFile(save_file, work_id);
			writeToFile(save_file, wall_position.x);
			writeToFile(save_file, wall_position.y);
			writeToFile(save_file, wall_index);
		}
	}

	save_file.close();
}

shared_ptr<player> loadPlayer(const string &data_path, string player_name, const shared_ptr<art_db> &database, const shared_ptr<loot_generator> &loot,
	const shared_ptr<ogl_context> &context, shared_ptr<texture_handler> &textures)
{
	/*
	string loaded_player_name
	unsigned long xp
	unsigned short level
	string bank_balance_string
	time_t last_balance_check

	unsigned short inventory_size
		unsigned work_id = getFromFile<unsigned>(load_file);

	unsigned short num_galleries
		int gallery_index
		string gallery_template_name
		string gallery_name
		unsigned num_walls
			unsigned wall index
			string texture name

		unsigned short paintings_in_gallery
			unsigned work_id
			float position_x
			float position_y
			unsigned short wall_index
	*/

	cout << "CHECKING FOR OLD PLAYER FILE" << endl;
	string file_path = data_path + "\\gamesave_data\\" + player_name + "_old.sav";
	if (fileExists(file_path))
	{
		cout << "REFORMATTING OLD PLAYER FILE" << endl;
		shared_ptr<player> generated_player = loadAndUpdate(data_path, player_name, database, loot, context, textures);
		savePlayer(data_path, player_name, generated_player);
		return generated_player;
	}

	cout << "LOADING PLAYER" << endl;
	file_path = data_path + "\\gamesave_data\\" + player_name + ".sav";
	
	if (fileExists(file_path))
	{
		std::ifstream load_file;
		load_file.open(file_path.c_str(), std::ios::in | std::ios::binary);

		string loaded_player_name = getStringFromFile(load_file);

		unsigned long xp = getFromFile<unsigned long>(load_file);
		unsigned short level = getFromFile<unsigned short>(load_file);
		string bank_balance_string = getStringFromFile(load_file);

		//TODO make new constructor using the loaded parameters
		shared_ptr<player> generated_player(new player(loaded_player_name, context, textures, xp, level, bank_balance_string));
		time_t last_balance_check = getFromFile<time_t>(load_file);
		generated_player->setLastBalanceCheck(last_balance_check);
		cout << "last_balance_check loaded: " << generated_player->getLastBalanceCheck() << endl;

		unsigned short inventory_size = getFromFile<unsigned short>(load_file);

		vector<unsigned> inventory;

		for (int i = 0; i < inventory_size; i++)
		{
			unsigned work_id = getFromFile<unsigned>(load_file);
			shared_ptr<artwork_data> work_data = database->getArtwork(work_id);
			shared_ptr<artwork> work(new artwork(work_data, false, 1.0f));

			bool profited = getFromFile<bool>(load_file);
			work->setProfitedTEMP(profited);

			generated_player->addWorkToInventory(work);
		}

		unsigned short num_galleries = getFromFile<unsigned short>(load_file);

		for (int i = 0; i < num_galleries; i++)
		{
			int gallery_index = getFromFile<int>(load_file);
			string gallery_template_name = getStringFromFile(load_file);
			string gallery_name = getStringFromFile(load_file);

			shared_ptr<gallery> loaded_gallery(new gallery(
				context, textures,
				data_path + "model_data\\",
				data_path + "model_data\\",
				gallery_template_name,
				generated_player->getName(),
				gallery_name
				));

			//unsigned short num_walls
				//unsigned short wall index
				//string texture name

			unsigned short num_walls = getFromFile<unsigned short>(load_file);

			for (int j = 0; j < num_walls; j++)
			{
				unsigned short wall_index = getFromFile<unsigned short>(load_file);
				string texture_filename = getStringFromFile(load_file);
				loaded_gallery->getWall(wall_index)->setTexture(texture_filename, textures);
			}

			unsigned short paintings_in_gallery = getFromFile<unsigned short>(load_file);

			for (int j = 0; j < paintings_in_gallery; j++)
			{
				unsigned work_id = getFromFile<unsigned>(load_file);
				float position_x = getFromFile<float>(load_file);
				float position_y = getFromFile<float>(load_file);
				unsigned short wall_index = getFromFile<unsigned short>(load_file);

				shared_ptr<artwork_data> work_data = database->getArtwork(work_id);
				shared_ptr<artwork> work(new artwork(work_data, false, 1.0f));
				work->applyFrameTemplate(context, textures, *(generated_player->getDefaultFrame()));

				generated_player->addPaintingToDisplay(work);
				loaded_gallery->addArtwork(wall_index, work, vec2(position_x, position_y));
			}

			generated_player->addGallery(loaded_gallery);
		}

		load_file.close();
		generated_player->updateBank();
		return generated_player;
	}

	else
	{
		shared_ptr<player> generated_player(new player(player_name, loot, context, textures, data_path));

		generated_player->addGallery(shared_ptr<gallery>(new gallery(
			context, textures,
			data_path + "model_data\\",
			data_path + "model_data\\",
			"gallery_template_01",
			generated_player->getName(),
			"default gallery"
			)));

		savePlayer(data_path, player_name, generated_player);
		return generated_player;
	}
}

shared_ptr<player> loadAndUpdate(const string &data_path, string player_name, const shared_ptr<art_db> &database, const shared_ptr<loot_generator> &loot,
	const shared_ptr<ogl_context> &context, shared_ptr<texture_handler> &textures)
{
	/*
	string loaded_player_name
	unsigned long xp
	unsigned short level
	string bank_balance_string
	time_t last_balance_check

	unsigned short inventory_size
		unsigned work_id

	unsigned short num_galleries
		int gallery_index
		string gallery_template_name
		string gallery_name

		unsigned short paintings_in_gallery
			unsigned work_id
			float position_x
			float position_y
			unsigned short wall_index
	*/

	//plaster.bmp

	cout << "LOADING PLAYER" << endl;
	string file_path = data_path + "\\gamesave_data\\" + player_name + "_old.sav";

	if (fileExists(file_path))
	{
		std::ifstream load_file;
		load_file.open(file_path.c_str(), std::ios::in | std::ios::binary);

		string loaded_player_name = getStringFromFile(load_file);

		unsigned long xp = getFromFile<unsigned long>(load_file);
		unsigned short level = getFromFile<unsigned short>(load_file);
		string bank_balance_string = getStringFromFile(load_file);

		//TODO make new constructor using the loaded parameters
		shared_ptr<player> generated_player(new player(loaded_player_name, context, textures, xp, level, bank_balance_string));
		time_t last_balance_check = getFromFile<time_t>(load_file);
		generated_player->setLastBalanceCheck(last_balance_check);
		cout << "last_balance_check loaded: " << generated_player->getLastBalanceCheck() << endl;

		unsigned short inventory_size = getFromFile<unsigned short>(load_file);

		vector<unsigned> inventory;

		for (int i = 0; i < inventory_size; i++)
		{
			unsigned work_id = getFromFile<unsigned>(load_file);
			shared_ptr<artwork_data> work_data = database->getArtwork(work_id);
			shared_ptr<artwork> work(new artwork(work_data, false, 1.0f));

			bool profited = getFromFile<bool>(load_file);
			work->setProfitedTEMP(profited);

			generated_player->addWorkToInventory(work);
		}

		unsigned short num_galleries = getFromFile<unsigned short>(load_file);

		for (int i = 0; i < num_galleries; i++)
		{
			int gallery_index = getFromFile<int>(load_file);
			string gallery_template_name = getStringFromFile(load_file);
			string gallery_name = getStringFromFile(load_file);

			shared_ptr<gallery> loaded_gallery(new gallery(
				context, textures,
				data_path + "model_data\\",
				data_path + "model_data\\",
				gallery_template_name,
				generated_player->getName(),
				gallery_name
				));

			unsigned short paintings_in_gallery = getFromFile<unsigned short>(load_file);

			for (int j = 0; j < paintings_in_gallery; j++)
			{
				unsigned work_id = getFromFile<unsigned>(load_file);
				float position_x = getFromFile<float>(load_file);
				float position_y = getFromFile<float>(load_file);
				unsigned short wall_index = getFromFile<unsigned short>(load_file);

				shared_ptr<artwork_data> work_data = database->getArtwork(work_id);
				shared_ptr<artwork> work(new artwork(work_data, false, 1.0f));
				work->applyFrameTemplate(context, textures, *(generated_player->getDefaultFrame()));

				generated_player->addPaintingToDisplay(work);
				loaded_gallery->addArtwork(wall_index, work, vec2(position_x, position_y));
			}

			generated_player->addGallery(loaded_gallery);
		}

		load_file.close();
		generated_player->updateBank();
		return generated_player;
	}

	else
	{
		shared_ptr<player> generated_player(new player(player_name, loot, context, textures, data_path));

		generated_player->addGallery(shared_ptr<gallery>(new gallery(
			context, textures,
			data_path + "model_data\\",
			data_path + "model_data\\",
			"gallery_template_01",
			generated_player->getName(),
			"default gallery"
			)));

		savePlayer(data_path, player_name, generated_player);
		return generated_player;
	}
}

void saveGallery(const string &data_path, const shared_ptr<gallery> &toSave)
{
	/*
	string owner_name
	string gallery_template_name
	string gallery_name

	unsigned short paintings_in_gallery
		unsigned work_id
		float position_x
		float position_y
		unsigned short wall_index
	*/

	cout << "SAVING GALLERY" << endl;
	string file_path = data_path + "gamesave_data\\galleries\\" + toSave->getOwnerName() + "_" + toSave->getName() + ".gal";
	const char* c_path = file_path.c_str();

	std::ofstream save_file;
	save_file.open(c_path, std::ios::binary);

	writeStringToFile(save_file, toSave->getOwnerName());
	writeStringToFile(save_file, toSave->getTemplateName());
	writeStringToFile(save_file, toSave->getName());

	//work id, wall position, wall index
	const map< unsigned, pair<vec2, unsigned short> > work_map(toSave->getWorkMap());
	writeToFile(save_file, (unsigned short)work_map.size());

	for (const auto &map_data : work_map)
	{
		unsigned work_id = map_data.first;
		vec2 wall_position = map_data.second.first;
		unsigned short wall_index = map_data.second.second;

		writeToFile(save_file, work_id);
		writeToFile(save_file, wall_position.x);
		writeToFile(save_file, wall_position.y);
		writeToFile(save_file, wall_index);
	}

	save_file.close();
}

shared_ptr<gallery> loadGallery(const string &data_path, const pair<string, string> &owner_and_name, 
	const shared_ptr<art_db> &database, const shared_ptr<ogl_context> &context, shared_ptr<texture_handler> &textures)
{
	//TODO load/save frame data for each painting
	/*
	string owner_name
	string gallery_template_name
	string gallery_name

	unsigned short paintings_in_gallery
		unsigned work_id
		float position_x
		float position_y
		unsigned short wall_index
	*/

	cout << "LOADING GALLERY" << endl;
	string file_path = data_path + "gamesave_data\\galleries\\" + owner_and_name.first + "_" + owner_and_name.second + ".gal";

	if (fileExists(file_path))
	{
		std::ifstream load_file;
		load_file.open(file_path.c_str(), std::ios::in | std::ios::binary);
		
		string owner_name = getStringFromFile(load_file);
		string gallery_template_name = getStringFromFile(load_file);
		string gallery_name = getStringFromFile(load_file);

		shared_ptr<gallery> loaded_gallery(new gallery(
			context, textures,
			data_path + "model_data\\",
			data_path + "model_data\\",
			gallery_template_name,
			owner_name,
			gallery_name
			));

		unsigned short paintings_in_gallery = getFromFile<unsigned short>(load_file);

		for (int j = 0; j < paintings_in_gallery; j++)
		{
			unsigned work_id = getFromFile<unsigned>(load_file);
			float position_x = getFromFile<float>(load_file);
			float position_y = getFromFile<float>(load_file);
			unsigned short wall_index = getFromFile<unsigned short>(load_file);

			shared_ptr<artwork_data> work_data = database->getArtwork(work_id);
			shared_ptr<artwork> work(new artwork(work_data, false, 1.0f));

			//TODO Replace with frame information
			//work->applyFrameTemplate(context, textures, *(generated_player->getDefaultFrame()));

			loaded_gallery->addArtwork(wall_index, work, vec2(position_x, position_y));
		}

		load_file.close();

		return loaded_gallery;
	}

	else return nullptr;
}

vector<gallery_stub> getAvailableGalleries(string directory_path, const shared_ptr<player> &current_player)
{
	//directory_iterator end_itr;
	vector<gallery_stub> galleries;

	/*
	for (directory_iterator itr(directory_path); itr != end_itr; ++itr)
	{
		string file_path;
		file_path = itr->path().string();
		cout << "file_path: " << file_path << endl;
	}
	*/

	return galleries;
}

