#include "header.h"
#include "artist.h"
#include "artwork.h"
#include "art_db.h"
#include "utility_funcs.h"

int main()
{
	jep::init();

	int id_count = 0;

	string data_path("J:\\GitHub\\artfunkel\\");
	string paintings_path = data_path + "paintings.csv";
	string artists_path = data_path + "artists.csv";
	string images_path = data_path + "images\\paintings\\";

	string vert_file = data_path + "vertex_shader.glsl";
	string frag_file = data_path + "fragment_shader.glsl";

	
	shared_ptr<ogl_context> context(new ogl_context("Artfunkel", vert_file.c_str(), frag_file.c_str(), 1240, 960));
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<ogl_camera> camera(new ogl_camera_free(keys, vec3(0.0f, 0.0f, 5.0f)));

	art_db artist_database(artists_path.c_str(), paintings_path.c_str(), images_path.c_str(), context, camera);
	int art_count = artist_database.getArtworkCount();

	glfwSetTime(0);
	float render_fps = 60.0f;

	float x_offset = 0.0f;
	float z_offset = 0.0f;
	float previous_width = 0.0f;
	int display_count = 0;
	vector< shared_ptr<artwork> > paintings_to_display;

	for (int i = 0; i < art_count; i++)
	{
		shared_ptr<artwork> target = artist_database.getArtwork(i);
		if (target->getRarity() == LEGENDARY)
		{
			float buffer = (previous_width / 200.0f) + 1.0f + (target->getWidth() / 200.0f);
			artist_database.printArtwork(*target);
			paintings_to_display.push_back(target);
			if (display_count % 10 == 0)
			{
				x_offset = 0.0f;
				z_offset -= 4.0f;
				previous_width = 0.0f;
				buffer = 0.0f;
			}

			else
				x_offset += buffer;

			previous_width = target->getWidth();
			target->getSurface()->moveAbsolute(glm::translate(mat4(1.0f), vec3(x_offset, 0.0f, z_offset)));
			display_count++;
			cout << "buffer: " << buffer << endl;
		}
	}

	do
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			camera->updateCamera();
			for (auto i : paintings_to_display)
				i->getSurface()->draw();			

			context->swapBuffers();
			glfwSetTime(0.0f);
		}

	} while (glfwGetKey(context->getWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		!glfwWindowShouldClose(context->getWindow()));
}