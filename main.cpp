#include "header.h"
#include "artist.h"
#include "artwork.h"
#include "art_db.h"
#include "utility_funcs.h"
#include "loot.h"
#include "menus.h"
#include "player.h"
#include "gallery.h"

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
	string text_file = data_path + "images\\text_template.bmp";
	string vert_file = data_path + "vertex_shader.glsl";
	string frag_file = data_path + "fragment_shader.glsl";

	float eye_level = 1.65f;
	shared_ptr<ogl_context> context(new ogl_context("Artfunkel", vert_file.c_str(), frag_file.c_str(), 1280, 720));
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<text_handler> text(new text_handler(context, text_file.c_str()));
	shared_ptr<texture_handler> textures(new texture_handler(data_path + "\\" + "model_data"));
	loadTextures(textures);

	shared_ptr<art_db> artist_database(new art_db(artists_path.c_str(), paintings_path.c_str(), images_path.c_str()));
	shared_ptr<loot_generator> loot(new loot_generator(artist_database));

	shared_ptr<player> current_player(new player("Test Player", loot, context, textures, data_path));

	int menu_return = mainMenu(data_path, context, keys, text, textures);

	//TODO possibly give context a text handler
	while (menu_return != 4 && menu_return != 3)
	{
		switch (menu_return)
		{
		case 0: menu_return = viewGallery_HUD(data_path, context, keys, current_player, text, textures); break;
		case 1: menu_return = viewInventory_HUD(data_path, context, keys, current_player, text, textures); break;
		case 2: menu_return = openCrate_HUD(data_path, context, keys, current_player, loot, text, textures); break;
		}
	}

	return 0;
}