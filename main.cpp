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
	string images_path = data_path + "images\\";

	string vert_file = data_path + "vertex_shader.glsl";
	string frag_file = data_path + "fragment_shader.glsl";

	
	shared_ptr<ogl_context> context(new ogl_context("Artfunkel", vert_file.c_str(), frag_file.c_str(), 1000, 800));
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<ogl_camera> camera(new ogl_camera_free(keys, vec3(0.0f, 1.0f, 3.0f)));

	art_db artist_database(artists_path.c_str(), paintings_path.c_str(), images_path.c_str(), context, camera);

	glfwSetTime(0);
	float render_fps = 60.0f;

	do
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			artist_database.getArtwork(2)->getSurface()->draw();

			context->swapBuffers();
			glfwSetTime(0.0f);
		}

	} while (glfwGetKey(context->getWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		!glfwWindowShouldClose(context->getWindow()));
}