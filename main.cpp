#include "header.h"
#include "artist.h"
#include "artwork.h"
#include "art_db.h"
#include "utility_funcs.h"
#include "loot.h"
#include "menus.h"
#include "player.h"
#include "gallery.h"
#include "fileman.h"

int main(int argc, char* argv[])
{
	string data_path;
	string username;

	readConfigFile(data_path, username);

	cout << "data_path: " << data_path << endl;
	cout << "username: " << username << endl;

	srand(time(NULL));
	//jep::init();

	int id_count = 0;
	
	string paintings_path = data_path + "paintings.csv";
	string artists_path = data_path + "artists.csv";
	string images_path = data_path + "images\\paintings\\";
	string text_file = data_path + "images\\text_template.bmp";
	string vert_file = data_path + "vertex_shader.glsl";
	string frag_file = data_path + "fragment_shader.glsl";

	getAvailableGalleries("J:\\GitHub\\artfunkel\\gamesave_data\\galleries", nullptr);

	float eye_level = 1.65f;
	shared_ptr<ogl_context> context(new ogl_context("Artfunkel", vert_file.c_str(), frag_file.c_str(), 1280, 720));
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<texture_handler> textures(new texture_handler(data_path + "\\" + "model_data"));
	loadTextures(textures);
	shared_ptr<text_handler> text(new text_handler(context, textures->getTexture("text_template.bmp"), "transparency_color", vec4(0.0f, 1.0f, 0.0f, 1.0f)));

	//string myriad_string = data_path + "\\" + "model_data\\myriad_pro.bmp";	
	//text->addFont("myriad_pro", myriad_string.c_str());
	//text->switchFont("myriad_pro");
	
	shared_ptr<art_db> artist_database(new art_db(artists_path.c_str(), paintings_path.c_str(), images_path.c_str()));
	shared_ptr<loot_generator> loot(new loot_generator(artist_database));

	shared_ptr<player> current_player = nullptr;
	current_player = loadPlayer(data_path, username, artist_database, loot, context, textures);
	savePlayer(data_path, current_player->getName(), current_player);

	int menu_return = mainMenu(data_path, context, keys, current_player, text, textures);

	//TODO possibly give context a text handler
	while (menu_return != 4 && menu_return != 3)
	{
		current_player->updateBank();

		switch (menu_return)
		{
		//case -1: menu_return = menuTest(data_path, context, keys, current_player, loot, text, textures); break;
		case 0: menu_return = editGallery(data_path, context, keys, current_player, loot, text, textures); break;
		case 1: menu_return = viewInventory(data_path, context, keys, current_player, text, textures); break;
		case 2: menu_return = visitStore(data_path, context, keys, current_player, loot, text, textures); break;
		}
	}

	return 0;
}