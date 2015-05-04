#include "header.h"
#include "artist.h"
#include "artwork.h"
#include "art_db.h"
#include "utility_funcs.h"
#include "loot.h"
#include "menus.h"
#include "player.h"

int main(int argc, char* argv[])
{
	jep::init();

	int id_count = 0;

	string data_path = "";

	if (argc > 1)
	{
		data_path = argv[1];
		data_path += "\\";
	}

	else
	{
		//data_path = "C:\\Users\\Joseph\\Documents\\GitHub\\artfunkel\\";		//LAPTOP
		data_path = "J:\\GitHub\\artfunkel\\";								//DESKTOP
	}
	
	string paintings_path = data_path + "paintings.csv";
	string artists_path = data_path + "artists.csv";
	string images_path = data_path + "images\\paintings\\";

	string vert_file = data_path + "vertex_shader.glsl";
	string frag_file = data_path + "fragment_shader.glsl";

	float eye_level = 1.65f;
	shared_ptr<ogl_context> context(new ogl_context("Artfunkel", vert_file.c_str(), frag_file.c_str(), 1020, 700));	//LAPTOP
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<ogl_camera> camera(new ogl_camera_free(keys, vec3(0.0f, eye_level, 5.0f)));

	shared_ptr<art_db> artist_database(new art_db(artists_path.c_str(), paintings_path.c_str(), images_path.c_str()));
	shared_ptr<loot_generator> loot(new loot_generator(artist_database));

	shared_ptr<player> current_player(new player("Test Player", loot, context, data_path));

	int menu_return = mainMenu(data_path, context, keys);

	while (menu_return != 4)
	{
		switch (menu_return)
		{
		case 0: menu_return = viewGallery(data_path, context, keys, current_player); break;
		case 1: menu_return = viewInventory(data_path, context, keys, current_player); break;
		case 2: menu_return = openCrate(data_path, context, keys, current_player, loot); break;
		}
	}

	return 0;
}