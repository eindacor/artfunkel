#include "header.h"
#include "artist.h"
#include "artwork.h"
#include "art_db.h"
#include "utility_funcs.h"
#include "loot.h"

//this function takes a vector of instances and modifies their model matrices for proper display
void offsetArtworks(vector< shared_ptr<artwork_instance> > &art_vec, float eye_level, float starting_z = 0.0f)
{
	float x_offset = 0.0f;
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
			starting_z -= 4.0f;
			previous_width = 0.0f;
			buffer = 0.0f;
		}

		else
			x_offset += buffer;

		previous_width = i->getWidth();
		i->moveAbsolute(glm::translate(mat4(1.0f), vec3(x_offset, y_offset, starting_z)));
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

	int drop_count = 10;
	vector< shared_ptr<artwork_instance> > paintings_to_display = loot.generateArtworks(drop_count, 1.0f);
	offsetArtworks(paintings_to_display, eye_level);

	glfwSetTime(0);
	float render_fps = 60.0f;
	bool toggle_cam = false;

	cout << "data path: " << data_path << endl;

	//code below is for adding more paintings on the fly
	int add_wait = 30;
	int frame_count = 0;
	bool add_painting = false;

	for (int i = 0; i< 10; i++)
		cout << jep::intRoll(0, 10) << endl;

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


			add_painting = keys->checkPress(GLFW_KEY_J);
			if (add_painting)
			{
				if (frame_count == add_wait)
				{
					int add_count = 10;
					vector< shared_ptr<artwork_instance> > paintings_to_add = loot.generateArtworks(add_count, 1.0f);

					int painting_count = paintings_to_display.size();
					float new_z = (painting_count % 10 == 0 ?
						(painting_count / 10) * -4.0f : (painting_count / 10) + 1 * -4.0f);
					offsetArtworks(paintings_to_add, eye_level, new_z);
					paintings_to_display.insert(paintings_to_display.end(), paintings_to_add.begin(), paintings_to_add.end());
					frame_count = 0;
				}

				else frame_count++;
			}

			else frame_count = 0;
		}

	} while (glfwGetKey(context->getWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		!glfwWindowShouldClose(context->getWindow()));

	for (auto i : paintings_to_display)
		i->unloadData();
}