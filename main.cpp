#include "header.h"
#include "artist.h"
#include "artwork.h"
#include "art_db.h"
#include "utility_funcs.h"
#include "loot.h"

//this function takes a vector of instances and modifies their model matrices for proper display
void offsetArtworks(vector< shared_ptr<artwork_instance> > &art_vec, float eye_level)
{
	float x_offset = 0.0f;
	float z_offset = 0.0f;
	float previous_width = 0.0f;
	int display_count = 0;

	for (auto i : art_vec)
	{
		//center on eye level, unless painting is within .5 of floor
		float y_offset = 0.0f;
		float min_distance_from_floor = .5f;
		if ((i->getHeight() * .0067f) + min_distance_from_floor > eye_level)
			y_offset = (i->getHeight() / 200.0f) + min_distance_from_floor;

		else y_offset = eye_level - (i->getHeight() / 600.0f);

		float space_between = 2.0f;
		float buffer = (previous_width / 200.0f) + space_between + (i->getWidth() / 200.0f);
		if (display_count % 10 == 0)
		{
			x_offset = 0.0f;
			z_offset -= 4.0f;
			previous_width = 0.0f;
			buffer = 0.0f;
		}

		else
			x_offset += buffer;

		previous_width = i->getWidth();
		i->moveAbsolute(glm::translate(mat4(1.0f), vec3(x_offset, y_offset, z_offset)));
		//i->getSurface()->moveAbsolute(glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.0f * float(display_count))));
		display_count++;
	}
}

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
		data_path = "C:\\Users\\Joseph\\Documents\\GitHub\\artfunkel\\";		//LAPTOP
		//data_path = "J:\\GitHub\\artfunkel\\";								//DESKTOP
	}
	
	string paintings_path = data_path + "paintings.csv";
	string artists_path = data_path + "artists.csv";
	string images_path = data_path + "images\\paintings\\";

	string vert_file = data_path + "vertex_shader.glsl";
	string frag_file = data_path + "fragment_shader.glsl"; 

	float eye_level = 1.65f;
	//shared_ptr<ogl_context> context(new ogl_context("Artfunkel", vert_file.c_str(), frag_file.c_str(), 1240, 960));		//DESKTOP
	shared_ptr<ogl_context> context(new ogl_context("Artfunkel", vert_file.c_str(), frag_file.c_str(), 1020, 700));	//LAPTOP
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<ogl_camera> camera(new ogl_camera_free(keys, vec3(0.0f, eye_level, 5.0f)));
	shared_ptr<ogl_camera> alt_camera(new ogl_camera_free(keys, vec3(0.0f, eye_level + 10.0f, 5.0f)));

	shared_ptr<art_db> artist_database(new art_db(artists_path.c_str(), paintings_path.c_str(), images_path.c_str()));
	loot_generator loot(artist_database);

	//create a map of rarities, with proportions, to pass to jep::catRoll
	//move these to a separate function in loot class
	map<rarity, unsigned> rarity_map = {
		std::pair<rarity, int>(COMMON, 720),
		std::pair<rarity, int>(UNCOMMON, 360),
		std::pair<rarity, int>(RARE, 120),
		std::pair<rarity, int>(ULTRA, 30),
		std::pair<rarity, int>(LEGENDARY, 6),
		std::pair<rarity, int>(MASTERPIECE, 1)
	};

	int rarity_sumtotal = 0;
	for (auto i : rarity_map)
		rarity_sumtotal += i.second;

	cout << "Drop chance: " << endl;
	for (auto i : rarity_map)
		cout << "\t" << stringFromRarity(i.first) << ": " << 100.0f * ((float)i.second / (float)rarity_sumtotal) << "%" << endl;

	int drop_count = 10;
	vector< shared_ptr<artwork_instance> > paintings_to_display = loot.generateArtworks(drop_count, rarity_map);

	cout << "Crate Contents: " << endl;
	for (auto i : paintings_to_display)
		cout << "\t" << i->getTitle() << " by " << i->getArtistName() << 
		" (" << stringFromRarity(i->getRarity()) << ")" << endl;

	map<rarity, unsigned> frequency_results{
		std::pair<rarity, int>(COMMON, 0),
		std::pair<rarity, int>(UNCOMMON, 0),
		std::pair<rarity, int>(RARE, 0),
		std::pair<rarity, int>(ULTRA, 0),
		std::pair<rarity, int>(LEGENDARY, 0),
		std::pair<rarity, int>(MASTERPIECE, 0)
	};
	for (auto i : paintings_to_display)
		frequency_results[i->getRarity()]++;

	cout << "Frequency results..." << endl;
	for (auto i : frequency_results)
		cout << "\t" << stringFromRarity(i.first) << ": " << i.second << endl;

	offsetArtworks(paintings_to_display, eye_level);

	//code below filters and displays
	/*
	list< shared_ptr<artwork> > target_list = artist_database->getWorksByRarity(ULTRA, false);
	for (auto i : target_list)
	{
		//center on eye level, unless painting is within .5 of floor
		float y_offset = 0.0f;
		float min_distance_from_floor = .5f;
		if ((i->getHeight() * .0067f) + min_distance_from_floor > eye_level)
			y_offset = (i->getHeight() / 200.0f) + min_distance_from_floor;

		else y_offset = eye_level - (i->getHeight() / 600.0f);

		float space_between = 2.0f;
		float buffer = (previous_width / 200.0f) + space_between + (i->getWidth() / 200.0f);
		paintings_to_display.push_back(i);
		if (display_count % 10 == 0)
		{
			x_offset = 0.0f;
			z_offset -= 4.0f;
			previous_width = 0.0f;
			buffer = 0.0f;
		}

		else
			x_offset += buffer;

		previous_width = i->getWidth();
		i->getSurface()->moveAbsolute(glm::translate(mat4(1.0f), vec3(x_offset, y_offset, z_offset)));
		display_count++;
	}
	*/

	glfwSetTime(0);
	float render_fps = 60.0f;
	bool toggle_cam = false;

	cout << "data path: " << data_path << endl;

	do
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			camera->updateCamera();
			alt_camera->updateCamera();
			for (auto i : paintings_to_display)
			{
				if (toggle_cam)
					i->draw(context, alt_camera);

				else i->draw(context, camera);
			}

			//while the C key is being held, alt_cam is used instead of cam
			//detects to see which camera was used in previous frame
			bool initial_cam = toggle_cam;
			toggle_cam = keys->checkPress(GLFW_KEY_C);
			if (initial_cam != toggle_cam)
			{
				for (auto i : paintings_to_display)
					i->unloadData();
			}
			
			context->swapBuffers();
			glfwSetTime(0.0f);
		}

	} while (glfwGetKey(context->getWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		!glfwWindowShouldClose(context->getWindow()));

	for (auto i : paintings_to_display)
		i->unloadData();
}