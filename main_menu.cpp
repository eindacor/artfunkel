#include "header.h"
#include "geometry.h"
#include "loot.h"
#include "player.h"
#include "artwork.h"
#include "utility_funcs.h"
#include "menus.h"
#include "gallery.h"
#include "hud.h"
#include "fileman.h"

int mainMenu(string data_path, const shared_ptr<ogl_context> &context, const shared_ptr<key_handler> &keys, const shared_ptr<text_handler> &text,
	shared_ptr<texture_handler> &textures, shared_ptr<player> &current_player)
{
	savePlayer(data_path, current_player->getName(), current_player);

	bool item_selected = false;
	float camera_distance_from_items = 10.0f;
	float menu_item_offset = 3.0f;
	float menu_item_width = 2.0f, menu_item_height = 2.0f;

	vec4 original_background = context->getBackgroundColor();
	context->setBackgroundColor(vec4(0.0f, 0.2f, 0.1f, 1.0f));

	map<int, pair<painting_surface, frame_model> > options;

	shared_ptr<ogl_camera> camera(new ogl_camera(keys, context, vec3(0.0f, 0.0f, camera_distance_from_items), vec3(0.0f, 0.0f, 0.0f), 45.0f));
	//TODO CLEAN UP THIS MESS 
	options.insert(pair<int, pair<painting_surface, frame_model> > (0, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\enter_gallery.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, "frame_black.bmp", "white_matte.bmp", textures)
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(1, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\view_inventory.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, "frame_black.bmp", "white_matte.bmp", textures)
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(2, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\open_crate.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, "frame_black.bmp", "white_matte.bmp", textures)
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(3, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\options.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, "frame_black.bmp", "white_matte.bmp", textures)
		)));

	options.insert(pair<int, pair<painting_surface, frame_model> >(4, pair<painting_surface, frame_model>(
		painting_surface(menu_item_width, menu_item_height, context, (data_path + "images\\menu_art\\exit.bmp").c_str()),
		frame_model(menu_item_width, menu_item_height, context, "frame_black.bmp", "white_matte.bmp", textures)
		)));

	int current_selection = 0;
	glfwSetTime(0);
	float render_fps = 60.0f;
	while (!item_selected)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			glfwPollEvents();
			context->clearBuffers();

			if (keys->checkPress(GLFW_KEY_LEFT, false) && current_selection != 0)
				current_selection--;

			else if (keys->checkPress(GLFW_KEY_RIGHT, false) && current_selection != (options.size() - 1))
				current_selection++;

			for (const auto &i : options)
			{
				float x_offset = (float(current_selection) - float(i.first)) * menu_item_offset * -1.0f;
				mat4 model_matrix(glm::translate(mat4(1.0f), vec3(x_offset, 0.0f, 0.0f)));

				if (i.first == current_selection)
				{
					model_matrix = glm::scale(mat4(1.0f), vec3(1.3f, 1.3f, 1.3f));
					i.second.first.draw(context, model_matrix, camera);
					i.second.second.draw(context, model_matrix, camera);			
				}

				else
				{
					glUniform1f(context->getShaderGLint("dim_factor"), 0.5f);
					i.second.first.draw(context, model_matrix, camera);
					i.second.second.draw(context, model_matrix, camera);
					glUniform1f(context->getShaderGLint("dim_factor"), 1.0f);
				}
			}

			item_selected = keys->checkPress(GLFW_KEY_ENTER, false);

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	context->setBackgroundColor(original_background);
	return current_selection;
}