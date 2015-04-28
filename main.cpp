#include "header.h"
#include "artist.h"
#include "artwork.h"
#include "art_db.h"
#include "utility_funcs.h"

int main()
{
	jep::init();

	int id_count = 0;

	string data_path("C:\\Users\\Joseph\\Documents\\GitHub\\artfunkel\\");
	//string data_path("J:\\GitHub\\artfunkel\\");
	string paintings_path = data_path + "paintings.csv";
	string artists_path = data_path + "artists.csv";
	string images_path = data_path + "images\\paintings\\";

	string vert_file = data_path + "vertex_shader.glsl";
	string frag_file = data_path + "fragment_shader.glsl"; 

	float eye_level = 1.65f;
	//shared_ptr<ogl_context> context(new ogl_context("Artfunkel", vert_file.c_str(), frag_file.c_str(), 1240, 960));
	shared_ptr<ogl_context> context(new ogl_context("Artfunkel", vert_file.c_str(), frag_file.c_str(), 1020, 700));
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<ogl_camera> camera(new ogl_camera_free(keys, vec3(0.0f, eye_level, 5.0f)));

	shared_ptr<art_db> artist_database(new art_db(artists_path.c_str(), paintings_path.c_str(), images_path.c_str(), context, camera));
	int art_count = artist_database->getArtworkCount();

	glfwSetTime(0);
	float render_fps = 60.0f;

	float x_offset = 0.0f;
	float z_offset = 0.0f;
	float previous_width = 0.0f;
	int display_count = 0;
	vector< shared_ptr<artwork> > paintings_to_display;

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

	do
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			camera->updateCamera();
			//modify draw() functions to recieve a camera and context, to allow for toggling between each
			for (auto i : paintings_to_display)
				i->getSurface()->draw();			

			context->swapBuffers();
			glfwSetTime(0.0f);
		}

	} while (glfwGetKey(context->getWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		!glfwWindowShouldClose(context->getWindow()));
}