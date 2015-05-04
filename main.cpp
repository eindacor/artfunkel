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

	int drop_count = 10;
	vector< shared_ptr<artwork_instance> > paintings_to_display = loot->generateArtworks(drop_count, 1.0f);
	offsetArtworks(paintings_to_display, eye_level);
	addFrames(paintings_to_display, context, camera, data_path);

	glfwSetTime(0);
	float render_fps = 60.0f;

	//code below is for adding more paintings on the fly
	int add_wait = 30;
	int frame_count = 0;
	bool add_painting = false;

	int menu_return = mainMenu(data_path, context, keys);

	while (menu_return != 4)
	{
		cout << "displayed works from main().... menu_return =  " << menu_return << endl;
		map<int, shared_ptr<artwork_instance> > displayed = current_player->getDisplayed();
		for (auto i : displayed)
			cout << "\t" << i.second->getTitle() << endl;

		switch (menu_return)
		{
		case 0: menu_return = viewGallery(data_path, context, keys, current_player); break;
		case 1: menu_return = viewInventory(data_path, context, keys, current_player); break;
		}
	}

	return 0;

	do
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			camera->updateCamera();
			for (auto i : paintings_to_display)
				i->draw(context, camera);
			
			context->swapBuffers();
			glfwSetTime(0.0f);

			add_painting = keys->checkPress(GLFW_KEY_J);
			if (add_painting)
			{
				if (frame_count == add_wait)
				{
					int add_count = 10;
					vector< shared_ptr<artwork_instance> > paintings_to_add = loot->generateArtworks(add_count, 1.0f);

					int painting_count = paintings_to_display.size();
					float new_z = (painting_count % 10 == 0 ?
						(painting_count / 10) * -4.0f : (painting_count / 10) + 1 * -4.0f);
					offsetArtworks(paintings_to_add, eye_level, new_z);
					addFrames(paintings_to_add, context, camera, data_path);
					paintings_to_display.insert(paintings_to_display.end(), paintings_to_add.begin(), paintings_to_add.end());
					frame_count = 0;
				}

				else frame_count++;
			}

			else frame_count = 0;
		}

		if (keys->checkPress(GLFW_KEY_ESCAPE))
		{
			int menu_return;
			switch (mainMenu(data_path, context, keys))
			{
			case 1: menu_return = viewInventory(data_path, context, keys, current_player); break;
			case 4: return 0;
			}


			if (mainMenu(data_path, context, keys) == 4)
				return 0;
		}

	} while (!glfwWindowShouldClose(context->getWindow()));

	for (auto i : paintings_to_display)
		i->unloadData();

	return 0;
}